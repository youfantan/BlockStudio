#ifndef BLOCKSTUDIO_OPTIONS_H
#define BLOCKSTUDIO_OPTIONS_H

#include <string>
#include <vector>

typedef bool(*line_option_call)(std::string value);
typedef bool(*option_call)(int value);

struct option_menu{
    std::string title;
    std::vector<std::string> options;
};
class Options {
public:
    Options();
    void setTitle(std::string title);
    void addOption(const std::string& option);
    int getRetVal(int &ret);
private:
    option_menu *menu;
};


struct line_option_menu{
    std::string title;
    std::string tips;
    std::string error_tips;
    std::string success_tips;
};
class LineOptions{
public:
    LineOptions();
    void setTitle(std::string title);
    void setTips(std::string content);
    void setErrorTips(std::string content);
    void setSuccessTips(std::string content);
    void getInput(std::string &input, line_option_call);
private:
    line_option_menu *menu;
};
#endif //BLOCKSTUDIO_OPTIONS_H
