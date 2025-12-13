#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;


string cwd="";
vector<fs::path> exec_folders;
vector<string>builtin={"type","exit","echo","pwd"};

bool is_exec(string & path){
  const char * t=path.c_str();
  return access(t, X_OK)==0;
}

void run(string& path,const vector<string>& commands){
  pid_t pid = fork();
  vector<char* >argv;
  // argv.push_back(const_cast<char*>(path.c_str()));
  for(const auto & s:commands){
    argv.push_back(const_cast<char*>(s.c_str()));
  }
  argv.push_back(nullptr);
  if(pid==0){
    execvp(path.c_str(),argv.data());
  }
  else{
    wait(nullptr);
  }
  return ;

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
      if(entry.path().filename()==command){
        return s;
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

void pwd(){
  if(!cwd.size()){
    fs::path curr=fs::current_path();
    cwd=curr;
  }
  cout<<cwd<<endl;
  return;
}

void parent_dir(){
  if(cwd=="/"){
    return;
  }
  for(int i=cwd.size()-1;i>=2;i--){
    if(cwd[i]=='/'){
      cwd=cwd.substr(0,i);
      return;
    }
  }
}

void cd(const string& direc){
  
  if(!direc.size()){
    return;
  }

  if(direc[0]=='/'){
    cwd='/';
    string direct=direct.substr(1);
    cd(direct);
    return;
  }

  if(direc.size()>=2&&direc[0]=='.'&&direc[1]=='.'){
    if(direc.size()==2){
      parent_dir();
      return;
    }
    if(direc[2]=='/'){
      parent_dir();
      for(int i=3;i<direc.size();i++){
        if(direc[i]!='/'){
          string direct=direc.substr(i);
          cd(direct);
          return;
        }
      }
      return;
    }
  }

  if(direc[0]=='.'){
    if(direc.size()==1){
      return ;
    }
    if(direc[1]=='/'){
      for(int i=2;i<direc.size();i++){
        if(direc[i]!='/'){
          string direct=direc.substr(i);
          cd(direct);
          return;
        }
      }
      return;
    }
  }
  
  string temp;
  for(int i=0;i<direc.size();i++){
    if(direc[i]=='/'){
      string new_cwd;
      if(cwd[cwd.size()-1]=='/'){
        new_cwd=cwd+temp;
      }
      else{
        new_cwd=cwd+'/'+temp;
      }
      fs::path directory=new_cwd;
      if(!(fs::exists(directory))||!(fs::is_directory(directory))){
        cout<<"cd: "<<cwd+direc<<": No such file or directory"<<endl;
        return;
      }
      cwd=new_cwd;
      temp="";
      while(i<direc.size()){
        if(direc[i]!='/'){
          string direct=direc.substr(i);
          cd(direct);
          return;
        }
      }
      return;
    }
    else{
      temp.push_back(direc[i]);
    }
  }
  
  string new_cwd;
  if(cwd[cwd.size()-1]=='/'){
    new_cwd=cwd+temp;
  }
  else{
    new_cwd=cwd+'/'+temp;
  }

  fs::path directory=new_cwd;
  if(fs::exists(directory)){
    if(fs::is_directory(directory)){
      cwd=new_cwd;
      return;
    }
    else{
      cout<<"cd: "<<new_cwd<<": not a directory"<<endl;
      return;
    }
  }
  else{
    cout<<"cd: "<<new_cwd<<" : No such file or directory"<<endl;
    return;
  }

}

void cd_main(vector<string>& commands){
  if(cwd==""){
    fs::path curr=fs::current_path();
    cwd=curr;
  }
  string HOME=getenv("HOME");
  if(commands.size()==1){
    cwd=HOME;
    return;
  }
  if(commands[1][0]=='~'){
    if(commands[1]=="~"){
      cwd=HOME;
      return;
    }
    else{
      if(commands[1][2]=='/'){
        string str=commands[1].substr(2);
        cwd=HOME;
        cd(str);
        return;
      }
    }
  }
  cd(commands[1]);
  return;
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
