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
#include <sstream>
#include <cstring>
#include <iostream>

using namespace rapidjson;
/*
Return value of Core.cc functions
 0:Normal exit
 -1:Abnormal exit
 1:Return to last step
*/

extern char _binary_THIRD_PARTY_LICENCE_start[];
extern char _binary_THIRD_PARTY_LICENCE_end[];
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

int Core::deployServer() {
    Options option;
    option.setTitle(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_FIRST"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_SECOND"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_THIRD"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_FOURTH"));
    option.addOption(Localizer::getInstance()->getString("CORE_DEPLOY_SERVER_FIFTH"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    option.getRetVal(ret);
    switch (ret) {
        case 0:
        {
            return DeployServer::getInstance()->deployVanilla();
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
            return 0;
        }
        case 4:
        {
            return 0;
        }
        case 5:
        {
            return 1;
        }
        default:
        {
            deployServer();
            return 0;
        }
    }
}

int Core::launchServer() {
return 0;
}

int Core::utils() {
    Options option;
    option.setTitle(Localizer::getInstance()->getString("CORE_UTILS"));
    option.addOption(Localizer::getInstance()->getString("CORE_UTILS_FIRST"));
    option.addOption(Localizer::getInstance()->getString("CORE_UTILS_SECOND"));
    option.addOption(Localizer::getInstance()->getString("CORE_UTILS_THIRD"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    option.getRetVal(ret);
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
            return 1;
        }
        default:
        {
            utils();
            return 0;
        }
    }
}
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
int Core::about() {
    char licence[40000]={0};
    FILE *f= fopen("THIRD_PARTY_LICENCE","r");
    if (!f){
        extractLicence();
        fclose(f);
    }
    IOUtils::readFile("THIRD_PARTY_LICENCE",licence);
    printf("%s\n%s\n%s\n",Localizer::getInstance()->getString("ABOUT").c_str(),Localizer::getInstance()->getString("THIRD_PARTY_LICENCE").c_str(),licence);
    return 0;
}
Core *Core::INSTANCE;
Core *Core::getInstance() {
    if (INSTANCE== nullptr){
        INSTANCE=new Core();
    }
    return INSTANCE;
}
int DeployServer::deployVanilla() {
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
    printf("%d", strlen(manifest_content));
    Document manifest_object;
    manifest_object.Parse(manifest_content);
    const Value& latest=manifest_object["latest"];
    std::string latest_release=latest["release"].GetString();
    std::string latest_snapshot=latest["snapshot"].GetString();
    const Value& versions=manifest_object["versions"];
    std::map<int ,std::string> version_map;
    std::map<std::string ,std::string> url_map;
    int count=0;
    for (auto& version:versions.GetArray()){
        char option_value[64]={0};
        std::string type=version["type"].GetString();
        if (type!="snapshot"){
            sprintf(option_value,"%s (%s) (%s)",version["id"].GetString(),version["type"].GetString(),version["releaseTime"].GetString());
            option.addOption(option_value);
            version_map[count]=version["id"].GetString();
            url_map[version["id"].GetString()]=version["url"].GetString();
            count++;
        }
    }
    option.addOption(Localizer::getInstance()->getString("DEPLOY_SERVER_VANILLA_LATEST_RELEASE"));
    option.addOption(Localizer::getInstance()->getString("DEPLOY_SERVER_VANILLA_LATEST_SNAPSHOT"));
    option.addOption(Localizer::getInstance()->getString("MENU_LAST"));
    int ret;
    int n=option.getRetVal(ret);
    if (n==-1){
        deployVanilla();
        return 0;
    }
    const char* chooseVersion;
    if (ret==versions.Size()+2)
        return 1;
    else if (ret==versions.Size()+1)
        chooseVersion=latest_snapshot.c_str();
    else if (ret==versions.Size())
        chooseVersion=latest_release.c_str();
    else if (ret>versions.Size()+2)
        return false;
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
    Value& downloads=version_object["downloads"];
    Value& server=downloads["server"];
    std::string downloadUrl=server["url"].GetString();
    const char* sha1=server["sha1"].GetString();
    downloadUrl=downloadUrl.replace(0,27,prefix);
    LOG_INFO("%s",downloadUrl.c_str());
    RangedDownload download(downloadUrl.c_str(), serverJar.c_str(), GlobalVars::getGlobalConfig()->getInt("ThreadSize"));
    download.execute([](std::vector<node*> nodes,struct info *i)->void{
        double total=(double)i->total/1048576;
        double downloaded=(double)i->downloaded/1048576;
        LOG_INFO(Localizer::getInstance()->getString("RANGED_DOWNLOAD_INFO").c_str(),i->progress*100,i->speed,total,downloaded);
    });
    char destSHA1[41]={0};
    FileEncoder::GetFileSHA1(serverJar.c_str(),destSHA1);
    if (strcmp(destSHA1,sha1)!=0){
        LOG_ERROR(Localizer::getInstance()->getString("DEPLOY_SERVER_ERROR_SHA1").c_str(),destSHA1,sha1);
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
    info_writer.Key(sha1);
    info_writer.EndObject();
    const char* info=buffer.GetString();
    IOUtils::writeFile(serverConfig.c_str(),info);
    LOG_INFO("%s",Localizer::getInstance()->getString("DEPLOY_SERVER_SUCCESSFUL").c_str());
    return 0;
}

int DeployServer::deployBukkitOrSpigot() {
    return 0;
}

int DeployServer::deployForge() {
    return 0;
}

int DeployServer::deployFabric() {
    return 0;
}
DeployServer *DeployServer::INSTANCE;
DeployServer *DeployServer::getInstance() {
    if (INSTANCE== nullptr){
        INSTANCE=new DeployServer();
    }
    return INSTANCE;
}

DeployServer::DeployServer() {

}

DeployServer::~DeployServer() {

}
