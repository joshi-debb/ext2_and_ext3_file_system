

#include "../headers/manager.h"
#include "../headers/disk.h"

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>


using namespace std;

disk Disk;

void manager::cmd(){

    do {
        system("clear");
        string txt;

        cout << "Terminal:~$ ";
        getline(cin, txt);

        string tk = token(txt);
        vector<string> tks = split_tokens(txt);

        search(tk,tks);

        getline(cin, txt);
    } while (true);

}

void manager::search(string tk,vector<string> tks){
    
    if (tk == "mkdisk"){
        Disk.mkdisk(tks);
    }
    else if (tk== "fdisk"){
        Disk.fdisk(tks);
    }
    else if (tk== "rep"){
        Disk.rep();
    }else if (tk== "execute"){
        execute(tks);
    }else if (tk.substr(0,1) == "#"){
        cout << tk << endl;
    }else{
        cout << "Comando no encontrado" << endl;
    }

}

void manager::execute(vector<string> tks){
    string txt;
    for (string token : tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length() + 1);
        if (tk == "path"){
            txt = token;
        }
    }

    string filename(txt);
    vector <string> lines;
    string line;
    ifstream input_file(filename);
    while(getline(input_file,line)){
        lines.push_back(line);
    }
    for(const auto &i:lines){
        string txt = i;
        string tk = token(txt);
        if(txt!=""){
            txt.erase(0,tk.length()+1);
            vector <string> tks = split_tokens(txt);
            search(tk,tks);
        }
    }
    input_file.close();
}


string manager::lower(string String){
    string aux;
    locale loc;
    for (int i = 0; i < String.length(); i++){
        aux += tolower(String[i], loc);
    }
    return aux;
}


string manager::token(string txt){
    string tkn = "";
    bool flag = false;
    for (char &caracter : txt){
        if (flag){
            if(caracter == ' ' || caracter == '>'){
                break;
            }
            tkn += caracter;
        }else if ((caracter != ' ' && !flag)){
            if (caracter == '#'){
                tkn=txt;
                break;
            }else{
                tkn += caracter;
                flag = true;
            }
        }
    }return tkn;
}

vector<string> manager::split_tokens(string txt){
    vector<string> tokens;
    txt.push_back(' ');
    string token = "";
    int state = 0;
    for(char &caracter: txt){
        if (state == 0 && caracter == '>'){
            state = 1;
        }else if(state == 0 && caracter == '#'){
            continue;
        }else if(state != 0){
            if (state == 1){
                if(caracter == '='){
                    state = 2;
                }else if(caracter == ' '){
                    continue;
                }
            }else if(state == 2){
                if (caracter == '\"'){
                    state = 3;
                }else{
                    state = 4;
                }
            }else if(state == 3){
                if (caracter == '\"'){
                    state = 4;
                }
            }else if (state == 4 && caracter == '\"'){
                tokens.clear();
                continue;
            }else if (state == 4 && caracter == ' '){
                state = 0;
                tokens.push_back(token);
                token = "";
                continue;
            }
            token += caracter;
        }
    }
    return tokens;
}
