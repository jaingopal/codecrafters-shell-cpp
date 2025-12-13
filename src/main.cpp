#include "functions.h"
#include "commands.h"


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
    echo(input);
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
