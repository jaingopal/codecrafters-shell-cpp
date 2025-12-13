#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;

vector<fs::path> exec_files;
vector<string>builtin={"type","exit","echo"};

bool if_exec(string & path){
  const char * t=path.c_str();
  return access(t, X_OK)==0;
}

//For regenerating the executable files from the PATH variable
void get_execFiles(){
  string str=fs::path(getenv("PATH"));
  string temp;
  exec_files={};
  for(auto ch:str){
    if(ch==':'){
      if(temp.size()){
        if(fs::exists(temp)&&if_exec(temp)){
          exec_files.push_back(temp);
        }
        temp="";
      }
    }
    else{
      temp.push_back(ch);
    }
  }
  if(temp.size()){
    if(fs::exists(temp)&&if_exec(temp)){
      exec_files.push_back(temp);
    }
  }
  return ;
}

void echo(string& str){
  cout<<str.substr(5)<<endl;
  return;
}

void invalid_type(string& command){
  cout<<command<<": not found"<<endl;
  return;
}

void type(string& str){
  string command=str.substr(5);
  for(auto& s:builtin){
    if(s==command){
      cout<<command<<" is a shell builtin"<<endl;
      return;
    }
  }
  if(!command.size()){
    invalid_type(command);
    return;
  }
  get_execFiles();
  for(auto t:exec_files){
    string s=t;
    if(s.size()<command.size()){
      continue;
    }
    if(s==command){
      cout<<command<<" is "<<s<<endl;
      return ;
    }
    if(s.substr(s.size()-command.size())==command){
      if(s[s.size()-command.size()-1]=='/'){
        cout<<command<<" is "<<s <<endl;
        return;
      }
    }

  }

  invalid_type(command);
  return;

}

int main() {

  cout << unitbuf;
  cerr << unitbuf;

  cout<<"$ ";

  string input ;
  getline(cin,input);
  if(input=="exit"){
    return 0;
  }
  if(input.substr(0,5)=="echo "){
    echo(input);
    main();
  }
  if(input.substr(0,5)=="type "){
    type(input);
    main();
  }

  cout<<input<<": command not found"<<endl;
  main();

}
