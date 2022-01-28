#include "Utils.h"
#include "ApplicationEventBus.h"
#include "Localizer.h"
#include "Options.h"
#include "Core.h"
#include "MallocAllocator.hpp"
#include "PatchDecoder.hpp"
#include "PatchEncoder.hpp"
#include "GlobalVars.h"
#include "RangedDownload.h"
#include "Config.h"
#include <cstdlib>
#include "TaskQueue.h"
using namespace std;
int main_menu(void* data);
int choose_language(void* data);
void registerTasks();
int count=0;
int main(){
    setbuf(stdout, nullptr);
    //init and register event bus
    atexit(ApplicationEventBus::beforeClose);
    IOUtils::createDirIfNotExists("servers");
    IOUtils::createFileIfNotExists("blockstudio.config");
    auto *global=new Config("blockstudio.config");
    GlobalVars::setGlobalConfig(global);
    Log::INIT();
    RangedDownload::INIT();
    Localizer::getInstance()->setValue("OPTION_TIPS","");
    Localizer::getInstance()->setValue("OPTION_NOT_NUMBER","");
    registerTasks();
    TaskQueue::executeTask("choose_language",nullptr);
    TaskQueue::executeTask("main_menu", nullptr);
    while (TaskQueue::hasRemain()){
        TaskQueue::executeOneFromQueue();
    }
    return 0;
}
void registerTasks(){
    TaskQueue::addTask("choose_language",&choose_language);
    TaskQueue::addTask("main_menu",&main_menu);
    TaskQueue::addTask("deploy_server",&deploy_server);
    TaskQueue::addTask("deploy_vanilla",&deploy_vanilla);
    TaskQueue::addTask("deploy_bukkit_or_spigot",&deploy_bukkit_or_spigot);
    TaskQueue::addTask("deploy_forge",&deploy_forge);
    TaskQueue::addTask("deploy_fabric",&deploy_fabric);
    TaskQueue::addTask("start_server",start_server);
    TaskQueue::addTask("utils",&utils);
    TaskQueue::addTask("about",&about);
    TaskQueue::addTask("install_git",&install_git);
    TaskQueue::addTask("install_java",&install_java);
}
int choose_language(void* data){
    //choose language and load language file
    vector<string> path;
    int depth=1;
    int res=IOUtils::traverseDirectory("lang",path,depth,false,false);
    if (res==-1||path.size()==0){
        LOG_FATAL("%s","Error!Language files not exists.Please check the completeness.语言文件不存在，请检查文件完整性。");
        return -1;
    }
    Options choose_language;
    choose_language.setTitle("Please choose the language you want to use.请选择你想要使用的语言");
    for (int i = 0; i < path.size(); ++i) {
        choose_language.addOption(path[i].c_str());
    }
    int num;
    int ret=choose_language.getRetVal(num);
    if ((num+1)>path.size()||ret==-1){
        TaskQueue::executeTask("choose_language", nullptr);
        return 0;
    }
    Localizer::getInstance()->loadLanguageFile(path[num]);
    return 0;
}
int main_menu(void* data){
    int main_menu;
    Options options;
    options.setTitle(Localizer::getInstance()->getString("MAIN_MENU"));
    options.addOption(Localizer::getInstance()->getString("MAIN_MENU_FIRST"));
    options.addOption(Localizer::getInstance()->getString("MAIN_MENU_SECOND"));
    options.addOption(Localizer::getInstance()->getString("MAIN_MENU_THIRD"));
    options.addOption(Localizer::getInstance()->getString("MAIN_MENU_FOURTH"));
    options.addOption(Localizer::getInstance()->getString("MENU_EXIT"));
    int n=options.getRetVal(main_menu);
    if (n==-1){
        TaskQueue::executeTask("main_menu", nullptr);
        return 0;
    }
    switch (main_menu) {
        case 0:
        {
            TaskQueue::executeTask("deploy_server", nullptr);
            break;
        }
        case 1:
        {
            TaskQueue::executeTask("start_server", nullptr);
            break;
        }
        case 2:
        {
            TaskQueue::executeTask("utils", nullptr);
            break;
        }
        case 3:
        {
            TaskQueue::executeTask("about", nullptr);
            break;
        }
        case 4:
        {
            exit(0);
        }
        default:
        {
            TaskQueue::executeTask("main_menu", nullptr);
            break;
        }
    }
    return 0;
}