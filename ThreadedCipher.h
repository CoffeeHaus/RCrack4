#pragma once
#include <malloc.h>
#include "Key.h"


extern std::mutex mtxTcp;

extern int VERBOSE;

class ThreadedCipher
{
public:

    ThreadedCipher() { cipher = NULL; outCipher = NULL; };
    ~ThreadedCipher();
    void operator()(unsigned char*, int cipherSize, Key key, int, int mode, int thread);
    void print();
    void printKey();
    void BruteForceAscii();
    void PositionalKey();
    void EncryptCipher();
    bool isAscii();
    void ThreadStartPrint();
    void ThreadEndPrint();
    void WordList();
    unsigned char* cipher;
    unsigned char* outCipher;
    unsigned int length = 0;
    Key key;
    int keysCovered = 1;
    int thread;
    int mode;
};


void ThreadedCipher::operator()(unsigned char* inCipher, int cipherSize, Key k, int keys, int inMode, int setThread)
{
    //set key
    key.setKey(k);
    thread = setThread;
    keysCovered = keys;
    length = cipherSize;
    mode = inMode;

    ThreadStartPrint();

    cipher = (unsigned char*)malloc(cipherSize + 1);
    outCipher = (unsigned char*)malloc(cipherSize + 1);

    if (cipher)memcpy(cipher, inCipher, cipherSize);


    if (mode == 1)
    {
        BruteForceAscii();
    }
    else if (mode == 2)
    {
        PositionalKey();
    }
    else if (mode == 3)
    {
        WordList();
    }
    ThreadEndPrint();

}


ThreadedCipher::~ThreadedCipher()
{
    if (cipher)free(cipher);
    if (outCipher)free(outCipher);
}

void ThreadedCipher::BruteForceAscii()
{
    for (int x = 0; x < keysCovered; x++)
    {
        EncryptCipher();
        if (isAscii())
        {
            print();
        }
        key++;
    }
}

bool ThreadedCipher::isAscii()
{
    bool isAscii = true;
    for (int x = 0; x < length; x++)
    {
        if (outCipher[x] < 32 || outCipher[x] > 126)
        {
            isAscii = false;

        }
    }
    return isAscii;
}

void ThreadedCipher::print()
{
    char str[] = "[\!] KEY[%s] -> %s\n";
    char outstr[500];
    char strcipher[3000];
    int counter = 0;
    bool lastWasHex = false;

    for (int x = 0; x < length; x++)
    {
        // if ascii
        if (outCipher[x] > 31 && outCipher[x] < 127)
        {
            strcipher[counter++] = outCipher[x];
            if (lastWasHex)
            {
                lastWasHex = false;
                printf(" ");
            }
        }
        else
        {
            char temp[3]{};
            sprintf(temp, "%02x", outCipher[x]);
            if (!lastWasHex)strcipher[counter++] = ' ';
            strcipher[counter++] = temp[0];
            strcipher[counter++] = temp[1];
            strcipher[counter++] = ' ';
            lastWasHex = true;
        }

    }
    strcipher[counter] = 0;

    counter = 0;
    lastWasHex = false;
    char * strKey = (char *) malloc(key.length()*3); //malloc

    key.GetKeyString(strKey);
    

    sprintf(outstr, str, strKey, strcipher);

    mtxTcp.lock();
    fflush(stdout);
    printf("%s", outstr);
    mtxTcp.unlock();

    free(strKey);//free malloc
}

void ThreadedCipher::EncryptCipher()
{
    int SubBox[256]{};
    int a = 0;
    int b = 0;
    int swap = 0;

    for (a = 0; a < 256; a++)
    {
        SubBox[a] = a;
    }

    for (a = 0; a < 256; a++)
    {
        b = (b + SubBox[a] + key.key()[a % key.length()]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
    }

    //Encrypt the data
    for (long Offset = 0; Offset < length; Offset++)
    {
        a = (a + 1) % 256;
        b = (b + SubBox[a]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
        outCipher[Offset] ^= SubBox[(SubBox[a] + SubBox[b]) % 256];
    }
}

void ThreadedCipher::ThreadStartPrint()
{
    if (VERBOSE == 1)
    {
        char* strkey = (char*)malloc(key.length() * 3);;
        key.GetKeyString(strkey);
        mtxTcp.lock();
        printf("[+]Thread %d started with key [%s]\n", thread, strkey);
        mtxTcp.unlock();
        if (strkey)free(strkey);
    }
}

void ThreadedCipher::ThreadEndPrint()
{
    if (VERBOSE == 1)
    {
        char* strkey = (char*)malloc(key.length() * 3);;
        key.GetKeyString(strkey);
        mtxTcp.lock();
        printf("[-]Thread %d ended with key [%s]\n", thread, strkey);
        mtxTcp.unlock();
        if (strkey)free(strkey);
    }
}

void ThreadedCipher::WordList()
{

}

void ThreadedCipher::PositionalKey()
{

}