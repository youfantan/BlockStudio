#include "Core.h"
#include "Utils.h"
#include "Localizer.h"
#include "Options.h"
#include "Config.h"
#include "GlobalVars.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "RangedDownload.h"
#include "TaskQueue.h"
#include <sstream>
#include <cstring>
#include <iostream>
#include <direct.h>
#include <algorithm>

using namespace rapidjson;
/*
Return value of Core.cc functions
 0:Normal exit
 -1:Abnormal exit
 1:Return to last step
*/

using namespace std;
void print_lcs(vector< vector<char>> &b,string x, int i, int j, vector<char> &str)
{
    if(i == 0 || j == 0)
        return;
    if(b[i][j] == 'c'){
        str.push_back(x[i-1]);
        print_lcs(b,x,i-1,j-1,str);
    }else if(b[i][j] == 'u')
        print_lcs(b,x,i-1,j,str);
    else
        print_lcs(b,x,i,j-1,str);
}
void lcsMatrix(string x,string y, vector< vector<int>> &c, vector< vector<char>> &b)
{
    int m = x.size();
    int n = y.size();
    c.resize(m+1);
    b.resize(m+1);
    for(int i = 0; i < c.size(); ++i)
        c[i].resize(n+1);
    for(int i = 0; i < b.size(); ++i)
        b[i].resize(n+1);

    for(int i = 1; i <= m; ++i){
        for(int j = 1; j <= n; ++j){
            if(x[i-1] == y[j-1]){
                c[i][j] = c[i-1][j-1]+1;
                b[i][j] = 'c';
            }else if(c[i-1][j] >= c[i][j-1]){
                c[i][j] = c[i-1][j];
                b[i][j] ='u';
            }else{
                c[i][j] = c[i][j-1];
                b[i][j] = 'l';
            }
        }
    }
}
//use this function to output the matrix
void outMatrix(vector<vector<int>> &a){
    for (int i = 0; i < a.size(); ++i) {
        vector<int> v=a[i];
        for (int j = 0; j < v.size(); ++j) {
            printf("%d ",v[i]);
        }
        printf("\n");
    }
}
void Core::lcs(std::string a, std::string b, std::string &c, std::vector<std::vector<int>> d, std::vector<std::vector<char>> e) {
    lcsMatrix(a,b,d,e);
    vector<char> vct;
    //outMatrix(d);
    print_lcs(e,a,a.length(),b.length(),vct);
    if (!vct.empty()){
        for (auto i=vct.end()-1;i>=vct.begin();i--){
            c+=*i;
        }
    }
}

void Core::makePatch(std::string a,std::string b,std::string &c) {
    FILE *f= fopen(a.c_str(),"rb");
    FILE *f1= fopen(b.c_str(),"rb");
    long len;
    long len1;
    fseek(f,0,SEEK_END);
    fseek(f1,0,SEEK_END);
    len= ftell(f);
    len1= ftell(f1);
    fseek(f,0,SEEK_SET);
    fseek(f1,0,SEEK_SET);
    vector<vector<int>> d;
    vector<vector<char>> e;
    char buf[1024]={0};
    char buf1[1024]={0};
    string lcs_str;
    char *fullbuf=new char[len];
    char *fullbuf1=new char[len1];
    while (fgets(buf, 1024,f)&& fgets(buf1,1024,f1)){
        string dest;
        lcs(buf,buf1,dest,d,e);
        lcs_str.append(dest);
        strcat(fullbuf,buf);
        strcat(fullbuf1,buf1);
    }
    for (int i = 0; i < len;++i) {
        if (fullbuf1[i]==fullbuf[i]){
            c+=0x01;
        } else{
            c+=fullbuf1[i];
        }
    }
    fclose(f);
    fclose(f1);
}

void Core::mergePatch(std::string p, std::string o, string &dest) {
    ostringstream buf1;
    ostringstream buf2;
    ifstream i1(p.c_str(),ios::in|ios::binary);
    ifstream i2(o.c_str(),ios::in|ios::binary);
    buf1<<i1.rdbuf();
    buf2<<i2.rdbuf();
    string s1=buf1.str();
    string s2=buf2.str();
    for (int i = 0; i < s1.size(); ++i) {
        if (p.c_str()[i]==0x01){
            dest+=s2.c_str()[i];
        } else{
            dest+=s1.c_str()[i];
        }
    }
    i1.close();
    i2.close();
}

int deploy_server(void *data){
    Options option;
    option.setTitle(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_FIRST"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_SECOND"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_THIRD"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_FOURTH"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_FIFTH"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    int n=option.getRetVal(ret);
    if (n==-1){
        TaskQueue::executeTask("deploy_server",nullptr);
        return 0;
    }
    switch (ret) {
        case 0:
        {
            TaskQueue::executeTask("deploy_vanilla", nullptr);
            return 0;
        }
        case 1:
        {
            TaskQueue::executeTask("deploy_bukkit_or_spigot",nullptr);
            return 0;
        }
        case 2:
        {
            TaskQueue::executeTask("deploy_bukkit_or_spigot",nullptr);
            return 0;
        }
        case 3:
        {
            TaskQueue::executeTask("deploy_bukkit_or_spigot",nullptr);
            return 0;
        }
        case 4:
        {
            TaskQueue::executeTask("deploy_bukkit_or_spigot",nullptr);
            return 0;
        }
        case 5:
        {
            TaskQueue::executeTask("main_menu",nullptr);
            return 0;
        }
        default:
        {
            TaskQueue::executeTask("deploy_server",nullptr);
            return 0;
        }
    }
}
int start_server(void* data){
    return 0;
}
extern char _binary_THIRD_PARTY_LICENCE_start[];
extern char _binary_THIRD_PARTY_LICENCE_end[];
int extractLicence(){
    char *p = _binary_THIRD_PARTY_LICENCE_start;
    FILE *f= fopen("THIRD_PARTY_LICENCE","wb+");
    while(p != _binary_THIRD_PARTY_LICENCE_end){
        fwrite(p, 1,1,f);
        p++;
    }
    fclose(f);
    return 0;
}
int utils(void* data){
    Options option;
    option.setTitle(Localizer::getInstance()->getString("CORE_UTILS"));
    option.addOption(Localizer::getInstance()->getString("CORE_UTILS_FIRST"));
    option.addOption(Localizer::getInstance()->getString("CORE_UTILS_SECOND"));
    option.addOption(Localizer::getInstance()->getString("CORE_UTILS_THIRD"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    int n=option.getRetVal(ret);
    if(n!=0){
        TaskQueue::executeTask("utils", nullptr);
    }
    switch (ret) {
        case 0:
        {
            return 0;
        }
        case 1:
        {
            return 0;
        }
        case 2:
        {
            return 0;
        }
        case 3:
        {
            TaskQueue::executeTask("main_menu", nullptr);
        }
        default:
        {
            TaskQueue::executeTask("utils",nullptr);
            return 0;
        }
    }
}
int about(void* data){
    char licence[40000]={0};
    FILE *f= fopen("THIRD_PARTY_LICENCE","r");
    if (!f){
        extractLicence();
        fclose(f);
    }
    IOUtils::readFile("THIRD_PARTY_LICENCE",licence);
    printf("%s\n%s\n%s\n",Localizer::getInstance()->getString("ABOUT").c_str(),Localizer::getInstance()->getString("THIRD_PARTY_LICENCE").c_str(),licence);
    TaskQueue::executeTask("main_menu", nullptr);
    return 0;
}
int deploy_vanilla(void *data){
    std::string mirrorType=GlobalVars::getGlobalConfig()->getString("UseMirror");
    std::string manifest_url=VERSION_MANIFEST_URL;
    std::string prefix;
    std::string meta_prefix;
    if (mirrorType=="OFFICIAL"){
        prefix=OFFICIAL_URL;
        meta_prefix=OFFICIAL_META_URL;
    } else if (mirrorType=="MCBBS"){
        prefix=MCBBS_URL;
        meta_prefix=MCBBS_URL;
    } else if (mirrorType=="BMCLAPI"){
        prefix=BMCLAPI_URL;
        meta_prefix=BMCLAPI_URL;
    }else {
        //by default
        prefix = OFFICIAL_URL;
        meta_prefix=OFFICIAL_META_URL;
    }
    manifest_url.replace(0,31,meta_prefix);
    Options option;
    option.setTitle(Localizer::getInstance()->getString("DEPLOY_SERVER_VANILLA"));
    long manifest_length=IOUtils::getContentLength(manifest_url.c_str());
    char *manifest_content=(char*) malloc(sizeof(char)*manifest_length+1);
    IOUtils::download(manifest_url.c_str(),manifest_content);
    Document manifest_object;
    manifest_object.Parse(manifest_content);
    const Value& latest=manifest_object["latest"];
    std::string latest_release=latest["release"].GetString();
    std::string latest_snapshot=latest["snapshot"].GetString();
    const Value& versions=manifest_object["versions"];
    std::map<int ,std::string> version_map;
    std::map<std::string ,std::string> url_map;
    int count=0;
    bool enableUnstable;
    if (GlobalVars::getGlobalConfig()->getString("UseUnstableVersions")=="false"){
        enableUnstable=false;
    } else{
        enableUnstable=true;
    }
    LOG_INFO("%s",GlobalVars::getGlobalConfig()->getString("UseUnstableVersions").c_str());
    for (auto& version:versions.GetArray()){
        char option_value[64]={0};
        std::string type=version["type"].GetString();
        sprintf(option_value,"%s (%s) (%s)",version["id"].GetString(),version["type"].GetString(),version["releaseTime"].GetString());
        if (type!="release"&&!enableUnstable){
            continue;
        }
        option.addOption(option_value);
        version_map[count]=version["id"].GetString();
        url_map[version["id"].GetString()]=version["url"].GetString();
        count++;
    }
    option.addOption(Localizer::getInstance()->getString("DEPLOY_SERVER_VANILLA_LATEST_RELEASE"));
    option.addOption(Localizer::getInstance()->getString("DEPLOY_SERVER_VANILLA_LATEST_SNAPSHOT"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    int n=option.getRetVal(ret);
    if (n==-1){
        TaskQueue::executeTask("deploy_vanilla", nullptr);
        return 0;
    }
    const char* chooseVersion;
    if (ret==versions.Size()+2)
    {
        TaskQueue::executeTask("deploy_server", nullptr);
        return 0;
    }
    else if (ret==versions.Size()+1)
        chooseVersion=latest_snapshot.c_str();
    else if (ret==versions.Size())
        chooseVersion=latest_release.c_str();
    else if (ret>versions.Size()+2)
    {
        TaskQueue::executeTask("deploy_vanilla", nullptr);
        return 0;
    }
    else
    {
        chooseVersion=version_map[ret].c_str();
    }
    LineOptions getName;
    getName.setTitle(Localizer::getInstance()->getString("DEPLOY_SERVER_CHOOSE_ARCHIVE_NAME"));
    getName.setErrorTips(Localizer::getInstance()->getString("DEPLOY_SERVER_NAME_EXISTS"));
    std::string name;
    getName.getInput(name,[](std::string in)->bool{
        std::string path="servers/"+in;
        if (IOUtils::dirExists(path.c_str())!=0){
            return false;
        } else {
            return true;
        }
    });
    std::string url=url_map[chooseVersion];
    std::string path="servers/"+name;
    IOUtils::createDir(path.c_str());
    std::string serverConfig=path+"/config.json";
    IOUtils::createFile(serverConfig.c_str());
    std::string serverJson= path + "/server.json";
    IOUtils::createFile(serverJson.c_str());
    std::string serverJar= path + "/server.jar";
    IOUtils::createFile(serverJson.c_str());
    url=url.replace(0,31,meta_prefix);
    long meta_length=IOUtils::getContentLength(url.c_str());
    char* version_meta=(char*) malloc(sizeof(char)*meta_length+1);
    IOUtils::download(url.c_str(),version_meta);
    IOUtils::writeFile(serverJson.c_str(),version_meta);
    Document version_object;
    version_object.Parse(version_meta);
    Value& javaVersion=version_object["javaVersion"];
    char required_java_version[4];
    itoa(javaVersion["majorVersion"].GetInt(),required_java_version,10);
    pthread_t tid=TaskQueue::executeTaskAsync("install_java",(void*)required_java_version);
    pthread_join(tid,nullptr);
    Value& downloads=version_object["downloads"];
    Value& server=downloads["server"];
    std::string downloadUrl=server["url"].GetString();
    const char* sha1=server["sha1"].GetString();
    downloadUrl=downloadUrl.replace(0,27,prefix);
    RangedDownload download(downloadUrl.c_str(), serverJar.c_str(), GlobalVars::getGlobalConfig()->getInt("ThreadSize"));
    download.execute([](std::vector<node*> nodes,struct info *i)->void{
        double total=(double)i->total/1048576;
        double downloaded=(double)i->downloaded/1048576;
        LOG_INFO(Localizer::getInstance()->getString("RANGED_DOWNLOAD_INFO").c_str(),i->progress*100,i->speed,total,downloaded);
    });
    std::vector<std::string> availableJavaPaths=GlobalVars::getAvailableJavaPaths();
    std::string java_path;
    if (availableJavaPaths.size()>1){
        //judge which jre to use
        Options use_java_version;
        use_java_version.setTitle(Localizer::getInstance()->getString("DEPLOY_BUKKIT_SPIGOT_CHOOSE_JAVA_VERION"));
        for (auto it=availableJavaPaths.begin();it!=availableJavaPaths.end();++it){
            use_java_version.addOption(*it);
        }
        int java_version;
        int n1=use_java_version.getRetVal(java_version);
        if (n1!=0 || java_version > availableJavaPaths.size() - 1){
            TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
            return 0;
        }
        java_path=availableJavaPaths[java_version];
    } else{
        java_path=availableJavaPaths[0];
    }
    char destSHA1[41]={0};
    FileEncoder::GetFileSHA1(serverJar.c_str(),destSHA1);
    if (strcmp(destSHA1,sha1)!=0){
        LOG_ERROR(Localizer::getInstance()->getString("DEPLOY_SERVER_ERROR_SHA1").c_str(),destSHA1,sha1);
        return -1;
    }
    StringBuffer buffer;
    Writer<StringBuffer> info_writer(buffer);
    info_writer.StartObject();
    info_writer.Key("name");
    info_writer.String(name.c_str());
    info_writer.Key("version");
    info_writer.String(chooseVersion);
    info_writer.Key("url");
    info_writer.String(url.c_str());
    info_writer.Key("deployTime");
    info_writer.String(Log::getTime("%d-%02d-%02d %02d:%02d:%02d"));
    info_writer.Key("SHA1");
    info_writer.String(sha1);
    info_writer.Key("type");
    info_writer.String("vanilla");
    info_writer.Key("java_path");
    info_writer.String(java_path.c_str());
    info_writer.EndObject();
    const char* info=buffer.GetString();
    IOUtils::writeFile(serverConfig.c_str(),info);
    LOG_INFO("%s",Localizer::getInstance()->getString("DEPLOY_SERVER_SUCCESSFUL").c_str());
    free(manifest_content);
    free(version_meta);
    TaskQueue::executeTask("main_menu", nullptr);
    return 0;
}

//This function can be very slow.Can you thought some ways to optimizing?Post it on Pull Request.
void procApiHtmlSource(char* content,vector<std::string> &dest){
    for (int i = 0; i < strlen(content); ++i) {
        if ((content[i]=='h'&&content[i+1]=='r')&&(content[i+2]=='e'&&content[i+3]=='f')){
            std::string name;
            int j=1;
            int point_count=0;
            bool is_version;
            char c;
            while ((c=content[i+5+j])!='\"'){
                if (c=='/'){
                    is_version= false;
                } else{
                    is_version= true;
                }
                if (content[i+5+j]=='.'){
                    point_count++;
                }
                name+=content[i+5+j];
                j++;
            }
            i=i+5+j;
            if (point_count>1&&is_version){
                dest.push_back(name);
            }
        }
    }
}
int deploy_bukkit_or_spigot(void *data){
    Options option;
    option.setTitle(Localizer::getInstance()->getString("DEPLOY_SERVER_CRAFTBUKKIT_SPIGOT_CHOOSE_BUILD"));
    const std::string buildToolsApiUrl="https://hub.spigotmc.org/jenkins/job/BuildTools/api/json?tree=allBuilds[id,fullDisplayName,result,timestamp,url]";
    //long apiLength=IOUtils::getContentLength(buildToolsApiUrl.c_str());
    //The fuck jenkins didn't return content-length so I have to alloc it manually
    char *apiContent=(char*) malloc(sizeof(char)*51200);
    IOUtils::download(buildToolsApiUrl.c_str(),apiContent);
    Document apiObj;
    std::map<std::string,std::string> buildUrlMap;
    std::map<int,std::string> buildIdMap;
    apiObj.Parse(apiContent);
    Value &allBuilds=apiObj["allBuilds"];
    int count=0;
    std::string latestSuccessful;
    for (auto &build:allBuilds.GetArray()) {
        Value &build_obj=build;
        std::string result=build_obj["result"].GetString();
        int64_t time=build_obj["timestamp"].GetInt64();
        time_t t=time/1000;
        tm *pt= localtime(&t);
        char time_c[32]={0};
        strftime(time_c,32,"%Y-%m-%d %H:%M:%S",pt);
        const char *name=build_obj["fullDisplayName"].GetString();
        buildUrlMap[build_obj["id"].GetString()]=build_obj["url"].GetString();
        if(latestSuccessful.empty()&&result=="SUCCESS")
            latestSuccessful=build_obj["id"].GetString();
        buildIdMap[count]=build_obj["id"].GetString();
        char opt[128]={0};
        sprintf(opt,"%s (%s) %s",name,time_c,result.c_str());
        option.addOption(opt);
        count++;
    }
    option.addOption(Localizer::getInstance()->getString("DEPLOY_SERVER_CRAFTBUKKIT_SPIGOT_LATEST_BUILD"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    int n=option.getRetVal(ret);
    if (n==-1){
        TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
        return 0;
    }
    std::string chooseBuild;
    if (ret==allBuilds.Size()+1){
        TaskQueue::executeTask("deploy_server", nullptr);
        return 0;
    } else if (ret==allBuilds.Size()){
        chooseBuild=latestSuccessful;
    } else if (ret>allBuilds.Size()+1){
        TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
        return 0;
    } else{
        chooseBuild=buildIdMap[ret];
    }
    LineOptions getName;
    getName.setTitle(Localizer::getInstance()->getString("DEPLOY_SERVER_CHOOSE_ARCHIVE_NAME"));
    getName.setErrorTips(Localizer::getInstance()->getString("DEPLOY_SERVER_NAME_EXISTS"));
    std::string name;
    getName.getInput(name,[](std::string in)->bool{
        std::string path="servers/"+in;
        if (IOUtils::dirExists(path.c_str())!=0){
            return false;
        } else {
            return true;
        }
    });
    std::string path="servers/"+name;
    IOUtils::createDir(path.c_str());
    std::string serverConfig=path+"/config.json";
    IOUtils::createFile(serverConfig.c_str());
    std::string buildToolJar= path + "/BuildTools.jar";
    std::string buildToolsUrl=buildUrlMap[chooseBuild]+"artifact/target/BuildTools.jar";
    RangedDownload buildToolsDownloader(buildToolsUrl.c_str(),buildToolJar.c_str(),16);
    buildToolsDownloader.execute([](std::vector<node*> nodes,struct info *i)->void{
        double total=(double)i->total/1048576;
        double downloaded=(double)i->downloaded/1048576;
        LOG_INFO(Localizer::getInstance()->getString("RANGED_DOWNLOAD_INFO").c_str(),i->progress*100,i->speed,total,downloaded);
    });
    char* versions=(char*) malloc(sizeof(char)*2000000);
    IOUtils::download("https://hub.spigotmc.org/versions/",versions);
    std::vector<std::string> jsonUrls;
    LOG_INFO("%s",Localizer::getInstance()->getString("DEPLOY_SERVER_CRAFTBUKKIT_SPIGOT_WAIT").c_str());
    procApiHtmlSource(versions,jsonUrls);
    Options version_option;
    version_option.setTitle(Localizer::getInstance()->getString("DEPLOY_SERVER_CRAFTBUKKIT_SPIGOT_CHOOSE_VERSION"));
    for (auto it=jsonUrls.begin();it!=jsonUrls.end();++it){
        std::string displayName=it->substr(0,it->size()-5);
        version_option.addOption(displayName);
    }
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int vRet;
    int vN=version_option.getRetVal(vRet);
    if (vN!=0||vRet>jsonUrls.size()){
        TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
        return 0;
    }
    if (vRet==jsonUrls.size()){
        TaskQueue::executeTask("deploy_server", nullptr);
        return 0;
    }
    std::string useVersion=jsonUrls[vRet].substr(0,jsonUrls[vRet].size()-5);
    std::string mirrorType=GlobalVars::getGlobalConfig()->getString("UseMirror");
    std::string manifest_url=VERSION_MANIFEST_URL;
    std::string prefix;
    std::string meta_prefix;
    if (mirrorType=="OFFICIAL"){
        prefix=OFFICIAL_URL;
        meta_prefix=OFFICIAL_META_URL;
    } else if (mirrorType=="MCBBS"){
        prefix=MCBBS_URL;
        meta_prefix=MCBBS_URL;
    } else if (mirrorType=="BMCLAPI"){
        prefix=BMCLAPI_URL;
        meta_prefix=BMCLAPI_URL;
    }else {
        //by default
        prefix = OFFICIAL_URL;
        meta_prefix=OFFICIAL_META_URL;
    }
    manifest_url.replace(0,31,meta_prefix);
    long manifest_length=IOUtils::getContentLength(manifest_url.c_str());
    char *manifest_content=(char*) malloc(sizeof(char)*manifest_length+1);
    IOUtils::download(manifest_url.c_str(),manifest_content);
    Document manifest_object;
    manifest_object.Parse(manifest_content);
    const Value& latest=manifest_object["latest"];
    std::string latest_release=latest["release"].GetString();
    std::string latest_snapshot=latest["snapshot"].GetString();
    const Value& oversions=manifest_object["versions"];
    std::map<int ,std::string> version_map;
    std::map<std::string ,std::string> url_map;
    int vcount=0;
    for (auto& version:oversions.GetArray()) {
        std::string type = version["type"].GetString();
        version_map[count] = version["id"].GetString();
        url_map[version["id"].GetString()] = version["url"].GetString();
        vcount++;
    }
    std::string url=url_map[useVersion];
    std::string vpath="servers/"+name;
    IOUtils::createDir(vpath.c_str());
    std::string serverJson= vpath + "/server.json";
    IOUtils::createFile(serverJson.c_str());
    std::string workDir=vpath+"/work";
    IOUtils::createDir(workDir.c_str());
    std::string serverJar= vpath + "/work/minecraft_server."+useVersion+".jar";
    IOUtils::createFile(serverJson.c_str());
    url=url.replace(0,31,meta_prefix);
    long meta_length=IOUtils::getContentLength(url.c_str());
    char* version_meta=(char*) malloc(sizeof(char)*meta_length+1);
    IOUtils::download(url.c_str(),version_meta);
    IOUtils::writeFile(serverJson.c_str(),version_meta);
    Document version_object;
    version_object.Parse(version_meta);
    Value& javaVersion=version_object["javaVersion"];
    char required_java_version[4];
    itoa(javaVersion["majorVersion"].GetInt(),required_java_version,10);
    pthread_t tid=TaskQueue::executeTaskAsync("install_java",(void*)required_java_version);
    pthread_join(tid,nullptr);
    Value& downloads=version_object["downloads"];
    Value& server=downloads["server"];
    std::string downloadUrl=server["url"].GetString();
    const char* sha1=server["sha1"].GetString();
    downloadUrl=downloadUrl.replace(0,27,prefix);
    RangedDownload serverDownloader(downloadUrl.c_str(), serverJar.c_str(), GlobalVars::getGlobalConfig()->getInt("ThreadSize"));
    serverDownloader.execute([](std::vector<node*> nodes, struct info *i)->void{
        double total=(double)i->total/1048576;
        double downloaded=(double)i->downloaded/1048576;
        LOG_INFO(Localizer::getInstance()->getString("RANGED_DOWNLOAD_INFO").c_str(),i->progress*100,i->speed,total,downloaded);
    });
    pthread_t task_install_git=TaskQueue::executeTaskAsync("install_git", nullptr);
    pthread_join(task_install_git,nullptr);
    char destSHA1[41]={0};
    FileEncoder::GetFileSHA1(serverJar.c_str(),destSHA1);
    if (strcmp(destSHA1,sha1)!=0){
        LOG_ERROR(Localizer::getInstance()->getString("DEPLOY_SERVER_ERROR_SHA1").c_str(),destSHA1,sha1);
        TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
        return -1;
    }
    char current[255]={0};
    getcwd(current,255);
    std::string serverDir(current);
    serverDir=serverDir+"\\servers\\"+name+"\\";
#ifdef LINUX
    replace(workDir.begin(),workDir.end(),'\\','/');
#endif
    chdir(serverDir.c_str());
    std::string command;
#ifdef LINUX
    command="java -jar BuildTools.jar --rev "+useVersion;
#endif
    std::vector<std::string> availableJavaPaths=GlobalVars::getAvailableJavaPaths();
    std::string java_path;
    if (availableJavaPaths.size()>1){
        //judge which java to use
        Options use_java_version;
        use_java_version.setTitle(Localizer::getInstance()->getString("DEPLOY_BUKKIT_SPIGOT_CHOOSE_JAVA_VERION"));
        for (auto it=availableJavaPaths.begin();it!=availableJavaPaths.end();++it){
            use_java_version.addOption(*it);
        }
        int java_version;
        int n1=use_java_version.getRetVal(java_version);
        if (n1!=0 || java_version > availableJavaPaths.size() - 1){
            TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
            return 0;
        }
        java_path=availableJavaPaths[java_version];
    } else{
        java_path=availableJavaPaths[0];
    }
#ifdef WINDOWS
    std::vector<std::string> gitEnv=GlobalVars::getAvailableGitPaths();
    if (gitEnv.size()>1){
        //judge which git to use
        Options use_git_version;
        use_git_version.setTitle(Localizer::getInstance()->getString("DEPLOY_BUKKIT_SPIGOT_CHOOSE_GIT_VERION"));
        for (auto it=gitEnv.begin();it!=gitEnv.end();++it){
            use_git_version.addOption(*it);
        }
        int git_version;
        int n1=use_git_version.getRetVal(git_version);
        if (n1!=0||git_version>gitEnv.size()-1){
            TaskQueue::executeTask("deploy_bukkit_or_spigot", nullptr);
            return 0;
        }
        std::string git_path=gitEnv[git_version];
        command=git_path+R"(\bin\bash.exe -c ./build.sh)";
        //write in build command file
        std::string shContent;
        shContent.append("export SHELL="+git_path+"\\bin\\sh.exe\n");
        shContent.append("export PATH=/usr/local/bin:$PATH\n");
        shContent.append(java_path+"\\bin\\java.exe -Xms512M -Xmx2048M -jar BuildTools.jar --rev "+useVersion);
        replace(shContent.begin(),shContent.end(),'\\','/');
        IOUtils::writeFile("build.sh",shContent.c_str());
    } else{
        std::string git_path=gitEnv[0];
        command=git_path+R"(\bin\bash.exe -c ./build.sh)";
        //write in build command file
        std::string shContent;
        shContent.append("export SHELL="+git_path+"\\bin\\sh.exe\n");
        shContent.append("export PATH=/usr/local/bin:$PATH\n");
        shContent.append(java_path+"\\bin\\java.exe -jar BuildTools.jar --rev "+useVersion);
        replace(shContent.begin(),shContent.end(),'\\','/');
        IOUtils::writeFile("build.sh",shContent.c_str());
    }
#endif
    system(command.c_str());
    chdir(current);
    StringBuffer buffer;
    Writer<StringBuffer> info_writer(buffer);
    info_writer.StartObject();
    info_writer.Key("name");
    info_writer.String(name.c_str());
    info_writer.Key("version");
    info_writer.String(useVersion.c_str());
    info_writer.Key("url");
    info_writer.String(url.c_str());
    info_writer.Key("deployTime");
    info_writer.String(Log::getTime("%d-%02d-%02d %02d:%02d:%02d"));
    info_writer.Key("SHA1");
    info_writer.String(sha1);
    info_writer.Key("type");
    info_writer.String("bukkit/spigot");
    info_writer.String("java_path");
    info_writer.String(java_path.c_str());
    info_writer.EndObject();
    const char* info=buffer.GetString();
    IOUtils::writeFile(serverConfig.c_str(),info);
    LOG_INFO("%s",Localizer::getInstance()->getString("DEPLOY_SERVER_SUCCESSFUL").c_str());
    free(manifest_content);
    free(version_meta);
    TaskQueue::executeTask("main_menu", nullptr);
    return 0;
}
int deploy_forge(void *data){
    return 0;
}
int deploy_fabric(void *data){
    return 0;
}
extern char _binary_resource_list_json_start[];
extern char _binary_resource_list_json_end[];
int extractResourceList(){
    char *p = _binary_resource_list_json_start;
    FILE *f= fopen("resource_list.json","wb+");
    while(p != _binary_resource_list_json_end){
        fwrite(p, 1,1,f);
        p++;
    }
    fclose(f);
    return 0;
}
extern char _binary_java_details_jar_start[];
extern char _binary_java_details_jar_end[];
int extractJavaDetails(){
    char *p = _binary_java_details_jar_start;
    FILE *f= fopen("java_details.jar","wb+");
    while(p != _binary_java_details_jar_end){
        fwrite(p, 1,1,f);
        p++;
    }
    fclose(f);
    return 0;
}
int install_git(void* data){
    std::vector<std::string> gits;
    std::vector<std::string> availableGitPaths;
    EnvironmentUtils::whereGitExists(gits);
    bool haveSh=false;
    for (auto it=gits.begin();it!=gits.end();++it){
        std::string shDir=*it+"\\bin\\sh.exe";
        FILE *f= fopen(shDir.c_str(),"r");
        if (f){
            availableGitPaths.push_back(*it);
            haveSh=true;
            break;
        }
    }
    if (gits.empty()||!haveSh){
#ifdef WINDOWS
        LOG_INFO("%s",Localizer::getInstance()->getString("INSTALL_GIT_NOT_FOUND_WINDOWS").c_str());
        //start install git
        IOUtils::createDirIfNotExists("git");
        FILE *f= fopen("resource_list.json","r");
        if (!f){
            extractResourceList();
        }
        size_t length=IOUtils::getFileContentLength("resource_list.json");
        char* content=(char*) malloc(sizeof(char)*length+1);
        IOUtils::readFile("resource_list.json",content);
        Document document;
        document.Parse(content);
        Value& git_for_windows=document["git_for_windows"];
        const char *downloadUrl;
        if(EnvironmentUtils::isX64()){
            Value& x64=git_for_windows["x64"];
            downloadUrl=x64[0].GetString();
            LOG_INFO("%s",downloadUrl);
        } else {
            Value& x86=git_for_windows["x86"];
            downloadUrl=x86[0].GetString();
            LOG_INFO("%s",downloadUrl);
        }
        char dest[64]={0};
        FileEncoder::GetCacheName(dest);
        IOUtils::createFile(dest);
        RangedDownload serverDownloader(downloadUrl, dest, GlobalVars::getGlobalConfig()->getInt("ThreadSize"));
        serverDownloader.execute([](std::vector<node*> nodes, struct info *i)->void{
            double total=(double)i->total/1048576;
            double downloaded=(double)i->downloaded/1048576;
            LOG_INFO(Localizer::getInstance()->getString("RANGED_DOWNLOAD_INFO").c_str(),i->progress*100,i->speed,total,downloaded);
        });
        LOG_INFO("%s",Localizer::getInstance()->getString("INSTALL_GIT_UNCOMPRESS").c_str());
        FileEncoder::BZip2Decompress(dest,"git/");
        LOG_INFO("%s",Localizer::getInstance()->getString("INSTALL_GIT_DONE_WINDOWS").c_str());
        char cwd[255]={0};
        getcwd(cwd,255);
        strcat(cwd,"\\git");
        availableGitPaths.emplace_back(cwd);
#endif
#ifdef LINUX
        LOG_FATAL("%s",Localizer::getInstance()->getString("INSTALL_GIT_NOT_FOUND_LINUX").c_str());
        exit(0);
#endif
    }
    GlobalVars::setAvailableGitPaths(availableGitPaths);
    return 0;
}
int startsWith(string s, string sub){
    return s.find(sub)==0?1:0;
}
int install_java(void* data){
#ifdef WINDOWS
    char *version=(char*)data;
    FILE *f= fopen("java_details.json","r");
    if (!f){
        extractJavaDetails();
    }
    fclose(f);
    std::vector<std::string> javas;
    std::vector<std::string> availableJavaPaths;
    EnvironmentUtils::whereJavaExists(javas);
    bool hasLimitedJavaVersion= false;
    for (auto it = javas.begin();it!=javas.end(); it++) {
        std::string command = *it + " -jar java_details.jar";
        system(command.c_str());
        FILE *f = fopen("java_details.json", "r");
        if (!f) {
            continue;
        }
        size_t length = IOUtils::getFileContentLength("java_details.json");
        char *content = (char *) malloc(sizeof(char) * length + 1);
        IOUtils::readFile("java_details.json", content);
        Document java_details;
        java_details.Parse(content);
        std::string java_version = java_details["JavaVersion"].GetString();
        std::string required_java_version(version);
        if (startsWith(java_version, "1.8")) {
            if (stoi(required_java_version) != 8) {
                continue;
            }
        } else if (!startsWith(java_version, required_java_version)) {
            continue;
        }
        availableJavaPaths.emplace_back(java_details["JrePath"].GetString());
        hasLimitedJavaVersion = true;
    }
    if (!hasLimitedJavaVersion){
        LOG_INFO("%s",Localizer::getInstance()->getString("INSTALL_JAVA_NOT_FOUND_WINDOWS").c_str());
        //start install git
        IOUtils::createDirIfNotExists("jre");
        FILE *f= fopen("resource_list.json","r");
        if (!f){
            extractResourceList();
        }
        size_t length=IOUtils::getFileContentLength("resource_list.json");
        char* content=(char*) malloc(sizeof(char)*length+1);
        IOUtils::readFile("resource_list.json",content);
        Document document;
        document.Parse(content);
        Value& jre=document["jre"];
        const char *useVersion;
        const char *downloadUrl;
        std::string required_java_version(version);
        if (required_java_version=="8"){
            useVersion="8";
        } else if (required_java_version=="16"){
            useVersion="16";
        } else if (required_java_version=="17"){
            useVersion="17";
        } else{
            LOG_ERROR("%s",Localizer::getInstance()->getString("INSTALL_NOT_SUPPORT_JAVA_VERSION").c_str());
            exit(0);
        }
        if(EnvironmentUtils::isX64()){
            Value& x64=jre[useVersion]["x64"];
            downloadUrl=x64[0].GetString();
        } else {
            Value& x86=jre[useVersion]["x86"];
            downloadUrl=x86[0].GetString();
        }
        char dest[64]={0};
        FileEncoder::GetCacheName(dest);
        IOUtils::createFile(dest);
        RangedDownload serverDownloader(downloadUrl, dest, GlobalVars::getGlobalConfig()->getInt("ThreadSize"));
        serverDownloader.execute([](std::vector<node*> nodes, struct info *i)->void{
            double total=(double)i->total/1048576;
            double downloaded=(double)i->downloaded/1048576;
            LOG_INFO(Localizer::getInstance()->getString("RANGED_DOWNLOAD_INFO").c_str(),i->progress*100,i->speed,total,downloaded);
        });
        LOG_INFO("%s",Localizer::getInstance()->getString("INSTALL_JAVA_UNCOMPRESS").c_str());
        std::string outPath="jre/";
        char rootDirName[255]={0};
        FileEncoder::ZipDecompress(dest,outPath.c_str(),rootDirName);
        LOG_INFO("%s",Localizer::getInstance()->getString("INSTALL_JAVA_DONE_WINDOWS").c_str());
        char cwd[255]={0};
        getcwd(cwd,255);
        strcat(cwd,"\\jre\\");
        strcat(cwd,rootDirName);
        strcat(cwd,"bin\\java.exe");
        std::string command(cwd);
        replace(command.begin(),command.end(),'/','\\');
        LOG_DEBUG("%s",cwd);
        command.append(" -jar java_details.jar");
        system(command.c_str());
        FILE *f1 = fopen("java_details.json", "r");
        size_t length1 = IOUtils::getFileContentLength("java_details.json");
        char *content1 = (char *) malloc(sizeof(char) * length1 + 1);
        IOUtils::readFile("java_details.json", content1);
        Document java_details1;
        java_details1.Parse(content1);
        const char* jrePath1=java_details1["JrePath"].GetString();
        availableJavaPaths.emplace_back(jrePath1);
        GlobalVars::getGlobalConfig()->saveKey("JDK"+required_java_version,jrePath1);
    }
#endif
#ifdef LINUX
    LOG_FATAL("%s",Localizer::getInstance()->getString("INSTALL_JAVA_NOT_FOUND_LINUX").c_str());
    exit(0);
#endif
    GlobalVars::setAvailableJavaPaths(availableJavaPaths);
    return 0;
}