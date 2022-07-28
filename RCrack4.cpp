
/***********************************************************
* RC4 Cracker tool
* coded by CoffeeHaus
***********************************************************/


#include "RCrack4.h"

int main(int argc, char *argv[])
{
    std::vector<char> fileData;
    std::vector<string> wordList;
    const int c_KeysPerThread = 4096;
    int longestWord = 0;
    struct Options opt = parseInputs(argc, argv);

    //attempts to read cipher
    readCipherFile(argv[1], &fileData);

    //attempts to readwordlist
    if (opt.ptrWordList)
    {
        longestWord = readWordListFile(opt.ptrWordList, &wordList);
    }

    //displays cipher data
    printf("[!] Cipher read from file: \n[!]%s[!]\n", fileData.data());



    //sets up cipher pointer to be read by threads
    ThreadedCipher::s_ptrCipher = (unsigned char*)malloc(fileData.size() + 1);
    if (ThreadedCipher::s_ptrCipher)
    {
        ThreadedCipher::s_ptrCipher = (unsigned char *)fileData.data();
        ThreadedCipher::s_ptrCipher[fileData.size()] = 0;
    }
    else
    {
        perror("malloc issue");
    }

    //Handle wordlist
    if (&wordList != NULL)
    {
        ThreadedCipher::s_ptrWordlist = &wordList;
        ThreadedCipher::s_LongestWord = longestWord;
    }

    ThreadedCipher::s_CipherLen = fileData.size();
    ThreadedCipher::s_Verbose = opt.verbose;
    ThreadedCipher::s_Positional = opt.positional;
    ThreadedCipher::s_ptrOutfile = opt.ptrOutFile;


    //starts key
    Key key = Key(opt.minBytes);
    vector<thread> threads;
    int currentKey = 0;
   


    if (ThreadedCipher::s_Verbose >= 1)
    {
        mtxPrintf.lock();// print to cmd
        fflush(stdout);
        char str[1000];
        key.GetKeyString(str);
        printf("[!] Starting with key[%s] \n", str);
        currentKey = 0;
        mtxPrintf.unlock();
        }

    for (int x = 0; x < opt.threads; x++)// seeds the original threads into a vector
    {
        threads.push_back(thread(ThreadedCipher(), key, c_KeysPerThread, x));
        key.Add(c_KeysPerThread);
    }

    while (!key.atMax && key.length() <= opt.maxBytes  && (!ThreadedCipher::s_FlagFound || !opt.firstfound))// loops through threads to see if one is available, also will end once one key is found if --firstfound is set to true
    {
        if (currentKey > c_KeysPerThread * 10000)
        {
            if (ThreadedCipher::s_Verbose >= 1)
            {
                mtxPrintf.lock();// print to cmd
                fflush(stdout);
                char str[1000];
                key.GetKeyString(str);
                printf("[!] Now testing key[%s] \n", str);
                currentKey = 0;
               mtxPrintf.unlock();
            }
        }

        for (int x = 0; x < opt.threads; x++)
        {
            if (threads[x].joinable())
            {
                threads[x].join();
                threads[x] = thread(ThreadedCipher(), key, c_KeysPerThread, x);
                key.Add(c_KeysPerThread);
                currentKey += c_KeysPerThread;
            }

        }

    }
    // waits for all threads to join
    for (int x = 0; x < opt.threads; x++)
    {
        threads[x].join();
    }

    //Memory leak stopping
    if (ThreadedCipher::s_ptrWordlist)free(ThreadedCipher::s_ptrWordlist);
    if (ThreadedCipher::s_ptrCipher)free(ThreadedCipher::s_ptrCipher);
    if (ThreadedCipher::s_ptrOutfile)free(ThreadedCipher::s_ptrOutfile);
    if (opt.ptrOutFile)  free(opt.ptrOutFile);
    if (opt.ptrWordList) free(opt.ptrWordList);
    if (ThreadedCipher::s_FlagFound == true)
    {
        exit(EXIT_SUCCESS);
    }
   else
    {
       exit(EXIT_FAILURE);
    }
}

void writeFile() 
{
    FILE* pFile;
    char buffer[1000];

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

struct Options parseInputs(int argc, char** argv)
{
    if (argc == 1)
    {
        printHelp();
    }
    struct Options opt;
    // check valid cipher 
    char* key;
    std::ifstream ifile;
    ifile.open(argv[1]);
        if (!ifile)
        {
        perror("Error opening file");
        printHelp();
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
        if (strcmp(argv[x], "-h") == 0 || strcmp(argv[x], "--help") == 0)
        {
            printHelp();

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
            if (opt.verbose = -1)opt.verbose = 1;
            x++;
        }
        else if (strcmp(argv[x], "-vv") == 0 || strcmp(argv[x], "--verbose2") == 0)
        {
            if(opt.verbose = -1)opt.verbose = 2;
            x++;
        }
        else if (strcmp(argv[x], "-q") == 0 || strcmp(argv[x], "--quiet") == 0)
        {
            opt.verbose = -1;
            x++;
        }
        else if (strcmp(argv[x], "-f") == 0 || strcmp(argv[x], "--firstfound") == 0)
        {
            opt.firstfound = true;
            x++;
        }
        else if (strcmp(argv[x], "-w") == 0 || strcmp(argv[x], "--word-list") == 0)
        {
            
            std::ifstream ifile;
            ifile.open(argv[x+1]);
            if (!ifile) {
                printf("Error: outfile not found");
            }

            opt.ptrWordList = (char*)malloc(sizeof(argv[x + 1]) + 1);
            if(opt.ptrWordList)strcpy(opt.ptrWordList, argv[x + 1]);
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

//read words list and builds a vector of strings
int readWordListFile(char* fileName, std::vector<string>* wordList)
{
    int longest = 0;
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            wordList->push_back(line.c_str());
            longest = line.size() > longest ? line.size() : longest;
        }
        file.close();
    }
    return longest;
}

void printHelp() {
    const char* help = "RCrack4 - RC4 encrpytion bruteforcer \n\n"
        "RCrack4 <HEXCIPHER> [options][-w wordlist]\n"
        "-o <filename> --out-file the file that is outputted\n"
        "-n <number> --min-bytes sets the min bytes of key, default is 1\n"
        "-x <number> --max-bytes sets the max bytes of key, default is 8\n"
        "-t <number> --threads will set the max number of threads to be run, default is 20\n"
        "-w <filename> --wordlist should be a text file with words searched\n"
        "-p --positional will treat the wordlist as position sensitve, use * for wildcards, much faster as it doesnt parse the whole cipher\n"
        "-k --keyasciionly will only test keys that are ascii characters\n"
        "-v --verbose shows more info\n"
        "-vv --verbose2 even more verbose"
        "-q --quiet mutes all output in cmd prompt\n"
        "-h --help calls this page\n";
    printf(help);
    exit(EXIT_FAILURE);
}