#include "Config.h"
#include "Utils.h"
#include "ApplicationEventBus.h"
#include <string>
#include <algorithm>
#include <zlib.h>
#include <curl/curl.h>
#include <iomanip>
#include <bzlib.h>
#include <iostream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <compile_config.h>
#include <mutex>
#include <microtar.h>
#ifdef WINDOWS
#include <windows.h>
#include <io.h>
#include <sys/stat.h>
#include <sstream>
#include <fcntl.h>
#include <list>

#endif

#ifdef LINUX
#include <signal.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/stat.h>
#endif

using namespace std;

const char* getFileName(const char *path) {
    std::string s=path;
    std::string::size_type p = s.find_last_of('\\') + 1;
    std::string s1 = s.substr(p, s.length() - p);
    char* c=new char[32];
    strcpy(c,s1.c_str());
    return c;
}

const char* Log::getTime(const char* format){

    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);
    char *strTime=new char[32];
    sprintf(strTime, format, time_tm->tm_year + 1900,
            time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
            time_tm->tm_min, time_tm->tm_sec);
    return strTime;
}

const char* LOG(const char* LEVEL,const char* c,int i,const char* fmt, va_list *lst){
    char *msg=new char[1024];
    vsprintf(msg,fmt,*lst);
    char* formattedMsg=new char[1024];
    sprintf(formattedMsg,"[%s][%s][%s:%d]%s\n",Log::getTime("%d-%02d-%02d %02d:%02d:%02d"),LEVEL,getFileName(c),i,msg);
    return formattedMsg;
}

void Log::UDEBUG(const char *c, int i, const char *fmt, ...) {
#ifdef ENABLE_DEBUG
    std::lock_guard<std::mutex> guard(lock);
    va_list lst;
    va_start(lst,fmt);
    const char* msg= LOG("DEBUG",c,i,fmt,&lst);
    printf(msg);
    writeLog(msg);
    delete msg;
    va_end(lst);
#endif
}

void Log::UINFO(const char *c, int i, const char *fmt, ...) {
    std::lock_guard<std::mutex> guard(lock);
    va_list lst;
    va_start(lst,fmt);
    const char* msg= LOG("INFO",c,i,fmt,&lst);
    printf(msg);
    writeLog(msg);
    delete msg;
    va_end(lst);
}

void Log::UTRACE(const char *c, int i, const char *fmt, ...) {
    std::lock_guard<std::mutex> guard(lock);
    va_list lst;
    va_start(lst,fmt);
    const char* msg= LOG("TRACE",c,i,fmt,&lst);
    printf(msg);
    writeLog(msg);
    delete msg;
    va_end(lst);
}

void Log::UERROR(const char *c, int i, const char *fmt, ...) {
    std::lock_guard<std::mutex> guard(lock);
    va_list lst;
    va_start(lst,fmt);
    const char* msg= LOG("ERROR",c,i,fmt,&lst);
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
#endif
    printf(msg);
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
    writeLog(msg);
    delete msg;
    va_end(lst);
}

void Log::UFATAL(const char *c, int i, const char *fmt, ...) {
    std::lock_guard<std::mutex> guard(lock);
    va_list lst;
    va_start(lst,fmt);
    const char* msg= LOG("FATAL",c,i,fmt,&lst);
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
#endif
    printf(msg);
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
    writeLog(msg);
    delete msg;
    va_end(lst);
}

void doBeforeClose(){
    Log::doBeforeClose();
}

void Log::writeLog(const char *c) {
    fwrite(c,strlen(c),1,logFile);
}
FILE* Log::logFile;

void Log::doBeforeClose() {
#ifdef ENABLE_LOG
    //get file size
    fseek(logFile,0,SEEK_END);
    long size= ftell(logFile);
    rewind(logFile);
    char* buffer=new char[size];
    fread(buffer, sizeof(char),size,logFile);
    uLongf rLen=size;
    char* result=new char[10240];
    int rSize= sizeof(result);
    compress(reinterpret_cast<Bytef *>(result), &rLen, reinterpret_cast<const Bytef*>(buffer), size);
    const char* fn= getTime("logs/%d_%02d_%02d_%02d.%02d.%02d.log.compressed");
    FILE *compressedLog= fopen(fn,"wb+");
    char fnLen[16]={0};
    sprintf(fnLen,"%ld\n",size);
    LOG_DEBUG("%s","Ready to close.Saving logs to archive file");
    LOG_DEBUG("LOG FILE ORIGIN LENGTH:%ld bytes",size);
    LOG_DEBUG("LOG FILE COMPRESSED LENGTH:%ld bytes",rLen);
    fwrite(fnLen, strlen(fnLen),1,compressedLog);
    fwrite(result, rLen, 1, compressedLog);
    fclose(compressedLog);
    free(buffer);
    free(result);
#endif
    fclose(logFile);
}

void Log::INIT() {
#ifdef ENABLE_LOG
    struct stat info{};
    int res=stat("logs",&info);
    if (res!=0){
        IOUtils::createDir("logs");
    } else if (!info.st_mode&S_IFDIR){
        remove("logs");
        IOUtils::createDir("logs");
    }
#endif
    logFile= fopen("logs/latest.log","wb+");
    ApplicationEventBus::registerBeforeClose(doBeforeClose);
}

const char *Log::ViewLog(const char *logFN) {
    const char* dest;
    FILE *compressedFile= fopen(logFN,"rb+");
    char cLen[16]={0};
    fgets(cLen,16,compressedFile);
    long originSize=atol(cLen);
    dest=new char[originSize];
    fseek(compressedFile,0,SEEK_END);
    unsigned long srcSize= ftell(compressedFile)- strlen(cLen);
    char* src=new char[srcSize];
    fseek(compressedFile, strlen(cLen),SEEK_SET);
    fgets(src,srcSize+1,compressedFile);
    uncompress((Bytef *) dest, reinterpret_cast<uLongf *>(&originSize), reinterpret_cast<const Bytef *>(src), srcSize);
    delete[] src;
    return dest;
}

void Log::UWARNING(const char *c, int i, const char *fmt, ...) {
    std::lock_guard<std::mutex> guard(lock);
    va_list lst;
    va_start(lst,fmt);
    const char* msg= LOG("WARNING",c,i,fmt,&lst);
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN);
#endif
    printf(msg);
#ifdef WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
    writeLog(msg);
    delete msg;
    va_end(lst);
}

std::mutex Log::lock;

int FileEncoder::GZipDecompress(char *data, long nData, char *dest, long &nDest) {
    memset(dest,0,nDest);
    int err = 0;
    z_stream d_stream = { 0 };
    static char dummy_head[2] = {
            0x8 + 0x7 * 0x10,
            (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in = reinterpret_cast<unsigned char*>(data);
    d_stream.avail_in = nData;
    d_stream.avail_out = nDest;
    d_stream.next_out = reinterpret_cast<unsigned char*>(dest);
    int ret=-1;
    ret=inflateInit2(&d_stream, MAX_WBITS + 16);
    if (ret==Z_OK){
        err= inflate(&d_stream,Z_FINISH);
        if (err==Z_STREAM_END){
            nDest=d_stream.total_out;
        }
    }
    inflateEnd(&d_stream);
    return err;
}

int FileEncoder::GZipCompress(char *data, long nData, char *dest, long &nDest) {
    memset(dest,0,nDest);
    z_stream c_stream;
    int err = 0;

    if (data && nData > 0) {
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                         MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
        c_stream.next_in = reinterpret_cast<unsigned char*>(data);
        c_stream.avail_in = nData;
        c_stream.next_out = reinterpret_cast<unsigned char*>(dest);
        c_stream.avail_out = nDest;
        while (c_stream.avail_in != 0 && c_stream.total_out < nDest) {
            if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
        }
        if (c_stream.avail_in != 0) return c_stream.avail_in;
        while (true) {
            if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END){
                break;
            }
            if (err != Z_OK) return err;
        }
        if ((err=deflateEnd(&c_stream)) != Z_OK) return err;
        nDest = c_stream.total_out;
        return 0;
    }
    return -1;
}
const unsigned char FileEncoder::B64EncodeMap[64]=
        {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                '4', '5', '6', '7', '8', '9', '+', '/'
        };
const char FileEncoder::B64DecodeMap[256]=
        {
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
                -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
                -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
                -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
        };
void FileEncoder::Base64Encode(char *data, long nData, char* dest, long &nDest) {
    int num = 0,bin = 0,i;
    std::string _encode_result;
    const unsigned char * current;
    current = reinterpret_cast<unsigned char *>(data);
    while(nData > 2) {
        _encode_result += B64EncodeMap[current[0] >> 2];
        _encode_result += B64EncodeMap[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        _encode_result += B64EncodeMap[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        _encode_result += B64EncodeMap[current[2] & 0x3f];

        current += 3;
        nData -= 3;
    }
    if(nData > 0)
    {
        _encode_result += B64EncodeMap[current[0] >> 2];
        if(nData%3 == 1) {
            _encode_result += B64EncodeMap[(current[0] & 0x03) << 4];
            _encode_result += "==";
        } else if(nData%3 == 2) {
            _encode_result += B64EncodeMap[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            _encode_result += B64EncodeMap[(current[1] & 0x0f) << 2];
            _encode_result += "=";
        }
    }
    memcpy(dest,_encode_result.c_str(),_encode_result.size());
    nDest=_encode_result.size();
}

void FileEncoder::Base64Decode(char *data, long nData, char* dest, long &nDest) {
    int bin = 0,i=0,pos=0;
    std::string _decode_result;
    const char *current = data  ;
    char ch;
    while( (ch = *current++) != '\0' && nData-- > 0 )
    {
        if (ch == '=') {
            if (*current != '=' && (i % 4) == 1) {
                return;
            }
            continue;
        }
        ch = B64DecodeMap[ch];
        if (ch < 0 ) {
            continue;
        }
        switch(i % 4)
        {
            case 0:
                bin = ch << 2;
                break;
            case 1:
                bin |= ch >> 4;
                _decode_result += bin;
                bin = ( ch & 0x0f ) << 4;
                break;
            case 2:
                bin |= ch >> 2;
                _decode_result += bin;
                bin = ( ch & 0x03 ) << 6;
                break;
            case 3:
                bin |= ch;
                _decode_result += bin;
                break;
        }
        i++;
    }
    memcpy(dest,_decode_result.c_str(),_decode_result.size());
    nDest=_decode_result.size();
}

std::string FileEncoder::toHex(std::string src) {
    stringstream ss;
    for (int i = 0; i < src.size(); ++i) {
        ss<<hex<<(int)src[i];
    }
    return ss.str();
}

std::string FileEncoder::toHex(const char *src,long size) {
    stringstream ss;
    for (int i = 0; i < size; ++i) {
        ss<<hex<<(int)src[i];
    }
    return ss.str();
}

std::string FileEncoder::toHex(const unsigned char *src,long size) {
    std::ostringstream strHex;
    strHex << std::hex << std::setfill('0');
    for (size_t i = 0; i < size; ++i) {
        strHex << std::setw(2)<<static_cast<unsigned int>(src[i]);
    }
    return strHex.str();
}

void FileEncoder::MD5(char *data, long nData, char *dest) {
    unsigned char result[16]={0};
    ::MD5(reinterpret_cast<const unsigned char*>(data), nData,result);
    string hex= toHex(reinterpret_cast<const unsigned char*>(result),16);
    memcpy(dest,hex.c_str(),hex.size());
    dest[32]='\0';
}

void FileEncoder::GetFileMD5(const char *path, char *dest) {
    ifstream fs(path,ios::in|ios::binary);
    char buffer[1024];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    int remain;
    while(!fs.eof()){
        fs.read(buffer,1024);
        remain=fs.gcount();
        if (remain){
            MD5_Update(&ctx,buffer,remain);
        }
    }
    unsigned char result[16];
    MD5_Final(result,&ctx);
    string hex= toHex(reinterpret_cast<const unsigned char*>(result),16);
    memcpy(dest,hex.c_str(),hex.size());
    dest[32]='\0';
    fs.close();
}

void FileEncoder::SHA1(char *data, long nData, char *dest) {
    unsigned char result[20]={0};
    ::SHA1(reinterpret_cast<unsigned char*>(data),nData,result);
    string hex= toHex(reinterpret_cast<const unsigned char*>(result),20);
    memcpy(dest,hex.c_str(),hex.size());
    dest[40]='\0';
}

void FileEncoder::GetFileSHA1(const char* path,char* dest) {
    unsigned char result[20]={0};
    ifstream fs(path,ios::in|ios::binary);
    char buffer[1024];
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    int remain;
    while(!fs.eof()){
        fs.read(buffer,1024);
        remain=fs.gcount();
        if (remain){
            SHA1_Update(&ctx,buffer,remain);
        }
    }
    SHA1_Final(result,&ctx);
    string hex= toHex(reinterpret_cast<const unsigned char*>(result),20);
    memcpy(dest,hex.c_str(),hex.size());
    dest[40]='\0';
    fs.close();
}

void FileEncoder::SHA256(char *data, long nData, char *dest) {
    unsigned char result[32]={0};
    ::SHA256(reinterpret_cast<unsigned char*>(data),nData,result);
    string hex= toHex(reinterpret_cast<const unsigned char*>(result),32);
    memcpy(dest,hex.c_str(),hex.size());
    dest[64]='\0';
}

void FileEncoder::GetFileSHA256(const char *path, char *dest) {
    unsigned char result[32]={0};
    ifstream fs(path,ios::in|ios::binary);
    char buffer[1024];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    int remain;
    while(!fs.eof()){
        fs.read(buffer,1024);
        remain=fs.gcount();
        if (remain){
            SHA256_Update(&ctx,buffer,remain);
        }
    }
    SHA256_Final(result,&ctx);
    string hex= toHex(reinterpret_cast<const unsigned char*>(result),32);
    memcpy(dest,hex.c_str(),hex.size());
    dest[64]='\0';
    fs.close();
}
int IOUtils::traverseDirectory(std::string path, std::vector<std::string> &vct, int &depth,bool b,bool useRelativePath) {
    long hFile = 0;
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    depth--;
                    if (b){
                        std::string absolutePath=p.assign(path).append("\\").append(fileinfo.name);
                        if (useRelativePath){
                            char path[255]={0};
                            _getcwd(path,255);
                            vct.push_back(absolutePath.replace(0, strlen(path)+1,""));
                        } else{
                            vct.push_back(absolutePath);
                        }
                    } else{
                        traverseDirectory(p.assign(path).append("\\").append(fileinfo.name), vct,depth,b,useRelativePath);
                    }
                }
            }
            else
            {
                std::string absolutePath=p.assign(path).append("\\").append(fileinfo.name);
                if (useRelativePath){
                    char path[255]={0};
                    _getcwd(path,255);
                    vct.push_back(absolutePath.replace(0, strlen(path)+1,""));
                } else{
                    vct.push_back(absolutePath);
                }
            }
        }while (_findnext(hFile, &fileinfo) == 0&&(depth>0||depth<0));
        _findclose(hFile);
        return 0;
    }
    return -1;
}

void IOUtils::writeFile(const char* fileName,const char* content,long length){
    FILE *f= fopen(fileName,"wb");
    fwrite(content,length,1,f);
    fflush(f);
    fclose(f);
}
void IOUtils::writeFile(const char* fileName,const char* content){
    writeFile(fileName,content, strlen(content));
}
void IOUtils::writeFile(FILE *f,const char* content,long length){
    fwrite(content,length,1,f);
}
void IOUtils::writeFile(std::ofstream& stream,const char* content,long length){
    stream.write(content,length);
}
void IOUtils::appendFile(const char* fileName,const char* content,long length){
    FILE* f= fopen(fileName,"ab+");
    fseek(f,0,SEEK_END);
    long len=ftell(f);
    fwrite(content,length,1,f);
    fflush(f);
    fclose(f);
}
void IOUtils::appendFile(const char* fileName,const char* content){
    appendFile(fileName,content, strlen(content));
}
void IOUtils::readFile(const char* fileName,char* content,long length){
    ifstream in(fileName,std::ios::in|std::ios::binary);
    in.read(content,length);
    content[length]='\0';
    in.close();
}
//NOT SAFE
void IOUtils::readFile(const char* fileName,char* content){
    FILE* f= fopen(fileName,"rb");
    fseek(f,0,SEEK_END);
    long length= ftell(f);
    memset(content,0,length);
    fseek(f,0,SEEK_SET);
    fread(content,length,1,f);
    content[length]='\0';
    fclose(f);
}
void IOUtils::readFile(FILE *f,char* content,long length){
    memset(content,0,length);
    content[length]='\0';
    fread(content,length,1,f);
}
void IOUtils::readFile(std::ifstream& stream,char* content,long length){
    memset(content,0,length);
    content[length]='\0';
    stream.read(content,length);
}
int IOUtils::createFile(const char* fileName){
    ofstream s(fileName);
    if (s){
        s.close();
        return 0;
    } else{
        s.close();
        return -1;
    }
}
int IOUtils::createDir(const char* dirName){

    int i = 0;
    int iRet;
    size_t iLen;
    char* pszDir;
    if(dirName==nullptr)
    {
        return -1;
    }
    pszDir = strdup(dirName);
    iLen = strlen(pszDir);
    for (i = 0;i < iLen;i ++)
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/')
        {
            pszDir[i] = '\0';
            iRet =access(pszDir,0);
            if (iRet != 0)
            {
                iRet = mkdir(pszDir);
                if (iRet != 0)
                {
                    return -1;
                }
            }
            pszDir[i] = '/';
        }
    }

    iRet = mkdir(pszDir);
    free(pszDir);
    return iRet;
}

void IOUtils::createDirIfNotExists(const char *dirName) {
    struct stat info{};
    int res=stat(dirName,&info);
    if (res!=0){
        IOUtils::createDir(dirName);
    } else if (!S_ISDIR(info.st_mode)){
        remove(dirName);
        IOUtils::createDir(dirName);
    }
}
void IOUtils::createFileIfNotExists(const char* fileName) {
    struct stat info{};
    int res=stat(fileName,&info);
    if (res!=0){
        IOUtils::createFile(fileName);
    } else if (!S_ISREG(info.st_mode)) {
        remove(fileName);
        IOUtils::createFile(fileName);
    }
}
size_t write_file_function(void *ptr, size_t size, size_t nmemb, void *data){
    fwrite(ptr,size,nmemb,(FILE *)data);
    return size*nmemb;
}
struct memory{
    char *ptr;
    size_t len;
};
size_t write_memory_function(void *ptr, size_t size, size_t nmemb, void *udata){
    memory *data=(memory*)udata;
    size_t newLength=data->len+size*nmemb;
    data->ptr= (char*)realloc(data->ptr,newLength+1);
    memcpy(data->ptr+data->len,ptr,size*nmemb);
    data->ptr[newLength]='\0';
    data->len=newLength;
    return size*nmemb;
}
void IOUtils::downloadFile(const char* url,const char* savePath) {
    CURL *curl=curl_easy_init();
    FILE *f= fopen(savePath,"wb+");
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,f);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, write_file_function);
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.0 Safari/537.36");
    int res= curl_easy_perform(curl);
    if (res!=CURLE_OK){
        LOG_ERROR("CURL error.Code: %d",res);
    }
}
//avoid curl print the header in the terminal
size_t throwHeaderOutput(char *a, size_t b, size_t c, void *d){
    return b*c;
}
long IOUtils::getContentLength(const char* url){
    double length=0;
    CURL* curl;
    curl=curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_HEADER, 1);
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, throwHeaderOutput);
    curl_easy_setopt (curl, CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.0 Safari/537.36");
    CURLcode res = curl_easy_perform(curl);
    if (res!=CURLE_OK){
        LOG_ERROR("CURL error.Code: %d",res);
        return -1;
    }
    curl_easy_getinfo(curl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&length);
    curl_easy_cleanup(curl);
    return length;
}
void IOUtils::download(const char* url,char* dest) {
    char* buffer=(char*) malloc(sizeof(char)*1);
    memory *m=new memory;
    m->ptr=buffer;
    m->len=0;
    CURL *curl=curl_easy_init();
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, write_memory_function);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,m);
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.0 Safari/537.36");
    int res= curl_easy_perform(curl);
    if (res!=CURLE_OK){
        LOG_ERROR("CURL error.Code: %d",res);
        return;
    }
    memcpy(dest,m->ptr,m->len+1);
    free(m);
}

void IOUtils::readFile(const char *fileName, std::string dest) {
    ifstream f(fileName,ios::in|ios::binary);
    f>>dest;
}

int IOUtils::dirExists(const char* dirName) {
    struct stat info{};
    int res=stat(dirName,&info);
    if (res!=0){
        return 0;
    } else if (!S_ISDIR(info.st_mode)){
        return -2;
    } else{
        return -1;
    }
}

int IOUtils::fileExists(const char* fileName) {
    struct stat info{};
    int res=stat(fileName,&info);
    if (res!=0){
        return 0;
    } else if (!S_ISREG(info.st_mode)){
        return -2;
    } else{
        return -1;
    }
}

size_t IOUtils::getFileContentLength(const char *fileName) {
    FILE *f= fopen(fileName,"rb");
    fseek(f,0,SEEK_END);
    size_t size= ftell(f);
    fclose(f);
    return size;
}

int EnvironmentUtils::whereGitExists(std::vector<std::string> &path) {
#ifdef WINDOWS
    FILE* pp = _popen("where git", "r");
    if (!pp) {
        return -1;
    }
    char tmp[1024];
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0';
        }
        std::string str;
        if (str.find("\\cmd\\git.exe")!=std::string::npos){
            str=str.substr(0,str.find("\\bin\\java.exe"));
            path.push_back(str);

        }
    }
    _pclose(pp);
    return 0;
#endif
}

int EnvironmentUtils::whereJavaExists(std::vector<std::string> &path) {
#ifdef WINDOWS
    FILE* pp = _popen("where java", "r");
    if (!pp) {
        return -1;
    }
    char tmp[1024];
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0';
        }
        path.push_back(tmp);
    }
    _pclose(pp);
    return 0;
#endif
}

bool EnvironmentUtils::isX64() {
    if (sizeof(int*)==8){
        return true;
    }
    return false;
}

int FileEncoder::BZip2Decompress(const char* fileName,const char* extractPath){
    FILE *srcFile= fopen(fileName,"rb");
    if (!srcFile){
        fclose(srcFile);
        return -1;
    }
    int err;
    std::string res;
    BZFILE *bSrcFile= BZ2_bzReadOpen(&err,srcFile,0,0,nullptr,0);
    if (err!=BZ_OK){
        BZ2_bzReadClose(&err,bSrcFile);
        fclose(srcFile);
        return -2;
    }
    char cachePath[64]={0};
    GetCacheName(cachePath);
    IOUtils::createFileIfNotExists(cachePath);
    FILE *cache= fopen(cachePath,"wb");
    err=BZ_OK;
    char buffer[1024]={0};
    while (err==BZ_OK){
        int len=BZ2_bzRead(&err,bSrcFile,buffer,1024);
        if (err==BZ_OK){
            fwrite(buffer, len,1,cache);
        }
    }
    if (err!=BZ_STREAM_END){
        BZ2_bzReadClose(&err,bSrcFile);
        fclose(srcFile);
        return -3;
    }
    BZ2_bzReadClose(&err,bSrcFile);
    fclose(srcFile);
    fclose(cache);
    mtar_t tar;
    mtar_open(&tar,cachePath,"rb");
    mtar_header_t h;
    while((mtar_read_header(&tar,&h))!=MTAR_ENULLRECORD){
        char path[255]={0};
        strcat(path,extractPath);
        strcat(path,h.name);
        if (h.type==MTAR_TREG){
            char* content=(char*) malloc(sizeof(char)*(h.size+1));
            mtar_read_data(&tar,content,h.size);
            std::string s(path);
            std::string con_path=s.substr(0,s.find_last_of('/'));
            IOUtils::createDirIfNotExists(con_path.c_str());
            IOUtils::writeFile(path,content,h.size);
            free(content);
        }
        mtar_next(&tar);
    }
    mtar_close(&tar);
    return 0;
}

int FileEncoder::TarEncode(std::vector<std::string> compressFiles,const char* destFilePath){
    char cachePath[64]={0};
    GetCacheName(cachePath);
    mtar_t mtar;
    mtar_open(&mtar, cachePath, "w");
    std::list<std::string> pathList;
    for (auto it=compressFiles.begin();it!=compressFiles.end();++it){
        std::string str=*it;
        replace(str.begin(),str.end(),'\\','/');
        std::string path=str.substr(0,str.find_last_of('/'));
        if(find(pathList.begin(),  pathList.end(),path)==pathList.end()){
            pathList.push_back(path);
            mtar_write_dir_header(&mtar, path.c_str());
        }
        size_t len=IOUtils::getFileContentLength(str.c_str());
        char* content=(char*) malloc(sizeof(char)*len+1);
        IOUtils::readFile(str.c_str(),content);
        mtar_write_file_header(&mtar, str.c_str(), len);
        mtar_write_data(&mtar, content, len);
        free(content);
    }
    mtar_finalize(&mtar);
    mtar_close(&mtar);
    return 0;
}

void FileEncoder::GetCacheName(char *dest) {
    IOUtils::createDirIfNotExists("cache");
    time_t t= time(0);
    char cacheName[64]={0};
    char time[33]={0};
    sprintf(time,"%lld",t);
    MD5(time,11,cacheName);
    char cachePath[64]={0};
    strcat(cachePath,"cache/");
    strcat(cachePath,cacheName);
    strcpy(dest,cachePath);
}
