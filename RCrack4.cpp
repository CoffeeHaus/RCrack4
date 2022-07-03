
/***********************************************************
* RC4 Cracker tool
* coded by CoffeeHaus
* C++ Class
*RC4 classes intial code from:
* "Coded by Viotto © BreakingSecurity.net"
***********************************************************/

#include <iostream>
#include <string>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <fstream>
#include <vector>
#define _CRT_NONSTDC_NO_DEPRECATE

// Perform dictionary attack look for word in list
// Create attack that looks for strings with only Ascii characters
// perform attack looking for specific key 
// perform attack looking for key in specific location
// 


void PrintHex(unsigned char* sample);
int MakeKey(unsigned long long key, unsigned char* hkey);
void PrintHexKey(unsigned char* sample);
int ParseInputs(int argc, char** argv);
void BruteForce(unsigned char *, unsigned long long min = 0, unsigned long long max = ULLONG_MAX);
void BuildCipher(unsigned char* Key, unsigned int lenKey, unsigned char* string, unsigned int lenString);
void PrintHelp();
int readFile(char *, std::vector<char> *);
bool isCipherAscii(unsigned char* cipher);

int main(int argc, char** argv)
{
    unsigned char* cipher = {0x00};
    std::vector<char> fileData;

    int mode = ParseInputs(argc, argv);
    int len = readFile(argv[1], &fileData);

    switch (mode)
    {
    case -1:// something broke
        break;
    case 0:
        PrintHelp();
        break;

    case 1: 
        BruteForce(cipher);
        break;

    default:
        break;
    }

}
void BruteForce(unsigned char * cipher ,unsigned long long min, unsigned long long max )
{
    unsigned char hkey[10]{};
    int len = 0;
    int cipherLen = 0;
    for (unsigned long long x = min; x < max; x++)
    {
        
        unsigned char te[10];
        long p = 2712847316;//a1b2c3d4
        len = MakeKey(x, hkey);
        BuildCipher(hkey, len, cipher, cipherLen);
        if (isCipherAscii(cipher)) { printf(" %s", cipher); }


        


    }

}
bool isCipherAscii(unsigned char * cipher)
{
    return false;
}
//Will readfile and return the length of the data.
int readFile(char * file, std::vector<char> *data)
{
    std::ifstream input(file, std::ios::binary);

    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(input)),
        (std::istreambuf_iterator<char>()));
    *data = bytes;
    input.close();
    return bytes.size();
}


//
void BuildCipher(unsigned char* key, unsigned int lenKey, unsigned char* cipher, unsigned int lenCipher)
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
        b = (b + SubBox[a] + key[a % lenKey]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
    }

    //Encrypt the data
    for (long Offset = 0; Offset < lenCipher; Offset++)
    {
        a = (a + 1) % 256;
        b = (b + SubBox[a]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
        cipher[Offset] ^= SubBox[(SubBox[a] + SubBox[b]) % 256];
    }
}



int MakeKey(unsigned long long key, unsigned char * hkey)
{
    for (int x = 0; x < 8; x++)
    {
        hkey[7 - x] = (char)(key >> x * 8);
    }
    return 0;
}
void PrintHex(unsigned char *sample)
{
    while (*sample)
    {
        if (*sample > 31 && *sample < 127)printf("%c ", *(sample++));
        else printf("%x ", *(sample++));
        
    }
    printf("\n");
}
void PrintHexKey(unsigned char* sample)
{
    for(int x = 0; x<4;x++)
    {
        printf("%x ", sample[x]);

    }
    printf("\n");
}
int ParseInputs(int argc, char** argv)
{
    // check valid cipher 
    std::ifstream ifile;
    ifile.open(argv[1]);
    if (!ifile) {
        printf("Error: Cipher file not found");
        return -1;
    }

    return 1;
}

int DictionaryAttack()
{
    return 0;
}

void PrintHelp() {
    printf("Usage:RCrack4 <HEXCIPHER> [-] [-w wordlist]\n");
    printf("\t\t\
\
        Look, basically you input your cipher after -c then choose if you want a brute force (-b) or\n \
		dictionary (-d) attack. With a brute force attack you can choose what charsets you want to\n \
		include such as lower (l), upper (u), numeric (n), or symbols (s), just include them as one\n \
		string, no spaces. Or you can even specify a custom charset (-m), then you provide the upper\n \
		and lower limits (-l). Default charset is lun, default limits are 1 4. For a dictionary attack\n \
		just be sure to include a wordlist (-w).\n");
    exit(EXIT_SUCCESS);
}

/*
* RCrack4 -f HexCipher 
returns 0 on atleast one match found
returns 1 on failure to find any matches
-h --help displays help page
-w --wordlist species wordlist
-q --quiet no output


by default program will run against cipher up to 4 bytes of key, returning all key and text pairs that contain only ascii characters.
-

*/


void m()
{
    printf("RC4 cracker start!\n");
    unsigned char sample[] = { 0xF4 ,0x73 ,0xf1 ,0x30,0x24 ,0xb5 ,0x4f ,0x35 ,0x58 ,0xf1 ,0x62 ,0x48 ,0x55 ,0x00 };

    unsigned char goal[] = "Flag";
    uint32_t key;

    for (unsigned long x = 16777215; x < 4294967295; x++)
    {
        key = x;
        unsigned char sample2[] = { 0xF4 ,0x73 ,0xf1 ,0x30,0x24 ,0xb5 ,0x4f ,0x35 ,0x58 ,0xf1 ,0x62 ,0x48 ,0x55 ,0x00 };
        unsigned char hexString[5];
        //makeKey(key, hexString);

        //enc.RC4(sample2, 13);
        //printHex(sample2);
        if (sample2[0] == 'F' && sample2[1] == 'l' && sample2[2] == 'a' && sample2[3] == 'g')
        {
            printf("!!!! %s ", sample2);
            // printHexKey(hexString);
            break;
        }
        if ((x + 1) % 16777216 == 0)
        {
            // printHexKey(hexString);
        }


    }


}