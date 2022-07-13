#pragma once
#include <string>
#include <iostream>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <fstream>
#include <vector>
#include <thread>
#include <sstream>
#include <mutex>
#include <chrono>
#include "Key.h"
#include "ThreadedCipher.h"


#define _CRT_NONSTDC_NO_DEPRECATE
using namespace std;

int MAX_BYTES = 256;
int MIN_BYTES = 0;
int VERBOSE = 0;
int THREADS = 20;
std::mutex mtxTcp;

class Cipher 
{       
public:

    Cipher() {};
    Cipher(vector <char> vector);
    ~Cipher() { free(cipher);free(outCipher); };

    void printOutCipher();
    void printCipher();
    void Encrypt(Key key);
    bool isAsciiOutput();
    bool doesContain(char* value, int valueLen);
    bool doesContainLocationBased(char* value, int valueLen);

    unsigned char * cipher;
    unsigned char * outCipher;
    unsigned int length = 0;
    Key key;

};



Cipher::Cipher(vector <char> vector)
{
    cipher = (unsigned char*)malloc(vector.size());
    outCipher = (unsigned char*)malloc(vector.size());

    if (cipher)
    {
        for (int x = 0; x < vector.size(); x++)
        {
            cipher[x] = vector[x];
        }
        cipher[vector.size()] = 0; // makes char * null terminated
    }
    length = vector.size();

}

bool Cipher::isAsciiOutput()
{
    for (int x = 0; x < length; x++)
    {
        if (outCipher[x] < 32 || outCipher[x] > 126)
        {
            return false;
        }
    }
    return true;
}
void Cipher::Encrypt(Key key)
{
    int SubBox[256];
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
void Cipher::printOutCipher()
{
    bool lastWasHex = false;
    for (int x = 0; x < length; x++)
    {
        if (outCipher[x] > 31 && outCipher[x] < 127)
        {
            printf("%c", outCipher[x]);
            if (lastWasHex)
            {
                lastWasHex = false;
                printf(" ");
            }
        }
        else
        {

            printf(" %02x", outCipher[x]);
            lastWasHex = true;
        }
    }
}
void Cipher::printCipher()
{
    bool lastWasHex = false;
    for (int x = 0; x < length; x++)
    {
        if (cipher[x] > 31 && cipher[x] < 127)
        {
            printf("%c", cipher[x]);
            if (lastWasHex)
            {
                lastWasHex = false;
                printf(" ");
            }
        }
        else
        {

            printf(" %02x", cipher[x]);
            lastWasHex = true;
        }
    }
}
bool Cipher::doesContain(char* value, int valueLen)
{
    int x = 0;
    int y = 0;
    int t = 0;
    while (x < length)
    {
        y = 0;
        t = x;
        while (outCipher[t] == value[y])
        {
            y++;
            t++;
            if (y == valueLen)
            {
                return true;
            }
            if (t >= length)
            {
                break;
            }
        }
        x++;
    }

    return false;
}
bool Cipher::doesContainLocationBased(char* value, int valueLen)
{
    int y = 0;
    int x = 0;
    while (outCipher[x] == value[y])
    {
        y++;
        x++;
        if (y == valueLen)
        {
            return true;
        }
        if (x >= length)
        {
            break;
        }
    }


    return false;
}



void AsyncFunction(unsigned char* cipher, int cipherLen, unsigned char * key, int keylen);
int ParseInputs(int argc, char** argv);
void BruteForce(Cipher cipher, unsigned long long min = 1, unsigned long long max = ULLONG_MAX);
void PrintHelp();
int readFile(char*, std::vector<char>*);
int KnownValue(Cipher cipher, char* value);
int multiThreadedBruteForce(Cipher cipher);