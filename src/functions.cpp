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
    int tab=0;
    history.push_back("");
    int his_ind=history.size()-1;
    while(1){
        ch=get_ch();
        if(ch == '\b' || ch == 127){        //backspace
            if(!input.size()){
                cout<<"\x07";   //ring bell 
            }
            else{
                cout<<"\b \b"<<flush;
                input.pop_back();
                history[his_ind]=input;
            }
            continue;
        }
        if (ch == '\033') {          
            char c1 = get_ch();      
            char c2 = get_ch();      

            if (c1 == '[' && c2 == 'A') {   //upward arrow
                his_ind--;
                if(his_ind<0){
                    his_ind=0;
                    cout<<"\x07";   //ring bell 
                    continue;
                }
                else{   
                    cout << "\r\033[2K";    //clear the output written yet 
                    cout<<"$ "<<history[his_ind];   //written the output 
                    input=history[his_ind];
                    continue;
                }
            }

            if(c1=='['&&c2=='B'){       //downward arrow 
                his_ind++;
                if(his_ind==history.size()){
                    his_ind=history.size()-1;
                    cout<<"\x07";   //ring bell 
                    continue;
                }
                else{
                    cout << "\r\033[2K";    //clear the output written yet 
                    cout<<"$ "<<history[his_ind];   //written the output 
                    input=history[his_ind];
                    continue;
                }
            }
        }

        if(ch=='\t'){
            vector<string>matching;
            match_exec(input,matching);
            sort(matching.begin(),matching.end());
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
                history[his_ind]=input;
                continue;
            }
            string st1=matching[0],st2=matching[matching.size()-1];
            int i=0,j=0;
            string common;
            while(i<st1.size()&&j<st2.size()){
                if(st1[i]==st2[j]){
                    common.push_back(st1[i]);
                }   
                else{
                    break;
                }
                i++;
                j++;
            }
            if(input!=common){
                for(i=input.size();i<common.size();i++){
                    cout<<common[i];
                }
                input=common;
                history[his_ind]=input;
                continue;
            }
            else{
                if(tab){
                    cout<<endl;
                    for(auto match:matching){
                        cout<<match<<"  ";
                    }
                    cout<<endl;
                    cout<<"$ "<<input;
                    tab=0;
                }
                else{
                    tab++;
                    cout<<"\x07";
                }
            }
            continue;
        }
        tab=0;
        cout<<ch;
        if(ch=='\n'){
            if(his_ind!=history.size()-1){
                history.pop_back();
                history.push_back(history[his_ind]);
            }
            else{
                if(!history[his_ind].size()){
                    history.pop_back();
                }
            }
            return ;
        }
        input.push_back(ch);
        history[his_ind].push_back(ch);
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

string run(string& path,
           const vector<string>& commands,
           string& input)
{
    //0->read end   1->write end 
    int inpipe[2];   // parent -> child
    int outpipe[2];  // child -> parent

    pipe(inpipe);
    pipe(outpipe);

    pid_t pid = fork();

    if (pid == 0) {
        // CHILD

        dup2(inpipe[0], STDIN_FILENO);      //reads from child read end
        dup2(outpipe[1], STDOUT_FILENO);    //writes to child write end 

        close(inpipe[1]);
        close(outpipe[0]);
        close(inpipe[0]);
        close(outpipe[1]);

        vector<char*> argv;
        for (const auto& s : commands)
            argv.push_back(const_cast<char*>(s.c_str()));
        argv.push_back(nullptr);

        execvp(path.c_str(), argv.data());
        _exit(1);
    }

    // PARENT
    close(inpipe[0]);   
    close(outpipe[1]);

    if (!input.empty()) {
        write(inpipe[1], input.c_str(), input.size());
    }
    close(inpipe[1]);

    string output;
    char buf[256];
    ssize_t n;
    while ((n = read(outpipe[0], buf, sizeof(buf))) > 0) {
        output.append(buf, n);
    }

    close(outpipe[0]);
    waitpid(pid, nullptr, 0);

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




string redirect(vector<string>& commands,string& filename,string& errorname,bool append_file,bool append_err){
    if(!errorname.size()){
        ofstream file;
        if(filename.size()){
            if(append_file){
                file.open(filename,ios::app);
            }
            else{
                file.open(filename,ios::out|ios::trunc);
            }
            if(!file.is_open()){
                return filename+" not opening \n";
            }
        }
        if(commands[0]=="exit"){
            load_history_file();
            exit(0);
        }
        else if(commands[0]=="echo"){
            string ans= echo(commands);
            if(filename.size()){
                file<<ans<<flush;
                return "";
            }
            return ans;
        }
        else if(commands[0]=="type"){
            string ans= type_main(commands);
            if(filename.size()){
                file<<ans<<flush;
                return "";
            }
            return ans;
        }

        else if(commands[0]=="pwd"){
            string ans= pwd();
            if(filename.size()){
                file<<ans<<flush;
                return "";
            }
            return ans;
        }
        else if(commands[0]=="cd"){
            string ans= cd_main(commands);
            if(filename.size()){
                file<<ans<<flush;
                return "";
            }
            return ans;
        }
        else if(commands[0]=="history"){
            string ans= history_fx(commands);
            if(filename.size()){
                file<<ans<<flush;
                return "";
            }
            return ans;
        }
        else{
            string in="";
            string ans= ext(commands,in);
            if(filename.size()){
                file<<ans<<flush;
                return "";
            }
            return ans;
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
            return filename+" not opening \n";
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
            return errorname+" not opening \n";
        }
    }
    if(commands[0]=="exit"){
        if(file.is_open()){
            file.flush();
        }
        if(error.is_open()){
            error.flush();
        }
        load_history_file();
        exit(0);
    }
    else if(commands[0]=="echo"){
        string ans;
        output out=echo_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            ans=out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            ans=ans+out.str;
        }
        return ans;
    }
    else if(commands[0]=="type"){
        string ans;
        output out=type_main_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            ans=out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            ans=ans+out.str;
        }
        return ans;
    }
    
    else if(commands[0]=="pwd"){
        string ans;
        output out=pwd_error();
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            ans=out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            ans=ans+out.str;
        }
        return ans ;
    }
    else if(commands[0]=="cd"){
        string ans;
        output out=cd_main_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            ans=out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            ans=ans+out.str;
        }
        return ans;
    }
    else if(commands[0]=="history"){
        string ans;
        output out = history_error(commands);
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            ans=out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            ans=ans+out.str;
        }
        return ans;
    }
    else{
        output out=ext_error(commands);
        string ans;
        if(error.is_open()){
            error<<out.error;
            error.flush();
        }
        else{
            ans=out.error;
        }
        if(file.is_open()){
            file<<out.str;
            file.flush();
        }
        else{
            ans=ans+out.str;
        }
        return ans;
    }
    return "";
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
        if(ch=='|'){
            if(word.size()){
                words.push_back(word);
                word="";
            }
            words.push_back("|");
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
void load_history(){
    const char* env=getenv("HISTFILE");
    string histfile;
    if(env==nullptr){
        histfile=string(getenv("HOME"))+"/.zsh_history";
    }
    else{
        histfile=string(env);
    }
    ifstream file (histfile);
    if(file.is_open()){
        string his;
        while(getline(file,his)){
            if(his.size()){
                history.push_back(his);
            }
        }
    }

}
void load_history_file(){
    const char* env=getenv("HISTFILE");
    string histfile;
    if(env==nullptr){
        histfile=string(getenv("HOME"))+"/.zsh_history";
    }
    else{
        histfile=string(env);
    }
    ofstream file (histfile);
    if(file.is_open()){
        for(auto his:history){
            file<<his<<endl;
        }
    }
}

void split_with_pipe(vector<string>& words,vector<vector<string>>& par){
    vector<string>vec;
    for(auto str:words){
        if(str=="|"){
            if(vec.size()){
                par.push_back(vec);
                vec={};
            }
        }
        else{
            vec.push_back(str);
        }
    }
    if(vec.size()){
        par.push_back(vec);
    }
    return;
}

string execute(vector<vector<string>>& partitions){
    string file="",err="";
    bool file_=0,err_=0;
    string ans=redirect(partitions[0],file,err,file_,err_);
    for(int i=1;i<partitions.size();i++){

        ans=ext(partitions[i],ans);
    }
    return ans;
}