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
private:
    void setlength();
    unsigned char _key[256]= {0};
    int _length = 0;

public:
    bool atMax = false;
    Key& operator++();       
    Key operator++(int);     
    void printKey();
    void setKey(unsigned long long number) ;
    Key() {};
    Key(unsigned long long number);
    unsigned char * key();
    int length() { return _length; };



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
Key::Key(unsigned long long v)
{
    setKey(v);

}
void Key::setKey(unsigned long long number)
{
    int x = 0;
    int y = 0;
    bool first = false;

    for (x = 0; x < 8; x++)
    {
        _key[255 - x] = (number >> (8 * x)) & 0xff;

    }
    x = 0;
    while (x < 8) // this moves the key to the front of the array
    {
        if (!first && _key[x] != 0)
        {
            first = true;
            _key[y++] = _key[x];
        }
        else if (first)
        {
            _key[y++] = _key[x];
        }
        x++;
    }
    while (y < 8)//zeroing the remaining values
    {
        _key[y++] = 0;
    }
    setlength();

};
void Key::printKey()
{
    printf("[");
    for (int x = 0; x < _length; x++)
    {
        printf("%02x ", key()[x]);

    }
    printf("]");
}
Key& Key::operator++()
{
    int x = 255;
    int max = 0;
    while (x > -1)
    {
        if (_key[x] >= 255)
        {
            _key[x--] = 0;
            max = 256 - x;
        }
        else
        {
            _key[x] ++;
            x = -2;
        }
    }
    if (x == -1)
    {
        atMax = true;
    }

    if (_length < max) _length = max;

    return *this;
}
Key Key::operator++(int)
{
    Key temp = *this;
    ++* this;
    return temp;
}
unsigned char* Key::key()
{
    int x = 0;
    for(int x = 0; x < 256;x++)
    {
        if (_key[x] != 0)
        {
            unsigned char* t = &_key[x];
            return t;
        }
    }
}
void Key::setlength()
{
    int x = 255;
    for (int x = 0; x <256;x++)
    {
        if (_key[x] != 0)
        {
            _length = 256-x;
        }
    }
}

int ParseInputs(int argc, char** argv);
void BruteForce(Cipher cipher, unsigned long long min = 1, unsigned long long max = ULLONG_MAX);
void PrintHelp();
int readFile(char*, std::vector<char>*);