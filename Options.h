#pragma once
struct Options {
    int maxBytes = 256;
    int minBytes = 0;
    int verbose = 0;
    int threads = 20;
    bool positional = false;
    char* ptrWordList = NULL;
    char* ptrOutFile = NULL;
    char* ptrCipher = NULL;
};