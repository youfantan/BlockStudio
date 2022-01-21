#include "GlobalVars.h"

void GlobalVars::setGlobalConfig(Config *cfg) {
    GlobalConfig=cfg;
}

Config *GlobalVars::getGlobalConfig() {
    return GlobalConfig;
}
Config *GlobalVars::GlobalConfig;
