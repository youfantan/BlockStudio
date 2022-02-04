#include "RangedDownload.h"
#include "Utils.h"

extern char _binary_cacert_pem_start[];
extern char _binary_cacert_pem_end[];

int ExtractCA() {
    char *p = _binary_cacert_pem_start;
    FILE *f = fopen("cacert.pem", "wb+");
    while (p != _binary_cacert_pem_end) {
        fwrite(p, 1, 1, f);
        p++;
    }
    fclose(f);
    return 0;
}
size_t receive_data(void *buffer, size_t size, size_t nmemb,void *udata){
    node *node=(struct node*)udata;
    RangedDownload *instance=(RangedDownload*)node->INSTANCE;
    std::lock_guard<std::mutex> guard(instance->lock);
    size_t written = 0;
    if (node->start + size * nmemb <= node->end)
    {
        fseek (node->file, node->start, SEEK_SET);
        written = fwrite (buffer, size, nmemb, node->file);
        node->start += size * nmemb;
    }
    else
    {
        fseek (node->file, node->start, SEEK_SET);
        written = fwrite (buffer, 1, node->end-node->start+1, node->file);
        node->start = node->end;
    }
    return written;
}

void* workThread(void* arg){
    node* nArg=(node*)arg;
    char range[64] = { 0 };
    snprintf (range, sizeof (range), "%ld-%ld", nArg->start, nArg->end);
#ifdef WINDOWS
    LOG_DEBUG("Thread ID: %lu Range: %s",GetCurrentThreadId(),range);
#endif
#ifdef LINUX
    LOG_DEBUG("Thread ID: %lu Range: %s\n",gettid(),range);
#endif
    CURL *curl=curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, nArg->url);
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, receive_data);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) nArg);
    curl_easy_setopt (curl, CURLOPT_RANGE, range);
    curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.0 Safari/537.36");
    int res= curl_easy_perform(curl);
    if (res!=CURLE_OK){
        LOG_ERROR("CURL error.Code: %d",res);
    }
    curl_easy_cleanup(curl);
#ifdef WINDOWS
    LOG_DEBUG("Thread ID: %lu Finished Task",GetCurrentThreadId());
#endif
#ifdef LINUX
    LOG_DEBUG("Thread ID: %lu Finished Task",gettid())
#endif
    auto *instance=(RangedDownload*)nArg->INSTANCE;
    instance->tCount--;
    return 0;
}

RangedDownload::RangedDownload(const char *url, const char *savePath, int threadSize) {
    LOG_DEBUG("Download URL:%s",url);
    RangedDownload::url=url;
    RangedDownload::savePath=savePath;
    RangedDownload::threadSize=threadSize;
    tCount=0;
    FILE *file= fopen(savePath,"wb+");
    global=file;
    length= GetContentLength();
    long range=length/threadSize;
    for (int i = 0; i < threadSize; ++i) {
        long start=i*range;
        long end=(i+1)*range-1;
        if (i==threadSize-1){
            end=length;
        }
        node* arg=new node();
        arg->start=start;
        arg->end=end;
        arg->url=url;
        arg->file=file;
        arg->id=tCount;
        arg->INSTANCE=(void*)this;
        tCount++;
        nodes.push_back(arg);
    }
}

void RangedDownload::execute(callback c) {
    for(auto &i:nodes){
        pthread_create(&i->tid,nullptr, workThread,i);
    }
    long latest=0;
    info *i=(info*) malloc(sizeof(info));
    while (true){
        if (tCount==0){
            break;
        }else{
            long total=0;
            for (auto & node : nodes) {
                total+=node->end-node->start;
            }
            long downloaded=length-total;
            double progress=(double)downloaded/length;
            double speed=(double)((downloaded-latest)*10)/1048576;
            latest=downloaded;
            i->progress=progress;
            i->speed=speed;
            i->total=length;
            i->downloaded=latest;
            c(nodes,i);
            Sleep(100);
        }
    }
    free(i);
    fclose(global);
    for (int i = 0; i < nodes.size(); ++i) {
        fclose(nodes[i]->file);
        delete nodes[i];
    }
    curl_global_cleanup();
}

void* async(void *arg){
    async_arg *asyncArg=(async_arg*)arg;
    asyncArg->instance.execute(asyncArg->c);
    return nullptr;
}

void RangedDownload::executeAsync(callback c) {
    pthread_t tid;
    pthread_create(&tid, nullptr,async, &c);
}

//avoid curl print the header in the terminal
size_t thrownull(char *a, size_t b, size_t c, void *d){
    return b*c;
}

long RangedDownload::GetContentLength() {
    double length=0;
    CURL* curl;
    curl=curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_HEADER, 1);
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, thrownull);
    curl_easy_setopt (curl, CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.0 Safari/537.36");
    CURLcode res = curl_easy_perform(curl);
    if (res!=CURLE_OK){
        return -1;
    }
    curl_easy_getinfo(curl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&length);
    curl_easy_cleanup(curl);
    return length;
}

void RangedDownload::INIT() {
    FILE *f;
    f = fopen("cacert.pem", "r");
    if (!f){
        ExtractCA();
    }
    fclose(f);
}
