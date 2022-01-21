#ifndef BLOCKSTUDIO_LOCALIZER_H
#define BLOCKSTUDIO_LOCALIZER_H

#include <string>
#include <cstring>
#include <map>

class Localizer {
public:
    void setValue(std::string key,std::string value);
    void loadLanguageFile(std::string filePath);
    std::string getString(std::string k);
    static Localizer *getInstance();
private:

    Localizer();
    ~Localizer();
    std::map<std::string, std::string> map;
    static Localizer *Instance;
};


#endif //BLOCKSTUDIO_LOCALIZER_H
