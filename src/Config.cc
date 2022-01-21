#include "Config.h"
void Config::saveKey(std::string k, std::string v) {
    std::string finalStr=k+"="+v;
    fwrite(finalStr.c_str(),finalStr.size(),1,f);
}
void Config::getKey(char *buffer) {
    bool isLeft=true;
    char left[1024];
    char right[1024];
    int left_sub=0;
    int right_sub=0;
    for (int i = 0; i < strlen(buffer); ++i) {
        if (buffer[i]!='='){
            if (isLeft){
                left[i]=buffer[i];
                left_sub++;
            } else{
                right[i- left_sub-1]=buffer[i];
                right_sub++;
            }
        } else{
            isLeft= false;
        }
    }
    left[left_sub]='\0';
    right[right_sub-1]='\0';
    if (right[right_sub-2]=='\r'){
        right[right_sub-2]='\0';
    }
    kv[left]=right;
}

int Config::getInt(std::string k) {
    return std::stoi(kv[k]);
}

double Config::getDouble(std::string k) {
    return std::stod(kv[k]);
}

const char *Config::getCharPtr(std::string k) {
    return kv[k].c_str();
}

std::string Config::getString(std::string k) {
    return kv[k];
}

Config::Config(const char* name) {
    f= fopen(name,"rb+");
    char buffer[1024];
    while (fgets(buffer,1024,f)){
        getKey(buffer);
    }
}

Config::~Config() {
    for (auto it=kv.begin();it!=kv.end();++it){
        saveKey(it->first,it->second);
    }
    fclose(f);
}

void Config::setValue(std::string k, std::string v) {
    kv[k]=v;
}

