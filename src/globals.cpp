#include "globals.h"
string cwd;
vector<string>builtin={"echo","exit","history","pwd","type"};
vector<fs::path> exec_folders={};
vector<string>history;
int hist_append_ind=0;