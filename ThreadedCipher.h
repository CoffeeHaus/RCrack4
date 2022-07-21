#pragma once
#include <malloc.h>
#include "Key.h"
#include "Options.h"
#include <mutex>
#include <vector>

extern std::mutex mtxTcp;


class ThreadedCipher
{
public:

    ThreadedCipher();
    ~ThreadedCipher();
    void operator()(Key k, int keys, int setThread);
    void print();
    void printKey();
    void BruteForceAscii();
    void PositionalKey();
    void EncryptCipher();
    bool isAscii();
    void ThreadStartPrint();
    void ThreadEndPrint();
    void WordList();

    static std::vector<std::string>* s_ptrWordlist;
    static unsigned char* s_ptrCipher ;
    static char* s_ptrOutfile;
    static int s_Positional;
    static int s_Verbose;
    static int s_CipherLen;

    unsigned char* outCipher;
    unsigned int length = 0;
    Key key;
    int keysCovered = 1;
    int thread;
    int mode;
    struct Options opt;
};
