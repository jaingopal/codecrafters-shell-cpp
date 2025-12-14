#include "globals.h"


bool is_exec(string & path);
string run(string& path,const vector<string>& commands);
void get_execFiles();
string invalid_type(string& command);
string check_type(string& command,string& folder);
void split_by_spaces(const string& str,vector<string>& words,string& filename,string& errorfile,bool& append_file,bool& append_error);
void redirect(vector<string>& commands,string& filename,string& errorname,bool append_file,bool append_err);
int main();
void parent_dir();