
#ifndef KEY_H
#define KEY_H
#include <iostream>
#include "Options.h"

class Key
{
private:

    unsigned char _key[256] = { 0 };
    int _length = 0;
    bool _asciiOnly = false;
public:

    Key() {};
    Key(int number);
    void setLength();
    Key& operator++();
    Key operator++(int);
    void SetAsciiOnly();
    bool isEqual(Key*);
    void printKey();
    void setKey(unsigned long long number);
    void setKey(Key);
    void Add(int value);
    int length() { return _length; };
    void GetKeyString(char*);
    unsigned char * getKey();

    bool atMax = false;
    unsigned char* key();

    int SubBox[256];
   




};

#endif KEY_H
