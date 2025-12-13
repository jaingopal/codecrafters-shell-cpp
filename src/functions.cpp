#include "functions.h"
#include "commands.h"

bool is_exec(string & path){
  const char * t=path.c_str();
  return access(t, X_OK)==0;
}

//uppdate this run by piping 

string run(string& path,const vector<string>& commands){
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if(pid==0){
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        vector<char* >argv;
        for(const auto & s:commands){
          argv.push_back(const_cast<char*>(s.c_str()));
        }
        argv.push_back(nullptr);
        execvp(path.c_str(),argv.data());
    }
    
    close(pipefd[1]);

    string output;
    char buf[256];
    ssize_t n;

    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        output.append(buf, n);
    }

    close(pipefd[0]);
    wait(nullptr);
    return output;

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


string invalid_type(string& command){
    return command+": not found\n";
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



void split_by_spaces(const string& str,vector<string>& words,string& filename){
    bool singleq=false;
    bool doubleq=false;
    string word;
    for(int i=0;i<str.size();i++){
        char ch=str[i];
        if((doubleq&&ch!='\"')||(singleq&&ch!='\'')){
            if(doubleq&&ch=='\\'){
                if(str[i+1]=='\"'||str[i+1]=='\\'){
                    word.push_back(str[i+1]);
                    i++;
                    continue;
                }
            }
            word.push_back(ch);
            continue;
        }
        if(ch=='\\'){
            word.push_back(str[i+1]);
            i++;
            continue;
        }
        if(ch=='>'){
            if(word.size()){
                if(word[word.size()-1]=='1'){
                    word.pop_back();
                }
            }
            if(word.size()){
                words.push_back(word);
                word="";
            }
            words.push_back(">");
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
    vector<string> temp;
    for(int i=0;i<words.size();i++){

        if(words[i]==">"){

            filename=words[i+1];
            i++;
        }
        else{
            temp.push_back(words[i]);

        }
    }
    words=temp;
}

void redirect(vector<string>& commands,string& filename){
    ofstream file(filename,ios::out|ios::trunc);
    if(!file.is_open()){
        cout<<"NOT OPENING "<<endl;
        return;
    }
    if(commands[0]=="exit"){
        file.flush();
        exit(0);
    }
    else if(commands[0]=="echo"){
        file<<echo(commands);
        file.flush();
        main();
    }
    else if(commands[0]=="type"){
        file<<type_main(commands);
        file.flush();
        main();
    }

    else if(commands[0]=="pwd"){
        file<<pwd();
        file.flush();
        main();
    }
    else if(commands[0]=="cd"){
        file<<cd_main(commands);
        file.flush();
        main();
    }
    else{
        file<<ext(commands);
        file.flush();
        main();
    }
}