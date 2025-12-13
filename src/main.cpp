#include <iostream>
#include <string>
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;
  cout<<"$ ";

  string input ;
  getline(cin,input);
  if(input=="exit"){
    return 0;
  }
  if(input.substr(0,5)=="echo "){
    cout<<input.substr(5)<<endl;
    main();
  }
  if(input.substr(0,5)=="type "){
    string command=input.substr(5);
    if(command=="type"||command=="exit"||command=="echo"){
      cout<<command<<" is a shell builtin"<<endl;
    }
    else{
      cout<<command<<": not found"<<endl;
    }
    main();
  }
  cout<<input<<": command not found"<<endl;
  main();

  // TODO: Uncomment the code below to pass the first stage

}
