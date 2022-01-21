#include "Localizer.h"
using namespace std;
Localizer *Localizer::Instance;
Localizer::Localizer():map() {

}

Localizer::~Localizer() {
    map.clear();
}

Localizer *Localizer::getInstance() {
    if (Instance== nullptr){
        Instance=new Localizer();
    }
    return Instance;
}

std::string Localizer::getString(std::string k) {
    if (map.find(k)==map.end()){
        return k;
    }
    return map[k];
}


void Localizer::loadLanguageFile(std::string filePath) {
    FILE *f= fopen(filePath.c_str(),"r");
    char c[256]={0};
    while (!feof(f)){
        fgets(c,256,f);
        string s(c);
        string left=s.substr(0,s.find('='));
        string right=s.substr(s.find('=')+1,s.length());
        //remove line breaks
        //我不知道为什么要+1，但删掉它有概率不会正常运行（但可以正常在GDB里运行）
        //这种东西不应该叫c++，我觉得它应该被写进《周易》
        //可能姬昌看到这个bug也会吐血
        //BUG复现环境：Windows 10 x64,MinGW64 6.0,Debug x64
        //错误码0xC0000374，错误发生在第44行
        char* c=new char[right.length()+1];
        strcpy(c,right.c_str());
        c[right.length()-1]='\0';
        string r1(c);
        map[left]=r1;
    }
}

void Localizer::setValue(std::string key,std::string value) {
    map[key]=value;
}
