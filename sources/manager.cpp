
#include "../headers/manager.h"

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

//funcion de terminal
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

//funcion para buscar comando
void manager::search(string tk,vector<string> tks){
    
    if (lower(tk) == "mkdisk"){
    Disk.mkdisk(tks);
    }else if (lower(tk) == "rmdisk"){
        Disk.rmdisk(tks);
    }else if (lower(tk) == "fdisk"){
        Disk.fdisk(tks);
    }else if (lower(tk) == "mount"){
        Disk.mount(tks);
    }else if (lower(tk) == "unmount"){
        Disk.unmount(tks);
    }else if (lower(tk) == "mkfs"){
        Disk.mkfs(tks);
    }else if (lower(tk) == "login"){
        if (!User.logged_in) User.login(tks, Disk);
        else cout << "Ya hay una sesion iniciada" << endl;
    }else if (lower(tk) == "logout"){
        if (User.logged_in) User.logout();
        else cout << "No hay ninguna sesion iniciada" << endl;
    }else if (lower(tk) == "mkgrp" || lower(tk) == "rmgrp" || lower(tk) == "mkusr" || lower(tk) == "rmusr"){
        if (User.logged_in){
            if (lower(tk) == "mkgrp") User.mkgrp(tks);
            else if (lower(tk) == "rmgrp") User.rmgrp(tks);
            else if (lower(tk) == "mkusr") User.mkusr(tks);
            else if (lower(tk) == "rmusr") User.rmusr(tks);
        }else{
            cout << "No hay ninguna sesion iniciada" << endl;
        }
    }else if (lower(tk) == "rep"){
        Disk.rep();
    }else if (lower(tk) == "execute"){
        execute(tks);
    }else if (lower(tk) == "pause"){
        pause();
    }else if (tk.substr(0,1) == "#"){
        cout << tk << endl;
    }else{
        cout << "Comando no encontrado" << endl;
    }

}

//funcion para ejecutar script
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

//funcion pause
void manager::pause(){
    cout << "Presione enter para continuar..." << endl;
    cin.get();
}

//funcion para extraer token
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

//funcion para separar tokens
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

//funcion para convertir a minusculas
string manager::lower(string String){
    string aux;
    locale loc;
    for (int i = 0; i < String.length(); i++){
        aux += tolower(String[i], loc);
    }
    return aux;
}