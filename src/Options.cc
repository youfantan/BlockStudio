#include "Options.h"
#include "Localizer.h"
#include <cstdio>
#include <iostream>
#include <algorithm>


Options::Options() {
    menu=new option_menu;
}

void Options::setTitle(std::string title) {
    menu->title=title;
}
void Options::addOption(const std::string& option) {
    menu->options.push_back(option);
}

bool BeginWithNum(std::string str)
{
    if (std::isdigit(str[0])){
        return true;
    }
    return false;
}
int Options::getRetVal(int &ret) {
    printf("******** M E N U ********\n%s:\n%s\n",menu->title.c_str(),Localizer::getInstance()->getString("OPTION_TIPS").c_str());
    for (int i = 0; i < menu->options.size(); ++i) {
        printf("%d: %s\n",i,menu->options[i].c_str());
    }
    printf("*************************\n>");
    std::string line;
    std::getline(std::cin,line);
    if (!BeginWithNum(line)){
        printf("%s\n",Localizer::getInstance()->getString("OPTION_NOT_NUMBER").c_str());
        return -1;
    }
    ret=std::stoi(line);
    return 0;
}

LineOptions::LineOptions() {
    menu=new line_option_menu;
}

void LineOptions::setTitle(std::string title) {
    menu->title=title;
}

void LineOptions::setTips(std::string content) {
    menu->tips=content;
}

void LineOptions::getInput(std::string &input, line_option_call c) {
    bool can_exit= false;
    while (!can_exit){
        printf("%s:",menu->title.c_str());
        if (!menu->tips.empty()){
            printf("(%s)",menu->tips.c_str());
        }
        std::string buffer;
        std::getline(std::cin,input);
        can_exit=c(input);
        if (!can_exit&&!menu->error_tips.empty()){
            printf("%s\n",menu->error_tips.c_str());
        } else if (can_exit&&!menu->success_tips.empty()){
            printf("%s\n",menu->success_tips.c_str());
        }
    }
}

void LineOptions::setErrorTips(std::string content) {
    menu->error_tips=content;
}

void LineOptions::setSuccessTips(std::string content) {
    menu->success_tips=content;
}
