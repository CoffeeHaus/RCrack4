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
#include "Options.h"

#define _CRT_NONSTDC_NO_DEPRECATE
using namespace std;



std::mutex mtxFlagFound;
std::mutex mtxTcp;



void WriteFile();
struct Options ParseInputs(int argc, char** argv);
int readCipherFile(char*, std::vector<char>*);
int multiThreadedBruteForce(vector<char>);
void readWordListFile(char* file, std::vector<string>* data);
void PrintHelp();