#include "functions.h"
#include "commands.h"

int main() {

  cout << unitbuf;
  cerr << unitbuf;

  cout<<"$ ";

  string input ;
  getline(cin,input);
  vector<string>commands;
  string file="";
  string errorfile="";
  bool append_file=0,append_err=0;
  split_by_spaces(input,commands,file,errorfile,append_file,append_err);
  redirect(commands,file,errorfile,append_file,append_err);
}
