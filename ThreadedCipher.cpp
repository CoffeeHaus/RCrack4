#include "ThreadedCipher.h"


 std::vector<std::string>* ThreadedCipher::s_ptrWordlist = NULL;
 unsigned char* ThreadedCipher::s_ptrCipher = NULL;
 char* ThreadedCipher::s_ptrOutfile = NULL;
 int ThreadedCipher::s_Positional = 0;
 int ThreadedCipher::s_Verbose = 0;
 int ThreadedCipher::s_CipherLen = 0;
 int ThreadedCipher::s_LongestWord = 0;

ThreadedCipher::ThreadedCipher() { };
 
ThreadedCipher::~ThreadedCipher()
{
    if(outCipher)free(outCipher );
};
void ThreadedCipher::operator()(Key k, int keys, int setThread)
{
    //set key
    key.setKey(k);
    thread = setThread;
    keysCovered = keys;

    threadStartPrint();

    outCipher = (unsigned char*)malloc(ThreadedCipher::s_CipherLen + 1);
    if (ThreadedCipher::s_ptrWordlist != NULL)
    {
        wordList();

    }
    else
    {
        bruteForceAscii();
    }
    threadEndPrint();

}

void ThreadedCipher::bruteForceAscii()
{
    for (int x = 0; x < keysCovered; x++)
    {
        encryptCipher();
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
    int counter = 0;

    char* strKey = (char*)malloc(key.length() * 3); //malloc
    key.GetKeyString(strKey);

    char * cipherStr = cipherToString(); // malloc

    if (ThreadedCipher::s_Verbose >= 1)
    {
        mtxPrintf.lock();// print to cmd
        fflush(stdout);
        printf("[\!] KEY[% s] -> % s\n", strKey, cipherStr);
        mtxPrintf.unlock();
    }

    if (ThreadedCipher::s_ptrOutfile)
    {
        mtxPrintToString.lock(); // print to file
        FILE* pFileTXT = fopen(s_ptrOutfile, "a");
        fprintf(pFileTXT, "[\!] KEY[%s] -> %s\n", strKey, cipherStr); // append instead of overwrite
        fclose(pFileTXT);

        mtxPrintToString.unlock();
    }

    if (cipherStr)free(cipherStr);// free malloc
    if(strKey)free(strKey);//free malloc
}

void ThreadedCipher::encryptCipher()
{
    int length = 0;// If the word list is positional, then there is no reason to encrypt all of the cipher
    if (ThreadedCipher::s_Positional)
    {
        memcpy(outCipher, ThreadedCipher::s_ptrCipher, ThreadedCipher::s_LongestWord + 1);
        length = ThreadedCipher::s_LongestWord;
    }
    else
    {
        memcpy(outCipher, ThreadedCipher::s_ptrCipher, ThreadedCipher::s_CipherLen + 1);
        length = ThreadedCipher::s_CipherLen;
    }
    
    int SubBox[256];
    int a = 0;
    int b = 0;
    unsigned char* strKey = key.key();
    unsigned char swap = 0;
    
    unsigned int len = key.length();

    for (a = 0; a < 256; a++)
    {
        SubBox[a] = a;
    }

    for (a = 0; a < 256; a++)
    {
        b = (b + SubBox[a] + strKey[a % len]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
    }

    //Encrypt the data
    a = 0;
    b = 0;
    for (long Offset = 0; Offset < length; Offset++)
    {
        a = (a + 1) % 256;
        b = (b + SubBox[a]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
        outCipher[Offset] ^= SubBox[(SubBox[a] + SubBox[b]) % 256];
    }
    outCipher[length + 1] = 0;
}

void ThreadedCipher::threadStartPrint()
{
    if (opt.verbose >= 1)
    {
        char* strkey = (char*)malloc(key.length() * 3);;
        key.GetKeyString(strkey);
        mtxPrintf.lock();
        printf("[+]Thread %d started with key [%s]\n", thread, strkey);
        mtxPrintf.unlock();
        if (strkey)free(strkey);
    }
}

void ThreadedCipher::threadEndPrint()
{
    if (opt.verbose >= 1)
    {
        char* strkey = (char*)malloc(key.length() * 3);;
        key.GetKeyString(strkey);
        mtxPrintf.lock();
        printf("[-]Thread %d ended with key [%s]\n", thread, strkey);
        mtxPrintf.unlock();
        if (strkey)free(strkey);
    }
}

void ThreadedCipher::wordList()
{
    for (int x = 0; x < keysCovered; x++)
    {
        encryptCipher();
        if (doesMatchWordList())
        {
            print();
        }
        key++;
    }

}

bool ThreadedCipher::doesMatchWordList()
{
    if (ThreadedCipher::s_Positional)
    {
        for (auto& element : *ThreadedCipher::s_ptrWordlist) 
        {
            bool isWordMatch = true;
            for (int x = 0; x < element.size(); x++)
                {
                if (element[x] != outCipher[x] || element[x] == '*')
                    {
                    isWordMatch = false;
                    }

                }
            if (isWordMatch) { return true; }

                    
        }
    }
    else
    {
        for (auto& element : *ThreadedCipher::s_ptrWordlist)
        {
            for (int x = 0; x < ThreadedCipher::s_CipherLen -element.size(); x++)
            {
                if (element[0] == outCipher[x] || outCipher[x] == '*')
                {
                    bool isWordMatch = true;
                    for (int z = 0; z < element.size(); z++)
                    {
                        if (element[z] != outCipher[x + z])
                            isWordMatch = false;
                    }
                    if (isWordMatch)return true;
                }
            }

        }
    }
     
    return false;
    
}

char* ThreadedCipher::cipherToString()
{
    char* ptr_output = (char*)malloc(ThreadedCipher::s_CipherLen * 3);
    char temp[3]{};
    bool lastWasHex = false;    
    int counter = 0;
    

    for (int x = 0; x < ThreadedCipher::s_CipherLen; x++)
    {
        // if ascii
        if (outCipher[x] > 31 && outCipher[x] < 127)
        {
            ptr_output[counter++] = outCipher[x];
            if (lastWasHex)
            {
                lastWasHex = false;
                ptr_output[counter++] = ' ';
            }
        }
        else
        {
            
            sprintf(temp, "%02x", outCipher[x]);
            if (!lastWasHex)ptr_output[counter++] = ' ';
            ptr_output[counter++] = temp[0];
            ptr_output[counter++] = temp[1];
            ptr_output[counter++] = ' ';
            lastWasHex = true;
        }

    }
    ptr_output[counter] = 0;
    return ptr_output;
}