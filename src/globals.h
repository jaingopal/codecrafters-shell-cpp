#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <algorithm>
using namespace std;

namespace fs = std::filesystem;

extern string cwd;
extern vector<fs::path> exec_folders;
extern vector<string>builtin;
extern vector<string>history;
class output{
public:
    string str;
    string error;
};

#endif

