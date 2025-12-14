#include "functions.h"
#include "commands.h"

int main() {


  load_history();
  looping:
  cout << unitbuf;
  cerr << unitbuf;

  cout<<"$ ";
  string input ;
  take_input(input);
  vector<string>commands;
  string file="";
  string errorfile="";
  bool append_file=0,append_err=0;

  split_by_spaces(input,commands,file,errorfile,append_file,append_err);
  vector<vector<string>> partitions;
  split_with_pipe(commands,partitions);
  if(partitions.size()==1){
    cout<<redirect(partitions[0],file,errorfile,append_file,append_err);
  }
  cout<<execute(partitions);
  goto looping;
}
