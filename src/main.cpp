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
  vector<string>commands;
  string filename;

  split_by_spaces(input,commands,filename);
  if(filename.size()){
    redirect(commands,filename);
  }
  if(commands[0]=="exit"){
    return 0;
  }
  else if(commands[0]=="echo"){
    cout<<echo(commands);
    main();
  }
  else if(commands[0]=="type"){
    cout<<type_main(commands);
    main();
  }

  else if(commands[0]=="pwd"){
    cout<<pwd();
    main();
  }
  else if(commands[0]=="cd"){
    cout<<cd_main(commands);
    main();
  }
  else{
    cout<<ext(commands);
    main();
  }

}
