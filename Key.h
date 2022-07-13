#pragma once

#include <iostream>

class Key
{
private:
    void setlength();
    unsigned char _key[256] = { 0 };
    int _length = 0;

public:

    Key() {};
    Key(unsigned long long number);

    Key& operator++();
    Key operator++(int);
    bool isEqual(Key*);
    void printKey();
    void setKey(unsigned long long number);
    void setKey(Key);
    void Add(int value);
    int length() { return _length; };
    void GetKeyString(char*);


    bool atMax = false;
    unsigned char* key();
    int SubBox[256];




};

Key::Key(unsigned long long v)
{
    setKey(v);

}

//sets key to number, limited to shorter keys since long long only has 8 bytes
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
//prints key
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
    Add(1);

    return *this;
}
Key Key::operator++(int)
{
    Key temp = *this;
    ++* this;
    return temp;
}
// adds value to key
void Key::Add(int value)
{
    int x = 255;
    int carryValue = value;
    int max = 0;
    int t = 0;
    while (x > -1)
    {
        t = carryValue + _key[x];

        if (t >= 256)
        {
            carryValue = t / 256;
            _key[x] = t % 256;
            x--;
        }
        else
        {
            _key[x] = t;
            max = 256 - x;
            x = -2;
        }

    }
    if (x == -1)
    {
        atMax = true;
    }
    if (max > _length) _length = max;
}
unsigned char* Key::key()
{
    int x = 0;
    for (int x = 0; x < 256; x++)
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
    for (int x = 0; x < 256; x++)
    {
        if (_key[x] != 0)
        {
            _length = 256 - x;
            break;
        }
    }
}
bool Key::isEqual(Key* okey)
{
    if (length() != okey->length())return false;
    for (int x = 0; x < length();)
    {
        if (key()[x] != okey->key()[x])
            return false;
    }
    return true;
}
void Key::setKey(Key k)
{
    memcpy(_key, k._key, 256);
    setlength();
}
void Key::GetKeyString(char* out)
{
    int counter = 0;
    char temp[3]{ 0 };
    for (int x = 0; x < length(); x++)
    {

        sprintf(temp, "%02x", key()[x]);
        out[counter++] = temp[0];
        out[counter++] = temp[1];
        out[counter++] = ' ';
    }
    out[counter - 1] = 0;
}