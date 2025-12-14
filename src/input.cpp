#include "../header/globals.h"
#include "../header/functions.h"

void take_input(string& input){
    char ch;
    int tab=0;
    history.push_back("");
    int his_ind=history.size()-1;
    int ind_input=0;
    while(1){
        ch=get_ch();
        if(ch == '\b' || ch == 127){        //backspace
            if(!ind_input){
                cout<<"\x07";   //ring bell 
            }
            else{
                cout << "\r\033[2K";    //clear the output written yet 
                ind_input--;
                input.erase(input.begin()+ind_input);
                cout<<"$ "<<input;
                history[his_ind]=input;
                int count_back=input.size()-ind_input;
                while(count_back--){
                    cout << "\033[D";
                }
            }
            continue;
        }
        if (ch == '\033') {             //arrow
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
                    ind_input=input.size();
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
                    ind_input=input.size();
                    continue;
                }
            }
            if(c1=='['&&c2=='C'){       //right arrow
                if(ind_input==input.size()){
                    cout<<"\x07";   //ring bell 
                    continue;
                }
                else{
                    ind_input++;
                    cout << "\033[C";   //moe cursor ahead
                    continue;
                }
            }
            if(c1=='['&&c2=='D'){       //left arrow 
                if(ind_input==0){
                    cout<<"\x07";
                    continue;
                }
                else{
                    ind_input--;
                    cout << "\033[D";       //move cursor back 
                    continue;
                }
            }
        }

        if(ch=='\t'){
            vector<string>matching;
            match_exec(input,matching);
            sort(matching.begin(),matching.end());
            if(!matching.size()){
                cout<<"\x07";       //ring bell 
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
                ind_input=input.size();
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
                ind_input=input.size();
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
        if(ch=='\n'){
            cout<<ch;
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
        input.insert(input.begin()+ind_input,ch);
        history[his_ind]=input;
        ind_input++;
        cout << "\r\033[2K";    //clear the output written yet 
        cout<<"$ "<<input;
        int count_back=input.size()-ind_input;
        while(count_back--){
            cout << "\033[D";
        }
        cout.flush();
    }
    return ;
}