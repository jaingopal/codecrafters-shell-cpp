#include "globals.h"

bool is_exec(string & path);
string run(string& path,const vector<string>& commands,string& message);
void get_execFiles();
string invalid_type(string& command);
string check_type(string& command,string& folder);
void split_by_spaces(const string& str,vector<string>& words,string& filename,string& errorfile,bool& append_file,bool& append_error);
string redirect(vector<string>& commands,string& filename,string& errorname,bool append_file,bool append_err);
int main();
void parent_dir();
void load_history();
void load_history_file();
void split_with_pipe(vector<string>& words,vector<vector<string>>& par);
string execute(vector<vector<string>>& partitions);
char get_ch();
void match_exec(const string& str,vector<string>& matching);