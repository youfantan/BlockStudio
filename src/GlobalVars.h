#include "Config.h"
class GlobalVars {
public:
    static void setGlobalConfig(Config* cfg);
    static Config * getGlobalConfig();

private:
    static Config *GlobalConfig;
    static Config *ServersConfig;
};
