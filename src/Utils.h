#ifndef BLOCKSTUDIO_UTILS_H
#define BLOCKSTUDIO_UTILS_H
#include <cstdio>
#include <string>
#include <mutex>
#include <fstream>
#include <vector>

#define BREAKPOINT Utils::UDEBUG(__FILE__, __LINE__, "%s", "");
#define LOG_DEBUG(fmt, ...) Log::UDEBUG(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) Log::UINFO(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_TRACE(fmt, ...) Log::UTRACE(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) Log::UWARNING(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) Log::UERROR(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_FATAL(fmt, ...) Log::UFATAL(__FILE__, __LINE__, fmt, __VA_ARGS__)

class Log {
public:
    static void INIT();
    static const char* getTime(const char* format);
    static const char* ViewLog(const char* logFN);
    static void UDEBUG(const char *c, int i, const char *fmt, ...);
    static void UINFO(const char *c, int i, const char *fmt, ...);
    static void UWARNING(const char *c, int i, const char *fmt, ...);
    static void UTRACE(const char *c, int i, const char *fmt, ...);
    static void UERROR(const char *c, int i, const char *fmt, ...);
    static void UFATAL(const char *c, int i, const char *fmt, ...);
    static void doBeforeClose();
    static std::mutex lock;
private:
    static void writeLog(const char* c);
    static FILE* logFile;
};

class FileEncoder{
public:
    static const unsigned char B64EncodeMap[];
    static const char B64DecodeMap[];
    static FileEncoder* GetInstance();
    int GZipCompress(char *data,long nData,char *dest,long &nDest);
    int GZipDecompress(char *data,long nData,char* dest,long &nDest);
    static void Base64Encode(char *data, long nData,char* dest,long &nDest);
    static void Base64Decode(char *data, long nData,char* dest,long &nDest);
    static std::string toHex(std::string src);
    static std::string toHex(const char* src,long size);
    static std::string toHex(const unsigned char* src,long size);
    static void MD5(char* data,long nData,char* dest);
    static void GetFileMD5(const char* path,char* dest);
    static void SHA1(char *data,long nData,char* dest);
    static void GetFileSHA1(const char* path,char* dest);
    static void SHA256(char *data,long nData,char* dest);
    static void GetFileSHA256(const char* path,char* dest);

private:
    static FileEncoder* Instance;
};

#define BUFFER_SIZE 512
class IOUtils{
public:
    static int traverseDirectory(std::string path,std::vector<std::string> &vct,int &depth,bool includeFolder);
    static void writeFile(const char* fileName,const char* content,long length);
    static void writeFile(const char* fileName,const char* content);
    static void writeFile(FILE *f,const char* content,long length);
    static void writeFile(std::ofstream& stream,const char* content,long length);
    static void appendFile(const char* fileName,const char* content,long length);
    static void appendFile(const char* fileName,const char* content);
    static void readFile(const char* fileName,char* dest,long length);
    static void readFile(const char* fileName,char* dest);
    static void readFile(const char* fileName,std::string dest);
    static void readFile(FILE *f,char* content,long length);
    static void readFile(std::ifstream& stream,char* content,long length);
    static int createFile(const char* fileName);
    static int createDir(const char* dirName);
    static void createDirIfNotExists(const char* dirName);
    static void createFileIfNotExists(const char* fileName);
    static void downloadFile(const char* url,const char* savePath);
    static void download(const char* url,char* dest);
    static long getContentLength(const char* url);
    static int dirExists(const char* dirName);
    static int fileExists(const char* fileName);
};

//@ABANDONED
//Archive Format:

/*
FILE HEADER       |FIRST CHUNK                                                           |ANOTHER CHUNK                              |FILE END
A R C H I V E 0x01 0x01 [NAME]          0x01 [CONTENT_LENGTH] 0x01 [CONTENT]         0x01 [NAME] 0x01 [CONTENT_LENGTH] 0x01 [CONTENT] EOF
(8 Bytes)         |    |(MAX 128 Bytes)|    |(MAX 64 Bytes)  |    |(MAX 10^63 Bytes)|    |(SAME AS FRONT)
*/

#define _SEPARATOR 0x01
struct archive_chunk{
    char NAME;
    int CONTENT_LENGTH;
    char *CONTENT;
    char SEPARATOR;
};

class Archive{
public:
    Archive(const char* fileName);
    ~Archive();
    void writeFile(char* chunkName,char* chunkContent,long contentLength);
    void readFile(char* chunkName,char* chunkContent,long &contentLength);
    bool haveNext();
    static bool available(const char* fileName);

private:
    std::ifstream in;
    std::ofstream out;
    void writeContent(archive_chunk *f,char* content,long contentLength);
    void readContent(archive_chunk *f,char* content,long &contentLength);
};

#endif //BLOCKSTUDIO_UTILS_H
