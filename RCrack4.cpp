
/***********************************************************
* RC4 Cracker tool
* coded by CoffeeHaus
***********************************************************/


#include "RCrack4.h"





int main(int argc, char *argv[])
{
    std::vector<char> fileData;

    int mode = ParseInputs(argc, argv);
    int len = readFile(argv[1], &fileData);
    
    Cipher cipher= Cipher(fileData);
    printf("read file: \n");
    cipher.printCipher();
    printf("\n");
    int i;


    //mode = 1;

    switch (mode)
    {
    case -1:// something broke
        break;
    case 0:
        PrintHelp();

        break;

    case 1: 
        i = multiThreadedBruteForce(cipher);
        break;

    case 2:
        //KnownValue(cipher);
        break;

    case 3:
       //KnownValue(cipher);
        break;

    default:
        break;
    }

}

void WriteFile() 
{
    FILE* pFile;
    char buffer[100];

    pFile = fopen("myfile.txt", "r");
    if (pFile == NULL) perror("Error opening file");
    else
    {
        while (!feof(pFile))
        {
            if (fgets(buffer, 100, pFile) == NULL) break;
            fputs(buffer, stdout);
        }
        fclose(pFile);
    }
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
    //TODO FIX
    char outfile[300] = {0};
    char wordlist[300] = {0};

    int x = 1;
    while (x < argc)
    {
        if (strcmp(argv[x], "-o") == 0 || strcmp(argv[x], "--out-file") == 0)
        {
            strcpy(outfile, argv[x + 1]);
        }
        else if (strcmp(argv[x], "-n") == 0 || strcmp(argv[x], "--min-value") == 0)
        {
            MIN_BYTES = atoi(argv[x + 1]);
        }
        else if (strcmp(argv[x], "-t") == 0 || strcmp(argv[x], "--threads") == 0)
        {
            THREADS = atoi(argv[x + 1]);
        }
        else if (strcmp(argv[x], "-x") == 0 || strcmp(argv[x], "--max-value") == 0)
        {
            MAX_BYTES = atoi( argv[x + 1]);
        }
        else if (VERBOSE != -1 && (  strcmp(argv[x], "-v") == 0 || strcmp(argv[x], "--verbose") == 0 ))
        {
            VERBOSE = 1;
        }
        else if (strcmp(argv[x], "-q") == 0 || strcmp(argv[x], "--quiet") == 0)
        {
            VERBOSE = -1;
        }
        else if (strcmp(argv[x], "-w") == 0 || strcmp(argv[x], "--word-list") == 0)
        {
            strcpy(wordlist, argv[x + 1]);
        }

        x += 2;
        
    }

    return 1;
}

void BruteForce(Cipher cipher ,unsigned long long min, unsigned long long max )
{
    Key key= Key(min);

    while(!key.atMax)
    {
        cipher.Encrypt(key);
        if (cipher.isAsciiOutput()) 
        {
            key.printKey();
            printf(" -> ");
            cipher.printOutCipher();
            printf("\n");
        }
        key++;
    } // end loop

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

int KnownValue(Cipher cipher, char* value)
{
    Key key = Key(1);
    int bytes = 1;
    
    while (!key.atMax)
    {
        cipher.Encrypt(key);
        if (cipher.doesContainLocationBased(value, 5))
        {
            key.printKey();
            printf(" -> ");
            cipher.printOutCipher();
            printf("\n");
        }
        if (bytes != key.length())
        {

            printf("[Now using %i byte keys]\n", bytes);
            bytes = key.length();
        }
        key++;

    }
    return 1;
}

int multiThreadedBruteForce(Cipher cipher)
{
    //atomic <char>[400] cipher;
    
    Key key = Key(1);
    vector<thread> threads;
    int keysPerThread = 2048;

    for (int x = 0; x < THREADS; x++)// loops through threads to see if one is available
    {
            threads.push_back(thread(ThreadedCipher(), cipher.cipher, cipher.length, key, keysPerThread, 1, x));
            key.Add(keysPerThread);
    }

    while (!key.atMax && key.length() <= MAX_BYTES)
    {
        for (int x = 0; x < THREADS; x++)// loops through threads to see if one is available
        {
            if (threads[x].joinable())
            {
                threads[x].join();
                threads[x] = thread(ThreadedCipher(), cipher.cipher, cipher.length, key, keysPerThread, 1, x);
                key.Add(keysPerThread);
            }
            
        }

    } // end loop
    return 0;
}

void AsyncFunction(unsigned char *  cipher, int cipherLen, unsigned char * key, int keylen)
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
        b = (b + SubBox[a] + key[a % keylen]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
    }

    //Encrypt the data
    for (long Offset = 0; Offset < cipherLen; Offset++)
    {
        a = (a + 1) % 256;
        b = (b + SubBox[a]) % 256;
        swap = SubBox[a];
        SubBox[a] = SubBox[b];
        SubBox[b] = swap;
        cipher[Offset] ^= SubBox[(SubBox[a] + SubBox[b]) % 256];
    }
    bool isAscii = true;
    for (int x = 0; x < cipherLen; x++)
    {
        if (cipher[x] < 32 || cipher[x] > 126)
        {
            isAscii = false;
        }
    }
    char out[300] = {0};
    int v = 0;

    char hex[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
    out[v++] =  '[';
    for (int x = 0; x < keylen; x++)
    {
        int l = key[x] / 16;
        out[v++] = hex[l];
        out[v++] = hex[key[x] % 16];
        out[v++] = ' ';

    }
        
    out[v++] = ']';
    out[v++] = '\t';

    for (int x = 0; x < cipherLen; x++)
    {

        out[v++] = cipher[x];

    }
   // mtxTcp.lock();
   // printf("%s", out);
   // mtxTcp.unlock();

    fflush(stdout);

    if(isAscii)
        {
        mtxTcp.lock();
        printf("key %s\n", out);
        mtxTcp.unlock();
        }
    if (cipher[0] == 'F' && cipher[1] == 'l' && cipher[2] == 'a' && cipher[3] == 'g' && cipher[4] == '=')
    {
        mtxTcp.lock();
        printf("key %s -> %s \n", key, cipher);
        mtxTcp.unlock();

    }
    free(key);
    free(cipher);

}

void PrintHelp() {
    printf("RCrack4 - RC4 encrpytion bruteforcer \n");
    printf("Usage:\n");
    printf("RCrack4 <HEXCIPHER> [-] [-w wordlist]\n\
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
