#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;


void ext(vector<string>& commands);
void pwd();
void parent_dir();
void cd(const string& direc);
void cd_main(vector<string>& commands);
void type(string& command);
void type_main(vector<string>& commands);
void echo(string& str);
