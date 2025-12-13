#include "functions.h"

bool is_exec(string & path){
  const char * t=path.c_str();
  return access(t, X_OK)==0;
}

void run(string& path,const vector<string>& commands){
  pid_t pid = fork();
  vector<char* >argv;
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



vector<string> split_by_spaces(const string& str){
    bool singleq=false;
    bool doubleq=false;
    string word;
    vector<string> words;
    for(int i=0;i<str.size();i++){
        char ch=str[i];
        if((doubleq&&ch!='\"')||(singleq&&ch!='\'')){
            word.push_back(ch);
            continue;
        }
        if(!doubleq&&!singleq){
            if(ch=='\''){
                singleq=true;
                continue;
            }
            if(ch=='\"'){
                doubleq=true;
                continue;
            }
            if(ch==' '){
                if(word.size()){
                    words.push_back(word);
                    word="";
                }
            }
            else{
                word.push_back(ch);
            }
            continue;
        }
        singleq=doubleq=false;
    }
    if(word.size()){
        words.push_back(word);
    }
    // for(auto word:words){
    //     cout<<word<<" ";
    // }
    // cout<<endl;
  return words;
}