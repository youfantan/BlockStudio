#ifndef _TEST_H_
#define _TEST_H_
#include <string>
#include <cstring>
#include "../src/Utils.h"
#ifdef WINDOWS
#include <windows.h>
#endif

void _assert_true(bool cond,std::string cond_str){
    if(cond){
#ifdef WINDOWS
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
        printf("[BOOLEQUAL]PASSED");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
        printf("\n%s\n",cond_str.c_str());
#endif
#ifdef LINUX
        printf("\033[32m[BOOLEQUAL]PASSED\033[0m\n%s\n",cond_str.c_str());
#endif
    } else{
#ifdef WINDOWS
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
        printf("[BOOLEQUAL]FAILED");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
        printf("\n%s\n",cond_str.c_str());
#endif
#ifdef LINUX
        printf("\033[31m[BOOLEQUAL]FAILED\033[0m\n%s\n",cond_str.c_str());
#endif
    }
}
void _assert_hex(const char* c1,const char* c2){
    if (strlen(c1)!= strlen(c2)){
#ifdef WINDOWS
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
        printf("[STREQUAL ]FAILED");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
        printf("\nC1: %s\nC2: %s\n",FileEncoder::toHex(c1, strlen(c1)).c_str(),FileEncoder::toHex(c2, strlen(c2)).c_str());
#endif
#ifdef LINUX
        printf("\033[31m[STREQUAL ]FAILED\033[0m\nC1: %s\nC2: %s\n",FileEncoder::toHex(c1).c_str(),FileEncoder::toHex(c2).c_str());
#endif
        return;
    }
    for (int i = 0; i < strlen(c1); ++i) {
        if (c1[i]!=c2[i]){
#ifdef WINDOWS
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
            printf("[STREQUAL ]FAILED");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
            printf("\nC1: %s\nC2: %s\n",FileEncoder::toHex(c1, strlen(c1)).c_str(),FileEncoder::toHex(c2,strlen(c2)).c_str());
#endif
#ifdef LINUX
            printf("\033[31m[STREQUAL ]FAILED\033[0m\nC1: %s\nC2: %s\n",FileEncoder::toHex(c1).c_str(),FileEncoder::toHex(c2).c_str());
#endif
            return;
        }
    }
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
    printf("[STREQUAL ]PASSED");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    printf("\nC1: %s\nC2: %s\n",FileEncoder::toHex(c1,strlen(c1)).c_str(),FileEncoder::toHex(c2,strlen(c2)).c_str());
#endif
#ifdef LINUX
    printf("\033[32m[STREQUAL ][PASSED]\033[0m\nC1: %s\nC2: %s\n",FileEncoder::toHex(c1).c_str(),FileEncoder::toHex(c2).c_str());
#endif
}
void _assert_num(int c1,int c2){
    if (c1!=c2) {
#ifdef WINDOWS
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
        printf("[NUMEQUAL ]FAILED");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("\nN1: %d\nN2: %d\n", c1, c2);
#endif
#ifdef LINUX
        printf("\033[31m[NUMEQUAL ]FAILED\033[0m\nN1: %d\nN2: %d\n",c1,c2);
#endif
    }
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
    printf("[NUMEQUAL ]PASSED");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    printf("\nN1: %d\nN2: %d\n",c1,c2);
#endif
#ifdef LINUX
    printf("\033[32m[NUMEQUAL ][PASSED]\033[0m\nN1: %d\nN2: %d\n",c1,c2);
#endif
}
void _test_header(const char* header,const char* file){
    std::string s=file;
    std::string::size_type p = s.find_last_of('\\') + 1;
    std::string s1 = s.substr(p, s.length() - p);
    char* c=new char[32];
    strcpy(c,s1.c_str());
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN);
        LOG_INFO("==== T E S T:%s(%s) ====",header,c);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
#ifdef LINUX
    LOG_INFO("==== T E S T :%s(%s) = = = =",header,c)
#endif
}
void _test_end(){
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN);
    LOG_INFO("%s","==== T E S T  E N D ====");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
#ifdef LINUX
    LOG_INFO("%s","==== T E S T  E N D = = = =");
#endif
}
void _assert_string(const char* c1,const char* c2){
    if (strlen(c1)!= strlen(c2)){
#ifdef WINDOWS
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
        printf("[STREQUAL ]FAILED");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
        printf("\nC1: %s\nC2: %s\n",c1,c2);
#endif
#ifdef LINUX
        printf("\033[31m[STREQUAL ]FAILED\033[0m\nC1: %s\nC2: %s\n",c1,c2);
#endif
        return;
    }
    for (int i = 0; i < strlen(c1); ++i) {
        if (c1[i]!=c2[i]){
#ifdef WINDOWS
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
            printf("[STREQUAL ]FAILED");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
            printf("\nC1: %s\nC2: %s\n",c1,c2);
#endif
#ifdef LINUX
            printf("\033[31m[STREQUAL ]FAILED\033[0m\nC1: %s\nC2: %s\n",c1,c2);
#endif
            return;
        }
    }
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
    printf("[STREQUAL ]PASSED");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    printf("\nC1: %s\nC2: %s\n",c1,c2);
#endif
#ifdef LINUX
    printf("\033[32m[STREQUAL ][PASSED]\033[0m\nC1: %s\nC2: %s\n",c1,c2);
#endif
}
#define ASSERT_TRUE(_COND_) _assert_true(_COND_, #_COND_)
#define ASSERT_STRING(c1,c2) _assert_string(c1,c2)
#define ASSERT_HEX(c1,c2) _assert_hex(c1,c2)
#define ASSERT_ZERO(c1) _assert_num(c1,0)
#define ASSERT_ONE(c1) _assert_num(c1,1)
#define ASSERT_NEGATIVE_ONE(c1) _assert_num(c1,-1)
#define ASSERT_NUM(c1,c2) _assert_num(c1,c2)
#define HEADER(s) _test_header(s,__FILE__);
#define END(s) _test_end(s);
#endif