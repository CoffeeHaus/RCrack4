
/***********************************************************
* RC4 Cracker tool
* coded by CoffeeHaus
***********************************************************/


#include "RCrack4.h"

int main(int argc, char *argv[])
{
    std::vector<char> fileData;
    std::vector<string> wordList;
    int c_KeysPerThread = 4096;

    struct Options opt = ParseInputs(argc, argv);
    readCipherFile(argv[1], &fileData);

    if (opt.ptrWordList)
    {
        readWordListFile(opt.ptrWordList, &wordList);
    }

    printf("[!] Cipher read from file: \n");
    printf("\n");
    char * tt = (char*)malloc(fileData.size() + 1);
    ThreadedCipher::s_ptrCipher = (unsigned char*)malloc(fileData.size() + 1);
  

    if (ThreadedCipher::s_ptrCipher)
    {
        ThreadedCipher::s_ptrCipher = (unsigned char *)fileData.data();
        ThreadedCipher::s_ptrCipher[fileData.size()] = 0;
    }

    ThreadedCipher::s_Verbose = opt.verbose;
    ThreadedCipher::s_Positional = opt.positional;
    ThreadedCipher::s_ptrOutfile = opt.ptrOutFile;
    ThreadedCipher::s_CipherLen = fileData.size();

    Key key = Key(opt.minBytes);
    vector<thread> threads;

    //test
    Key ke = Key(2);
    ke.Add(24673);
    ThreadedCipher ciph = ThreadedCipher();
    ciph(ke, c_KeysPerThread, 10);

    //end test

    for (int x = 0; x < opt.threads; x++)// seeds the original threads into a vector
    {
        threads.push_back(thread(ThreadedCipher(), key, c_KeysPerThread, x));
        key.Add(c_KeysPerThread);
    }

    while (!key.atMax && key.length() <= opt.maxBytes)
    {
        for (int x = 0; x < opt.threads; x++)// loops through threads to see if one is available
        {
            if (threads[x].joinable())
            {
                threads[x].join();
                threads[x] = thread(ThreadedCipher(), key, c_KeysPerThread, x);
                key.Add(c_KeysPerThread);
            }

        }

    }
    if (ThreadedCipher::s_ptrWordlist)free(ThreadedCipher::s_ptrWordlist);
    if (ThreadedCipher::s_ptrCipher)free(ThreadedCipher::s_ptrCipher);
    if (ThreadedCipher::s_ptrOutfile)free(ThreadedCipher::s_ptrOutfile);
    if (opt.ptrOutFile)  free(opt.ptrOutFile);
    if (opt.ptrWordList) free(opt.ptrWordList);
}

void WriteFile() 
{
    FILE* pFile;
    char buffer[100];

    pFile = fopen("output.txt", "a");
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

struct Options ParseInputs(int argc, char** argv)
{
    struct Options opt;
    // check valid cipher 
    char* key;
    std::ifstream ifile;
    ifile.open(argv[1]);
        if (!ifile)
        {
        perror("Error opening file");
        }

    //TODO FIX

    int x = 1;
    while (x < argc)
    {
        if (strcmp(argv[x], "-o") == 0 || strcmp(argv[x], "--out-file") == 0)
        {
            opt.ptrOutFile = (char*)malloc(sizeof(argv[x + 1] ) + 1);
            strcpy(opt.ptrOutFile, argv[x + 1]);
            x += 2;

        }
        else if (strcmp(argv[x], "-n") == 0 || strcmp(argv[x], "--min-bytes") == 0)
        {
            opt.minBytes = atoi(argv[x + 1]);
            x += 2;
        }
        else if (strcmp(argv[x], "-t") == 0 || strcmp(argv[x], "--threads") == 0)
        {
            opt.threads = atoi(argv[x + 1]);
            x += 2;
        }
        else if (strcmp(argv[x], "-x") == 0 || strcmp(argv[x], "--max-bytes") == 0)
        {
            opt.maxBytes = atoi( argv[x + 1]);
            x += 2;
        }
        else if (strcmp(argv[x], "-p") == 0 || strcmp(argv[x], "--positional") == 0)
        {
            opt.positional = true;
            x++;
        }
        else if ( strcmp(argv[x], "-k") == 0 || strcmp(argv[x], "--keyasciionly") == 0 )
        {
            //VERBOSE = 1;
            x++;
        }
        else if (strcmp(argv[x], "-v") == 0 || strcmp(argv[x], "--verbose") == 0)
        {
            opt.verbose = 1;
            x++;
        }
        else if (strcmp(argv[x], "-q") == 0 || strcmp(argv[x], "--quiet") == 0)
        {
            opt.verbose = -1;
            x++;
        }
        else if (strcmp(argv[x], "-s") == 0 || strcmp(argv[x], "--singleKey") == 0)
        {
           key = (char*)malloc(sizeof(argv[x + 1]) + 1);
            x += 2;
        }
        else if (strcmp(argv[x], "-w") == 0 || strcmp(argv[x], "--word-list") == 0)
        {
            
            std::ifstream ifile;
            ifile.open(argv[x+1]);
            if (!ifile) {
                printf("Error: outfile not found");
            }

            opt.ptrWordList = (char*)malloc(sizeof(argv[x + 1]) + 1);
            strcpy(opt.ptrWordList, argv[x + 1]);
            x += 2;
        }
        else
        {
            x++;
        }

        
        
    }

    return opt;
}

//Will readfile and return the length of the data.
int readCipherFile(char * file, std::vector<char> *data)
{
    std::ifstream input(file, std::ios::binary);

    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(input)),
        (std::istreambuf_iterator<char>()));
    *data = bytes;
    input.close();
    return bytes.size();
}


void readWordListFile(char* filea, std::vector<string>* data)
{
    std::ifstream file(filea);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // using printf() in all tests for consistency
            data->push_back(line.c_str());
        }
        file.close();
    }
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


