#pragma once
#include <string>
#include <iostream>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <fstream>
#include <vector>
#define _CRT_NONSTDC_NO_DEPRECATE
using namespace std;


class Key
{
public:
    void printKey();
    void setKey(unsigned long long number) ;
    Key() {};
    Key(unsigned long long number);
    unsigned long long value;
    unsigned char key[256];
    int length = 0;

};

class Cipher 
{       
public:

    Cipher() {};
    Cipher(vector <char> vector);
    ~Cipher() { free(cipher);free(outCipher); };

    void printOutCipher();
    void printCipher();
    void Encrypt(Key);
    bool isAsciiOutput();


    unsigned char * cipher;
    unsigned char * outCipher;
    unsigned int length = 0;        

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
        b = (b + SubBox[a] + key.key[a % key.length]) % 256;
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

Key::Key(unsigned long long v)
{
    value = v;
    setKey(value);

}
void Key::setKey(unsigned long long number) 
{
    int x = 0;
    int y = 0;
    bool first = false;

    for (x = 0; x < 8; x++)
    {
        key[7 - x] = (number >> (8 * x)) & 0xff;

    }
    x = 0;
    while (x < 8) // this moves the key to the front of the array
    {
        if (!first && key[x] != 0)
        {
            first = true;
            length = 8 - x;
            key[y++] = key[x];
        }
        else if (first)
        {
            key[y++] = key[x];
        }
        x++;
    }
    while (y < 8)//zeroing the remaining values
    {
        key[y++] = 0;
    }


};
void Key::printKey()
{
    printf("[");
    for (int x = 0; x < length; x++)
    {
        printf("%02x ", key[x]);

    }
    printf("]");
}




int ParseInputs(int argc, char** argv);
void BruteForce(Cipher cipher, unsigned long long min = 1, unsigned long long max = ULLONG_MAX);
void PrintHelp();
int readFile(char*, std::vector<char>*);