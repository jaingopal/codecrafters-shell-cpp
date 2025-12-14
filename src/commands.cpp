#include "functions.h"
#include "globals.h"



string echo(vector<string >& vec){
    string ans;
    for(int i=1;i<vec.size();i++){
        ans=ans+vec[i]+" ";
    }
    ans+="\n";
    return ans;
}


string ext(vector<string>& commands){
  get_execFiles();
  string ans;
  string exec="";
  for(auto t:exec_folders){
    string folder=t;
    exec=check_type(commands[0],folder);
    if(exec.size()){
      break;
    }
  }
  if(exec.size()){
    ans=run(exec,commands);
    return ans;
  }
  else{
    ans=commands[0]+": command not found"+"\n";
    return ans;
  }
}



string pwd(){
    string ans;
    if(!cwd.size()){
        fs::path curr=fs::current_path();
        cwd=curr;
    }
    ans=cwd+"\n";
    return ans;
}


string cd(const string& direc){
    string ans;
    if(!direc.size()){
        return "";
    }

    if(direc[0]=='/'){
        cwd='/';
        string direct=direc.substr(1);
        return cd(direct);
        
    }

    if(direc.size()>=2&&direc[0]=='.'&&direc[1]=='.'){
        if(direc.size()==2){
            parent_dir();
            return "";
        }
        if(direc[2]=='/'){
        parent_dir();
        for(int i=3;i<direc.size();i++){
            if(direc[i]!='/'){
                string direct=direc.substr(i);
                return cd(direct);
            }
        }
        return "";
        }
    }

    if(direc[0]=='.'){
        if(direc.size()==1){
            return "";
        }
        if(direc[1]=='/'){
            for(int i=2;i<direc.size();i++){
                if(direc[i]!='/'){
                string direct=direc.substr(i);
                return cd(direct);
                }
            }
            return "";
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
            ans="cd: "+new_cwd+'/'+direc.substr(i)+": No such file or directory\n";
            return ans;
        }
        cwd=new_cwd;
        temp="";
        while(i<direc.size()){
            if(direc[i]!='/'){
            string direct=direc.substr(i);
            return cd(direct);
            }
            i++;
        }
        return "";
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
            return "";
        } 
        else{
            ans="cd: "+new_cwd+": not a directory\n";
            return ans;
        }
    }
    else{
        ans="cd: "+new_cwd+": No such file or directory\n";
        return ans;
    }

}

string cd_main(vector<string>& commands){
  if(cwd==""){
    fs::path curr=fs::current_path();
    cwd=curr;
  }
  string HOME=getenv("HOME");
  if(commands.size()==1){
    cwd=HOME;
    chdir(cwd.c_str());
    return "";
  }
  if(commands[1][0]=='~'){
    if(commands[1]=="~"){
        cwd=HOME;
        chdir(cwd.c_str());
        return "";
    }
    else{
      if(commands[1][2]=='/'){
        string str=commands[1].substr(2);
        cwd=HOME;
        string ans=cd(str);
        chdir(cwd.c_str());
        return ans;
      }
    }
  }
  string ans= cd(commands[1]);
  chdir(cwd.c_str());
  return ans;
}

string type(string& command){
  for(auto& s:builtin){
    if(s==command){
        return command+" is a shell builtin\n";
    }
  }
  if(!command.size()){
    return "";
  }
  get_execFiles();
  for(auto t:exec_folders){
    string folder=t;
    string ret=check_type(command,folder);
    if(ret.size()){
      return command+" is "+ret+"\n";
    }
  }

  return invalid_type(command);

}

string type_main(vector<string>& commands){
  if(commands.size()==1){
    return "";
  }
  string ans;
  for(int i=1;i<commands.size();i++){
    ans=ans+type(commands[i]);
  }
  return ans;
}

string history_fx(int n){
    string ans;
    if(n>history.size()){
      n=history.size();
    }
    for(int i=history.size()-n;i<history.size();i++){
        ans=ans+"    "+to_string(i+1)+"  "+history[i]+"\n";
    }
    return ans;
}
