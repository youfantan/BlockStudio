#ifndef CONFIG_H
#define CONFIG_H
#include <fstream>
#include <cstring>
#include <map>
#include "Utils.h"
class Config {
public:
    Config(const char* name);
    ~Config();
    void getKey(char* buffer);
    void saveKey(std::string k,std::string v);
    int getInt(std::string k);
    double getDouble(std::string k);
    const char* getCharPtr(std::string k);
    std::string getString(std::string k);
    void setValue(std::string k,std::string v);
private:
    FILE *f;
    std::map<std::string,std::string> kv;
};
#endif