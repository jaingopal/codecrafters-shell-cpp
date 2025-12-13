#include "functions.h"
#include "commands.h"

string cwd;
vector<string>builtin={"type","exit","echo","pwd"};
vector<fs::path> exec_folders={};

int main() {

  cout << unitbuf;
  cerr << unitbuf;

  cout<<"$ ";

  string input ;
  getline(cin,input);
  vector<string>commands=split_by_spaces(input);
  if(commands[0]=="exit"){
    return 0;
  }
  else if(commands[0]=="echo"){
    echo(commands);
    main();
  }
  else if(commands[0]=="type"){
    type_main(commands);
    main();
  }

  else if(commands[0]=="pwd"){
    pwd();
    main();
  }
  else if(commands[0]=="cd"){
    cd_main(commands);
    main();
  }
  else{
    ext(commands);
    main();
  }

}
