#include <mutex>
#include <curl/curl.h>
#include <vector>
#include <pthread.h>
#include <ctime>
#ifndef BLOCKSTUDIO_RANGEDDOWNLOAD_H
#define BLOCKSTUDIO_RANGEDDOWNLOAD_H
struct info{
    double speed;
    double progress;
    long total;
    long downloaded;
};
struct node{
    const char* url;
    long start;
    long end;
    FILE *file;
    int id;
    pthread_t tid;
    void* INSTANCE;
};
typedef void(*callback)(std::vector<node*>,info*);
class RangedDownload {
public:
    static void INIT();
    RangedDownload(const char* url,const char* savePath,int threadSize);
    long GetContentLength();
    void execute(callback c);
    void executeAsync(callback c);
    std::vector<node*> nodes;
    FILE* global;
    std::mutex lock;
    int tCount=0;
    long length;
private:
    const char* url;
    const char* savePath;
    int threadSize;
};
struct async_arg{
    callback c;
    RangedDownload instance;
};

#endif //BLOCKSTUDIO_RANGEDDOWNLOAD_H
