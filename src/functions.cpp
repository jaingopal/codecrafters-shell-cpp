#include "functions.h"
#include "commands.h"
#include "error_commands.h"
#include <termios.h>

char get_ch(){
    termios oldt,newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt=oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}


void match_exec(const string& str,vector<string>& matching){
    
    for(auto fn:builtin){
        if(fn.size()<str.size()){
            continue;
        }
        else{
            if(fn.substr(0,str.size())==str){
                matching.push_back(fn);
            }
        }
    }

    get_execFiles();
    for(auto folder: exec_folders){
        for(const auto& entry:fs::directory_iterator(folder)){

            string entry_path=entry.path().string();
            string entry_name=entry.path().filename().string();
            if(!is_exec(entry_path)){
                continue;
            }
            if(entry_name.size()<str.size()){
                continue;
            }
            if(entry_name.substr(0,str.size())==str){
                bool check=true;
                for(auto builtin_fn:builtin){
                    if(builtin_fn==entry_name){
                        check=false;
                    }
                }
                if(check){
                    matching.push_back(entry_name);
                }
            }
        }
    }

    return ;

}

bool is_exec(string & path){
  const char * t=path.c_str();
  return access(t, X_OK)==0;
}

void take_input(string& input){
    char ch;
    while(1){
        ch=get_ch();
        if(ch=='\t'){
            vector<string>matching;
            match_exec(input,matching);
            if(!matching.size()){
                cout<<"\x07";
                continue;
            }
            if(matching.size()==1){
                for(int i=input.size();i<matching[0].size();i++){
                    cout<<matching[0][i];
                }
                cout<<" ";
                input=matching[0];
                input.push_back(' ');
            }
            else{
                cout<<endl;
                for(auto match:matching){
                    cout<<match<<endl;
                }
                cout<<"$ "<<input;
            }
            continue;
        }
        cout<<ch;
        if(ch==' '){
            if(!input.size()){
                continue;
            }
        }
        if(ch=='\n'){
            return ;
        }
        input.push_back(ch);
    }
    return ;
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
    if(fs::exists(temp)&&is_exec(temp)&&fs::is_directory(temp)){
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




void redirect(vector<string>& commands,string& filename,string& errorname,bool append_file,bool append_err){
    if(!filename.size()&&!errorname.size()){
        if(commands[0]=="exit"){
            exit(0);
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
    ofstream file,error;
    if(filename.size()){
        if(append_file){
            file.open(filename,ios::app);
        }
        else{
            file.open(filename,ios::out|ios::trunc);
        }
        if(!file.is_open()){
            cout<<filename<<" not opening "<<endl;
            main();
        }
    }
    if(errorname.size()){
        if(append_err){
            error.open(errorname,ios::app);
        }
        else{
            error.open(errorname,ios::out|ios::trunc);
        }
        if(!error.is_open()){
            cout<<errorname<<" not opening "<<endl;
            main();
        }
    }
    if(commands[0]=="exit"){
        if(file.is_open()){
            file.flush();
        }
        if(error.is_open()){
            error.flush();
        }
        exit(0);
    }
    else if(commands[0]=="echo"){
        
        output out=echo_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            cout<<out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            cout<<out.str;
        }
        main();
    }
    else if(commands[0]=="type"){
        output out=type_main_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            cout<<out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            cout<<out.str;
        }
        main();
    }
    
    else if(commands[0]=="pwd"){
        output out=pwd_error();
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            cout<<out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            cout<<out.str;
        }
        main();
    }
    else if(commands[0]=="cd"){
        output out=cd_main_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            cout<<out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            cout<<out.str;
        }
        main();
    }
    else{
        output out=ext_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            cout<<out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            cout<<out.str;
        }
        main();
    }
}


void split_by_spaces(const string& str,vector<string>& words,string& filename,string& errorfile,bool& append_file,bool& append_error){
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
            int val=0;
            if(word.size()){
                if(word[word.size()-1]=='1'){
                    word.pop_back();
                }
                else if(word[word.size()-1]=='2'){
                    val=2;
                    word.pop_back();
                }
            }
            if(word.size()){
                words.push_back(word);
                word="";
            }
            if(val==2){
                if(str[i+1]=='>'){
                    i++;
                    words.push_back("2>>");
                }
                else{
                    words.push_back("2>");
                }
            }
            else{
                if(str[i+1]=='>'){
                    i++;
                    words.push_back("1>>");
                }
                else{
                    words.push_back("1>");
                }
            }
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

        if(words[i]=="1>"){
            if(filename.size()){
                if(append_file){
                    ofstream file(filename,ios::app);
                }
                else{
                    ofstream file(filename,ios::out|ios::trunc);
                    file <<"";
                    file.flush();
                }
            }
            append_file=false;
            filename=words[i+1];
            i++;
        }
        else if(words[i]=="1>>"){
            if(filename.size()){
                if(append_file){
                    ofstream file(filename,ios::app);
                }
                else{
                    ofstream file(filename,ios::out|ios::trunc);
                    file <<"";
                    file.flush();
                }
            }
            append_file=true;
            filename=words[i+1];
            i++;
        }
        else if(words[i]=="2>"){
            if(errorfile.size()){
                if(append_error){
                    ofstream file(errorfile,ios::app);
                }
                else{
                    ofstream file(errorfile,ios::out|ios::trunc);
                    file <<"";
                    file.flush();
                }
            }
            append_error=false;
            errorfile=words[i+1];
            i++;
        }
        else if(words[i]=="2>>"){
            if(errorfile.size()){
                if(append_error){
                    ofstream file(errorfile,ios::app);
                }
                else{
                    ofstream file(errorfile,ios::out|ios::trunc);
                    file <<"";
                    file.flush();
                }
            }
            append_error=true;
            errorfile=words[i+1];
            i++;
        }
        else{
            temp.push_back(words[i]);

        }
    }
    words=temp;
}