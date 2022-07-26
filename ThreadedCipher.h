#pragma once
#include <malloc.h>
#include "Key.h"
#include "Options.h"
#include <mutex>
#include <vector>

extern std::mutex mtxPrintf;
extern std::mutex mtxFlagFound;
extern std::mutex mtxPrintToString;

class ThreadedCipher
{
public:

    ThreadedCipher();
    ~ThreadedCipher();
    void operator()(Key k, int keys, int setThread);
    void print();
    void printKey();
    void bruteForceAscii();
    void encryptCipher();
    bool isAscii();
    void threadStartPrint();
    void threadEndPrint();
    void wordList();
    bool doesMatchWordList();
    char*  cipherToString();

    static std::vector<std::string>* s_ptrWordlist;
    static unsigned char* s_ptrCipher ;
    static char* s_ptrOutfile;
    static int s_Positional;
    static int s_Verbose;
    static int s_CipherLen;
    static int s_LongestWord;
    unsigned char* outCipher;

    Key key;
    int keysCovered = 1;
    int thread;
    int mode;
    struct Options opt;
};
