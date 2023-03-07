
#include "../headers/user.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>


using namespace std;

//funcion para iniciar sesion
void user::login(vector<string> tks, disk Dsk) {
    string id="";
    string usuario="";
    string password="";
    string path;
    Disk = Dsk;
    
    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "id"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                id = token.substr(1, token.length() - 2);
            }else{
                id = token;
            }
        }else if (lower(tk) == "pass"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                password = token.substr(1, token.length() - 2);
            }else{
                password = token;
            }
        }else if (lower(tk) == "user"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                usuario = token.substr(1, token.length() - 2);
            }else{
                usuario = token;
            }
        }
    }

    try {

        Partition partition = Disk.get_mount(id, &path);

        FILE *rfile = fopen(path.c_str(), "rb");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, rfile);
        fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
        fread(&fb, sizeof(Fileblock), 1, rfile);
        fclose(rfile);

        string txt;
        txt += fb.b_content;
        int count = 0;
        vector<string> vctr = get_data(txt, '\n');
        for (string line:vctr) {
            if (toupper(line[2]) == 'U' && line[0] != '0') {
                vector<string> in = get_data(line, ',');
                if (lower(in[3]) == usuario && lower(in[4]) == password) {
                    active_user.id = id;
                    active_user.user = usuario;
                    active_user.password = password;
                    active_user.uid = stoi(in[0]);
                    logged_in = true;
                    cout << "logueado correctamente" << endl;
                    return;
                }
            }
        }
        throw runtime_error("Usuario o contraseña incorrectos");
    }

    catch (exception &e) {
        cout << e.what() << endl;
        logged_in = false;
        return;
    }
}

//funcion para cerrar sesion
void user::logout() {
    cout << "deslogueado correctamente" << endl;
    logged_in = false;
}

//funcion para crear grupo
void user::mkgrp(vector<string> tks) {

    string name;

    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "name"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                name = token.substr(1, token.length() - 2);
            }else{
                name = token;
            }
        }
    }

    if (tks.empty()) {
        show_datas_txt();
        return;
    }

    try {
        if (lower(active_user.user) != "root") {
            throw runtime_error("permiso denegado ...solo para usuario root");
        }

        string path;
        Partition partition = Disk.get_mount(active_user.id, &path);

        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, rfile);
        fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
        fread(&fb, sizeof(Fileblock), 1, rfile);

        string txt;
        txt += fb.b_content;
        vector<string> vctr = get_data(txt, '\n');
        int aux = 0;
        for (string line:vctr) {
            if (toupper(line[2]) == 'G') {
                aux++;
                vector<string> in = get_data(line, ',');
                if (in[2] == name) {
                    throw runtime_error("el grupo ya existe");
                }
            }
        }
        txt += to_string(aux + 1) + ",G," + name + "\n";
        strcpy(fb.b_content, txt.c_str());
        fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
        fwrite(&fb, sizeof(Fileblock), 1, rfile);
        fclose(rfile);
        cout << "grupo creado correctamente" << endl;

    }
    catch (exception &e) {
        cout << e.what() << endl;
    }

}

//funcion para eliminar grupo
void user::rmgrp(vector<string> tks) {

    string name;

    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "name"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                name = token.substr(1, token.length() - 2);
            }else{
                name = token;
            }
        }
    }

    try {
        if (lower(active_user.user) != "root") {
            throw runtime_error("permiso denegado ...solo para usuario root");
        }

        string path;
        Partition partition = Disk.get_mount(active_user.id, &path);

        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, rfile);
        fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
        fread(&fb, sizeof(Fileblock), 1, rfile);

        
        if (group_exists(name)) {
            string txt;
            txt += fb.b_content;
            vector<string> vctr = get_data(txt, '\n');
            txt = "";
            for (string line:vctr) {
                if (toupper(line[2]) == 'G'&& line[0] != '0') {
                    vector<string> in = get_data(line, ',');
                    if (in[2] == name) {
                        txt += to_string(0) + ",G," + in[2] + "\n";
                        continue;
                    }
                }
                txt += line + "\n";
            }
            strcpy(fb.b_content, txt.c_str());
            fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
            fwrite(&fb, sizeof(Fileblock), 1, rfile);
            cout << "grupo eliminado correctamente" << endl;

        }else {
            throw runtime_error("el grupo no existe");
        }

        fclose(rfile);
    
    }
    catch (exception &e) {
        cout << e.what() << endl;
    }

}

//funcion para crear usuario
void user::mkusr(vector<string> tks) {

    string user;
    string pwd;
    string grp;

    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "user"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                user = token.substr(1, token.length() - 2);
            }else{
                user = token;
            }
        }else if (lower(tk) == "pass"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                pwd = token.substr(1, token.length() - 2);
            }else{
                pwd = token;
            }
        }else if (lower(tk) == "grp"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                grp = token.substr(1, token.length() - 2);
            }else{
                grp = token;
            }
        }
    }

    

    try {

        if (lower(active_user.user) != "root") {
            throw runtime_error("permiso denegado ...solo para usuario root");
        }

        string path;
        Partition partition = Disk.get_mount(active_user.id, &path);

        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, rfile);
        fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
        fread(&fb, sizeof(Fileblock), 1, rfile);

        if(group_exists(grp)){
            string txt;
            txt += fb.b_content;
            vector<string> vctr = get_data(txt, '\n');
            int aux = 0;
            for (string line:vctr) {
                if (toupper(line[2]) == 'U' && line[0] != '0') {
                    aux++;
                    vector<string> in = get_data(line, ',');
                    if (in[3] == user) { 
                        throw runtime_error("el usuario ya existe");
                    }
                }
            }
            txt += to_string(aux + 1) + ",U," + grp + "," + user + "," + pwd + "\n";
            strcpy(fb.b_content, txt.c_str());
            fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
            fwrite(&fb, sizeof(Fileblock), 1, rfile);
            fclose(rfile);
            cout << "usuario creado correctamente" << endl;

        }else{
            throw runtime_error("el grupo no existe");
        }
    }
    catch (exception &e) {
        cout << e.what() << endl;
    }
}

//funcion para crear usuario
void user::rmusr(vector<string> tks) {

    string user;

    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "user"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                user = token.substr(1, token.length() - 2);
            }else{
                user = token;
            }
        }
    }

    try {

        if (lower(active_user.user) != "root") {
            throw runtime_error("permiso denegado ...solo para usuario root");
        }

        string path;
        Partition partition = Disk.get_mount(active_user.id, &path);

        FILE *rfile = fopen(path.c_str(), "rb+");
        fseek(rfile, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, rfile);
        fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
        fread(&fb, sizeof(Fileblock), 1, rfile);

        if (user_exists(user)) {
            string txt;
            txt += fb.b_content;
            vector<string> vctr = get_data(txt, '\n');
            txt = "";
            for (string line : vctr) {
                if (toupper(line[2]) == 'U' && line[0] != '0') {
                    vector<string> lines = get_data(line, ',');
                    if (lines[3] == user) {
                        txt += "0,U," + lines[2] + "," + lines[3] + "," + lines[4] + "\n";
                        continue;
                    }
                }
                txt += line + "\n";
            }

            strcpy(fb.b_content, txt.c_str());
            fseek(rfile, super.s_block_start + sizeof(Folderblock), SEEK_SET);
            fwrite(&fb, sizeof(Fileblock), 1, rfile);
            fclose(rfile);
            cout << "usuario eliminado correctamente" << endl;
            
        }else{
            throw runtime_error("el usuario no existe");
        }

        

    }
    catch (exception &e) {
        cout << e.what() << endl;
    }
}

//funcion para imprimir el archivo de datos txt
void user::show_datas_txt(){

    vector<string> lines = get_data(fb.b_content, '\n');
    for (string line : lines) {
        vector<string> data = get_data(line, ',');
        for (string element : data) {
            if (!element.empty()) { 
                cout << element << ", ";
            }
        }
        cout << endl; 
    }
}

//funcion para extraer los datos del txt de usuarios
vector<string> user::get_data(string txt, char caracter) {
    vector<string> datas;
    istringstream iss(txt);
    string line;

    while (getline(iss, line, caracter)) {
        datas.push_back(line);
    }

    return datas;
}

//funcion para convertir a minusculas
string user::lower(string String){
    string aux;
    locale loc;
    for (int i = 0; i < String.length(); i++){
        aux += tolower(String[i], loc);
    }
    return aux;
}

//funcion para buscar usuario
bool user::user_exists(string usr) {

    vector<string> vctr = get_data(fb.b_content, '\n');
    for (string line:vctr) {
        if (toupper(line[2]) == 'U' && line[0] != '0') {
            vector<string> in = get_data(line, ',');
            if (in[3] == usr) {
                return true;
            }
        }
    }
    return false;
}

//funcion pra buscar grupo
bool user::group_exists(string grp) {

    vector<string> vctr = get_data(fb.b_content, '\n');
    for (string line:vctr) {
        if (toupper(line[2]) == 'G' && line[0] != '0') {
            vector<string> in = get_data(line, ',');
            if (in[2] == grp) {
                return true;
            }
        }
    }
    return false;
}