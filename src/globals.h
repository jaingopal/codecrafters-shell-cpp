#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;

#ifndef cwd
    #define cwd 
    string cwd;
#endif

#ifndef exec_folders
    #define exec_folders
    vector<fs::path> exec_folders;
#endif

#ifndef builtin
    #define builtin
    vector<string>builtin;
#endif
    