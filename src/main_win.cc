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
#include <tar.hpp>
using namespace std;
int mainMenu();
int chooseLanguage();
int main() {
    setbuf(stdout, nullptr);
    //init and register event bus
    atexit(ApplicationEventBus::beforeClose);
    IOUtils::createDirIfNotExists("servers");
    IOUtils::createFileIfNotExists("blockstudio.config");
    Config *global=new Config("blockstudio.config");
    GlobalVars::setGlobalConfig(global);
    Log::INIT();
    RangedDownload::INIT();
    Localizer::getInstance()->setValue("OPTION_TIPS","");
    Localizer::getInstance()->setValue("OPTION_NOT_NUMBER","");
    chooseLanguage();
    return mainMenu();

}
int chooseLanguage(){
    //choose language and load language file
    vector<string> path;
    int depth=1;
    int res=IOUtils::traverseDirectory("lang",path,depth,false);
    if (res==-1||path.size()==0){
        LOG_FATAL("%s","Error!Language files not exists.Please check the completeness.语言文件不存在，请检查文件完整性。");
        return -1;
    }
    Options choose_language;
    choose_language.setTitle("Please choose the language you want to use 请选择你想要使用的语言");
    for (int i = 0; i < path.size(); ++i) {
        choose_language.addOption(path[i].c_str());
    }
    int num;
    int ret=choose_language.getRetVal(num);
    if ((num+1)>path.size()||ret==-1){
        chooseLanguage();
        return false;
    }
    Localizer::getInstance()->loadLanguageFile(path[num]);
    return 0;
}
int mainMenu(){
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
        mainMenu();
        return 0;
    }
    int ret;
    switch (main_menu) {
        case 0:
        {
            ret=Core::getInstance()->deployServer();
            break;
        }
        case 1:
        {
            ret=Core::getInstance()->launchServer();
            break;
        }
        case 2:
        {
            ret=Core::getInstance()->utils();
            break;
        }
        case 3:
        {
            ret=Core::getInstance()->about();
            break;
        }
        case 4:
        {
            exit(0);
        }
        default:
        {
            ret=1;
            break;
        }
    }
    switch (ret) {
        case 0:
            return 0;
        case -1:
            return -1;
        case 1:
        {
            mainMenu();
            break;
        }
    }
    return 0;
}