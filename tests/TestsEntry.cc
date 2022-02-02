#include <winsock2.h>
#include "UnitTest.hpp"
#include "../src/PatchDecoder.hpp"
#include "../src/PatchEncoder.hpp"
#include "../src/MallocAllocator.hpp"
#include "../src/RangedDownload.h"
#include "../src/ApplicationEventBus.h"
#include <random>
#include <fcntl.h>
#include <microtar.h>

void IOTests(){
    HEADER("IOTests")
    setbuf(stdout, nullptr);
    ASSERT_TRUE(IOUtils::createDir("test")==0);
    ASSERT_TRUE(IOUtils::createFile("testfile.dat")==0);
    ASSERT_TRUE(IOUtils::createFile("test.dat")==0);
    std::string test1="TEST STRING|1234|\n\tTEST";
    const char* test2="TEST STRING|1234|\n\tTEST";
    const char* test3=" STRING|1234|\n\tTESTTEST";
    const char* test4="TEST STRING|1234|\n\tTESTTEST STRING|1234|\n\tTESTTEST STRING|1234|\n\tTEST";
    const char* FILE_NAME="test.dat";
    FILE *f= fopen(FILE_NAME,"rb");
    std::ofstream os(FILE_NAME,std::ios::out|std::ios::binary);
    std::ifstream is(FILE_NAME,std::ios::in|std::ios::binary);
    IOUtils::writeFile(FILE_NAME,test1.c_str(),test1.size());
    IOUtils::writeFile(FILE_NAME,test2);
    IOUtils::writeFile(f,test2, strlen(test2));
    IOUtils::writeFile(os,test2, strlen(test2));
    IOUtils::appendFile(FILE_NAME,test1.c_str(),test1.size());
    IOUtils::appendFile(FILE_NAME,test2);
    char buffer[1024]={0};
    IOUtils::readFile(FILE_NAME,buffer,test1.size());
    ASSERT_STRING(buffer, test2);
    IOUtils::readFile(FILE_NAME,buffer);
    ASSERT_STRING(buffer, test4);
    IOUtils::readFile(f,buffer,test1.size());
    ASSERT_STRING(buffer, test2);
    is.seekg(4,std::ios::beg);
    IOUtils::readFile(is,buffer,test1.size());
    ASSERT_STRING(buffer, test3);
    std::vector<std::string> vct;
    int depth=-1;
    char cwd[128]={0};
    _getcwd(cwd,128);
    ASSERT_ZERO(IOUtils::traverseDirectory(cwd,vct,depth,true,false));
    for (auto & i : vct) {
        LOG_INFO("%s",i.c_str());
    }
    os.close();
    is.close();
    fclose(f);
    END()
}
void GenerateSrc(){
    std::random_device rd;
    std::default_random_engine random(rd());
    char old_c[10240]={0};
    char new_c[10240]={0};
    for (int i = 0; i < 5120; ++i) {
        old_c[i]=random()%127;
        new_c[i]=random()%127;
    }
    for (int i = 5120; i < 10240; ++i) {
        old_c[i]=random()%127;
        new_c[i]=old_c[i];
    }
    IOUtils::writeFile("new.dat",new_c,10240);
    IOUtils::writeFile("old.dat",old_c,10240);
    IOUtils::createFile("patch.dat");
    IOUtils::createFile("apply.dat");
}
void PatchTests(){
    HEADER("PatchTests")
    GenerateSrc();
    MallocAllocator allocator;
    xdl_set_allocator(&allocator);
    LOG_INFO("%s","====C R E A T E  P A T C H====");
    PatchEncoder old_file(1024,XDL_MMF_ATOMIC);
    old_file.load("old.dat");
    PatchEncoder new_file(1024,XDL_MMF_ATOMIC);
    new_file.load("new.dat");
    PatchEncoder patch(1024,XDL_MMF_ATOMIC);
    patch.diff(old_file,new_file);
    patch.store("patch.dat");
    LOG_INFO("%s","====A P P L Y  P A T C H====");
    PatchDecoder original_file(1024,XDL_MMF_ATOMIC);
    original_file.load("old.dat");
    PatchDecoder patch_file(1024,XDL_MMF_ATOMIC);
    patch_file.load("patch.dat");
    PatchDecoder apply_file(1024,XDL_MMF_ATOMIC);
    apply_file.patch(original_file,patch_file);
    apply_file.store("apply.dat");
    char apply_c[10240]={0};
    char new_c[10240]={0};
    IOUtils::readFile("new.dat",new_c,10240);
    IOUtils::readFile("apply.dat",apply_c,10240);
    ASSERT_HEX(new_c, apply_c);
    END()
}
void dl_callback(std::vector<node*> nodes,info* i){
    long total=0;
    for (auto & node : nodes) {
        total+=node->end-node->start;
    }
    LOG_INFO("Remain Bytes: %ld Progress: %0.2f%s Speed: %0.2f mb/s",total,i->progress*100,"%%",i->speed);
}
void DownloadTests() {
    HEADER("DownloadTests")
    setbuf(stdout, nullptr);
    RangedDownload download("https://mirrors.tuna.tsinghua.edu.cn/github-release/git-for-windows/git/Git%20for%20Windows%202.34.1/MinGit-2.34.1-busybox-64-bit.zip","git.tar.bz2",64);
    download.execute(dl_callback);
    char downloaded_md5[33]={0};
    FileEncoder::GetFileMD5("testdownload.dat",downloaded_md5);
    const char *expect_downloaded_md5="263fda3ef13e2c35a86c617b33258a7d";
    ASSERT_STRING(downloaded_md5,expect_downloaded_md5);
    char downloaded_sha1[41]={0};
    FileEncoder::GetFileSHA1("testdownload.dat",downloaded_sha1);
    const char *expect_download_sha1="890eccbd0e9fe7365860ae42561036cd03425f0f";
    ASSERT_STRING(downloaded_sha1,expect_download_sha1);
    char downloaded_sha256[65]={0};
    FileEncoder::GetFileSHA256("testdownload.dat",downloaded_sha256);
    const char *expect_download_sha256="de6df8c8727409fbafa37220cc0100fbb2bc51ded0f5bebdda4b23fa92d892b6";
    ASSERT_STRING(downloaded_sha256,expect_download_sha256);
    END()
}
void EncoderTests(){
    HEADER("EncoderTests")
    setbuf(stdout, nullptr);
    char str[1024]="This is a test string";
    char res[1024]={0};
    long nRes=1024;
    FileEncoder::Base64Encode(str, strlen(str), res, nRes);
    char dest[1024]={0};
    long nDest=1024;
    FileEncoder::Base64Decode(const_cast<char *>(res), strlen(res), dest, nDest);
    ASSERT_STRING(dest, str);
    char origin_md5[33]={0};
    char dest_md5[33]={0};
    FileEncoder::MD5(str, strlen(str),origin_md5);
    FileEncoder::MD5(dest, strlen(dest),dest_md5);
    ASSERT_STRING(origin_md5, dest_md5);
    char origin_sha1[41]={0};
    char dest_sha1[41]={0};
    FileEncoder::SHA1(str, strlen(str),origin_sha1);
    FileEncoder::SHA1(dest, strlen(str),dest_sha1);
    ASSERT_STRING(origin_sha1,dest_sha1);
    char origin_sha256[65]={0};
    char dest_sha256[65]={0};
    FileEncoder::SHA256(str, strlen(str),origin_sha256);
    FileEncoder::SHA256(dest, strlen(str),dest_sha256);
    ASSERT_STRING(origin_sha256,dest_sha256);
    END()
}
void EnvironmentTests(){
    HEADER("EnvironmentTests")
    std::vector<std::string> git;
    EnvironmentUtils::whereGitExists(git);
    for (auto it=git.begin();it!=git.end();++it) {
        LOG_INFO("GIT: %s",it->c_str());
    }
    std::vector<std::string> java;
    EnvironmentUtils::whereJavaExists(java);
    for (auto it=java.begin();it!=java.end();++it) {
        LOG_INFO("JAVA: %s",it->c_str());
    }

    END()
}
void CompressTests(){
    HEADER("CompressTests")
    std::vector<std::string> files;
    char cwd[255]={0};
    _getcwd(cwd,255);
    strcat(cwd,R"(\servers\test\apache-maven-3.6.0)");
    int depth=-1;
    IOUtils::traverseDirectory(cwd,files,depth,false,true);
    FileEncoder::TarEncode(files,"test.tar");
    FileEncoder::BZip2Decompress("git.tar.bz2","git/");
    END()
}
int main(){
    atexit(ApplicationEventBus::beforeClose);
    Log::INIT();
    //IOTests();
    //PatchTests();
    //DownloadTests();
    //EncoderTests();
    //EnvironmentTests();
    //CompressTests();
}
