#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;

vector<fs::path> exec_folders;
vector<string>builtin={"type","exit","echo"};

bool is_exec(string & path){
  const char * t=path.c_str();
  return access(t, X_OK)==0;
}

void run(string& path,const vector<string>& commands){

  vector<char* >argv;
  // argv.push_back(const_cast<char*>(path.c_str()));
  for(const auto & s:commands){
    argv.push_back(const_cast<char*>(s.c_str()));
  }
  argv.push_back(nullptr);
  execvp(path.c_str(),argv.data());

}

//For regenerating the executable files from the PATH variable
void get_execFiles(){
  string str=fs::path(getenv("PATH"));
  string temp;
  exec_folders={};
  for(auto ch:str){
    if(ch==':'){
      if(temp.size()){
        if(fs::exists(temp)&&is_exec(temp)&&fs::is_directory(temp)){
          exec_folders.push_back(temp);
        }
        temp="";
      }
    }
    else{
      temp.push_back(ch);
    }
  }
  if(temp.size()){
    if(fs::exists(temp)&&is_exec(temp)){
      exec_folders.push_back(temp);
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

string check_type(string& command,string& folder){
  for(const auto& entry:fs::directory_iterator(folder)){
    string s=entry.path().string();
    if(is_exec(s)){
      if(s.size()<command.size()){
        continue;
      }
      if(s==command){
        return s;
      }
      if(s.substr(s.size()-command.size())==command){
        if(s[s.size()-command.size()-1]=='/'){
          return s;   
        }
      }
    }
  }
  return "";
}

void type(string& command){
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
  for(auto t:exec_folders){
    string folder=t;
    string ret=check_type(command,folder);
    if(ret.size()){
      cout<<command<<" is "<<ret<<endl;
      return;
    }
  }

  invalid_type(command);
  return;

}

void type_main(vector<string>& commands){
  if(commands.size()==1){
    return;
  }
  for(int i=1;i<commands.size();i++){
    type(commands[i]);
  }
}

vector<string> split_by_spaces(const string& str){
  istringstream iss(str);
  vector<string>words;
  string word;
  while(iss >> word){
    words.push_back(word);
  }
  return words;
}

void ext(vector<string>& commands){
  get_execFiles();
  string exec="";
  for(auto t:exec_folders){
    string folder=t;
    exec=check_type(commands[0],folder);
    if(exec.size()){
      break;
    }
  }
  // vector<string>args(commands.begin()+1,commands.end());
  if(exec.size()){
    run(exec,commands);
    return ;
  }
  else{
    cout<<commands[0]<<": command not found"<<endl;
    return ;
  }
}

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
  if(commands[0]=="input"){
    echo(input);
    main();
  }
  if(commands[0]=="type"){
    type_main(commands);
    main();
  }

  ext(commands);
  main();

}
