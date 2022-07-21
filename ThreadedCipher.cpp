#include "ThreadedCipher.h"


 std::vector<std::string>* ThreadedCipher::s_ptrWordlist = NULL;
 unsigned char* ThreadedCipher::s_ptrCipher = NULL;
 char* ThreadedCipher::s_ptrOutfile = NULL;
 int ThreadedCipher::s_Positional = 0;
 int ThreadedCipher::s_Verbose = 0;
 int ThreadedCipher::s_CipherLen = 0;

ThreadedCipher::ThreadedCipher() { };
 
ThreadedCipher::~ThreadedCipher() {};
void ThreadedCipher::operator()(Key k, int keys, int setThread)
{
    //set key
    key.setKey(k);
    thread = setThread;
    keysCovered = keys;
    ThreadStartPrint();
    outCipher = (unsigned char*)malloc(ThreadedCipher::s_CipherLen + 1);
    if (ThreadedCipher::s_ptrWordlist != NULL)
    {
        WordList();

    }
    else
    {
        BruteForceAscii();
    }
    ThreadEndPrint();

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
    for (int x = 0; x < ThreadedCipher::s_CipherLen; x++)
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
    char* strKey = (char*)malloc(key.length() * 3); //malloc

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
    memcpy(outCipher, ThreadedCipher::s_ptrCipher, ThreadedCipher::s_CipherLen + 1);
    int SubBox[256];
    int a = 0;
    int b = 0;
    unsigned char swap = 0;

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
    for (long Offset = 0; Offset < ThreadedCipher::s_CipherLen; Offset++)
    {
        a = (a + 1) % 256;
        b = (b + SubBox[a]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
        outCipher[Offset] ^= SubBox[(SubBox[a] + SubBox[b]) % 256];
    }
    outCipher[ThreadedCipher::s_CipherLen + 1] = 0;
}

void ThreadedCipher::ThreadStartPrint()
{
    if (opt.verbose >= 1)
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
    if (opt.verbose >= 1)
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

void ThreadedCipher::PositionalKey()
{

}