#include "functions.h"
#include "errors_functions.h"

output echo_error(vector<string >& vec){
    string ans;
    for(int i=1;i<vec.size();i++){
        ans=ans+vec[i]+" ";
    }
    ans+="\n";
    output ret;
    ret.str=ans;
    return ret;
}

output ext_error(vector<string>& commands){
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
    return run_errors(exec,commands);
  }
  else{
    ans=commands[0]+": command not found"+"\n";
    output ret;
    ret.error=ans;
    return ret;
  }
}

output pwd_error(){
    string ans;
    if(!cwd.size()){
        fs::path curr=fs::current_path();
        cwd=curr;
    }
    ans=cwd+"\n";
    output ret;
    ret.str=ans;
    return ret;
}

output cd_error(const string& direc){
    string ans;
    if(!direc.size()){
        output ret;
        return ret;
    }

    if(direc[0]=='/'){
        cwd='/';
        string direct=direc.substr(1);
        return cd_error(direct);
        
    }

    if(direc.size()>=2&&direc[0]=='.'&&direc[1]=='.'){
        if(direc.size()==2){
            parent_dir();
            output ret;
            return ret;
        }
        if(direc[2]=='/'){
        parent_dir();
        for(int i=3;i<direc.size();i++){
            if(direc[i]!='/'){
                string direct=direc.substr(i);
                return cd_error(direct);
            }
        }
        output ret;
        return ret;
        }
    }

    if(direc[0]=='.'){
        if(direc.size()==1){
            output ret;
            return ret;
        }
        if(direc[1]=='/'){
            for(int i=2;i<direc.size();i++){
                if(direc[i]!='/'){
                string direct=direc.substr(i);
                return cd_error(direct);
                }
            }
            output ret;
            return ret;
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
            output ret;
            ret.error=ans;
            return ret;
        }
        cwd=new_cwd;
        temp="";
        while(i<direc.size()){
            if(direc[i]!='/'){
            string direct=direc.substr(i);
            return cd_error(direct);
            }
            i++;
        }
        output ret;
        return ret;
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
            output ret;
            return ret;
        } 
        else{
            ans="cd: "+new_cwd+": not a directory\n";
            output ret;
            ret.error=ans;
            return ret;
        }
    }
    else{
        ans="cd: "+new_cwd+": No such file or directory\n";
        output ret;
        ret.error=ans;
        return ret;
    }
}

output cd_main_error(vector<string>& commands){
  if(cwd==""){
    fs::path curr=fs::current_path();
    cwd=curr;
  }
  string HOME=getenv("HOME");
  if(commands.size()==1){
    cwd=HOME;
    chdir(cwd.c_str());
    output ret;
    return ret;
  }
  if(commands[1][0]=='~'){
    if(commands[1]=="~"){
        cwd=HOME;
        chdir(cwd.c_str());
        output ret;
        return ret;
    }
    else{
      if(commands[1][2]=='/'){
        string str=commands[1].substr(2);
        cwd=HOME;
        output ans=cd_error(str);
        chdir(cwd.c_str());
        return ans;
      }
    }
  }
  output ans= cd_error(commands[1]);
  chdir(cwd.c_str());
  return ans;
}

output type_error(string& command){
  for(auto& s:builtin){
    if(s==command){
        output ret;
        ret.str= command+" is a shell builtin\n";
        return ret;
    }
  }
  if(!command.size()){
    output ret;
    return ret;
  }
  get_execFiles();
  for(auto t:exec_folders){
    string folder=t;
    string ret=check_type(command,folder);
    if(ret.size()){
      output retur;
      retur.str= command+" is "+ret+"\n";
      return retur;
    }
  }

  output ret;
  ret.error= invalid_type(command);
  return ret;
}

output type_main_error(vector<string>& commands){
  if(commands.size()==1){
    output ret;
    return ret;
  }
  output ans;
  for(int i=1;i<commands.size();i++){
    output temp=type_error(commands[i]);
    ans.error=ans.error+temp.error;
    ans.str=ans.str+temp.str;
  }
  return ans;
}

output history_error(vector<string>& commands){
  output ret;
    if(commands.size()==1){
      for(int i=0;i<history.size();i++){
          ret.str=ret.str+"    "+to_string(i+1)+"  "+history[i]+"\n";
      }
      return ret;
    }
    else{
      string str=commands[1];
      if(str=="-r"){
        if(commands.size()>2){
          ifstream file(commands[2]);
          if(file.is_open()){
            string his;
            file>>his;
            if(his.size()){
              history.push_back(his);
            }
          }
        }
        return ret;
      }
      int n=0;
      for(auto ch:str){
        if(ch>='0'&&ch<='9'){
          n=n*10+ch-'0';
        }
        else{
          ret.error= "history: "+str+": numeric argument required\n";
          return ret;
        }
      }
      if(n>history.size()){
        n=history.size();
        for(int i=history.size()-n;i<=history.size();i++){
          ret.str=ret.str+history[i];
        }
      }

    }
  return ret;
}