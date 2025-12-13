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
  cout<<input<<": command not found"<<endl;
  main();

  // TODO: Uncomment the code below to pass the first stage

}
