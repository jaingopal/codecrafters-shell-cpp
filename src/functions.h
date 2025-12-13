#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;
bool is_exec(string & path);
void run(string& path,const vector<string>& commands);
void get_execFiles();
void invalid_type(string& command);
string check_type(string& command,string& folder);
vector<string> split_by_spaces(const string& str);