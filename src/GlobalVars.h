#include "Config.h"
class GlobalVars {
public:
    static void setGlobalConfig(Config* cfg);
    static Config * getGlobalConfig();
    static std::vector<std::string> getAvailableGitPaths();
    static std::vector<std::string> getAvailableJavaPaths();
    static void setAvailableGitPaths(std::vector<std::string> vct);
    static void setAvailableJavaPaths(std::vector<std::string> vct);
private:
    static Config *GlobalConfig;
    static Config *ServersConfig;
    static std::vector<std::string> availableGitPaths;
    static std::vector<std::string> availableJavaPaths;
};
