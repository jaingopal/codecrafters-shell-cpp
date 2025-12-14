#include "globals.h"

output run_errors(string& path,const vector<string>& commands){
    int out_pipe[2];   // for stdout
    int err_pipe[2];   // for stderr

    pipe(out_pipe);
    pipe(err_pipe);

    pid_t pid = fork();
    if(pid==0){
        // Redirect stdout → out_pipe
        dup2(out_pipe[1], STDOUT_FILENO);

        // Redirect stderr → err_pipe
        dup2(err_pipe[1], STDERR_FILENO);


        // Close all pipe fds (important)
        close(out_pipe[0]);
        close(out_pipe[1]);
        close(err_pipe[0]);
        close(err_pipe[1]);

        vector<char* >argv;
        for(const auto & s:commands){
          argv.push_back(const_cast<char*>(s.c_str()));
        }
        argv.push_back(nullptr);
        execvp(path.c_str(),argv.data());

    }
    
    close(out_pipe[1]);
    close(err_pipe[1]);


    string out;
    char buf[256];
    ssize_t n;

    while ((n = read(out_pipe[0], buf, sizeof(buf))) > 0) {
        out.append(buf, n);
    }

    string error;
    
    while ((n = read(err_pipe[0], buf, sizeof(buf))) > 0) {
        error.append(buf, n);
    }

    waitpid(pid, nullptr, 0);
    
    output ret;
    ret.str=out;
    ret.error=error;

    return ret;

}