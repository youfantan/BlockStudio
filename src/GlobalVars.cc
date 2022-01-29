#include "GlobalVars.h"

void GlobalVars::setGlobalConfig(Config *cfg) {
    GlobalConfig=cfg;
}

Config *GlobalVars::getGlobalConfig() {
    return GlobalConfig;
}

std::vector<std::string> GlobalVars::getAvailableGitPaths() {
    return availableGitPaths;
}

std::vector<std::string> GlobalVars::getAvailableJavaPaths() {
    return availableJavaPaths;
}

void GlobalVars::setAvailableGitPaths(std::vector<std::string> vct) {
    availableGitPaths=vct;
}

void GlobalVars::setAvailableJavaPaths(std::vector<std::string> vct) {
    availableJavaPaths=vct;
}

Config *GlobalVars::GlobalConfig;
std::vector<std::string> GlobalVars::availableGitPaths;
std::vector<std::string> GlobalVars::availableJavaPaths;