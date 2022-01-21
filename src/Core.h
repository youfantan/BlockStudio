#include <string>
#include <vector>

class Core{
public:
    /* @ABANDONED
     * @params:
     * s1: sources of 1st content
     * s2: sources of 2nd content
     * dest: destination of the LCS result(can be empty)
     * matrix: an int matrix of LCS model(can be empty)
     * signal: signal vector of LCS model(can be empty)
     *
     * @function:
     * LCS Algorithm implement
     * */
    void lcs(std::string s1,std::string s2,std::string &dest,std::vector<std::vector<int>> matrix,std::vector<std::vector<char>> signal);
    /* @ABANDONED
     * @params:
     * s1: filepath of 1st content
     * s2: filepath of 2nd content
     * dest: destination of the LCS result(can be empty)
     *
     * @function:
     * Use LCS to make s1 and s2 to a patch(padding with space)
     */
    void makePatch(std::string s1,std::string s2,std::string &dest);
    /* @ABANDONED
     * @params:
     * p:filepath of the patch
     * o:filepath of original content(param of @func Core::lcs s1)
     * dest:destination of the merged content
     *
     * @function:
     * merge the patch to the second content
     * */
    void mergePatch(std::string p,std::string o,std::string &dest);
    static Core *getInstance();
    int deployServer();
    int launchServer();
    int utils();
    int about();

private:
    static Core *INSTANCE;
};
#define VERSION_MANIFEST_URL "https://launchermeta.mojang.com/mc/game/version_manifest.json"
#define OFFICIAL_META_URL "https://launchermeta.mojang.com"
#define OFFICIAL_URL "https://launcher.mojang.com"
#define MCBBS_URL "https://download.mcbbs.net"
#define BMCLAPI_URL "https://bmclapi2.bangbang93.com"
class DeployServer{
public:
    static DeployServer* getInstance();
    int deployVanilla();
    int deployBukkitOrSpigot();
    int deployForge();
    int deployFabric();

private:
    static DeployServer *INSTANCE;
    DeployServer();
    ~DeployServer();
};