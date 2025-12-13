#ifndef GL_H
    #define GLH
    #include "globals.h"
#endif


void ext(vector<string>& commands);
void pwd();
void parent_dir();
void cd(const string& direc);
void cd_main(vector<string>& commands);
void type(string& command);
void type_main(vector<string>& commands);
void echo(string& str);
