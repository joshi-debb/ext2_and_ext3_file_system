
#include "../headers/manager.h"

#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstdio>

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
        make_report(tks);
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
    cout << endl;
    cout << "Presione enter para continuar..." << endl;
    cin.get();
}

//metodo para convertir fecha
string manager::get_date(time_t &date){
    struct tm *tm;
    tm = localtime(&date);
    char date_converted [20];
    strftime(date_converted, 20, "%Y/%m/%d %H:%M:%S", tm);
    return date_converted;
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

//funcion para preparar reporte
void manager::make_report(vector<string> tks){

    string id_mount;
    string name_typ;
    string rep_path;
    string dir_path;
    string bin_path;
    string file_dot;
    string file_txt;  
    
    //extraer parametros
    for (string token : tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length() + 1);
        if (lower(tk) == "id"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                id_mount = token.substr(1, token.length() - 2);
            }else{
                id_mount = token;
            }
        } else if (lower(tk) == "name"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                name_typ = token.substr(1, token.length() - 2);
            }else{
                name_typ = token;
            }
        }else if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                rep_path = token.substr(1, token.length() - 2);
            }else{
                rep_path = token;
            }

            //obtener el nombre del archivo .dot
            file_dot = rep_path.substr(0, rep_path.find('.'));
            file_dot += ".dot";

            //obtener el nombre del archivo .dot
            file_txt = rep_path.substr(0, rep_path.find('.'));
            file_txt += ".txt";

            //obtener ruta de carpetas
            dir_path = rep_path;
            for (int i = dir_path.length() - 1; i >= 0; i--){
                if (rep_path[i] == '/'){
                    dir_path = dir_path.substr(0, i);
                    break;
                }
            }
        }
    }

    try {

        int free_inodes = -1;
        int free_blocks = -1;

        Superblock super;
        Inodes inodes;
        Journaling journal;

        //verificar si existe el directorio
        DIR* dir = opendir(dir_path.c_str());
        if (dir) {
            closedir(dir);
        } else {
            string cmds = "mkdir -p \"" + dir_path + "\"";
            system(cmds.c_str());
        }

        //verificar si existe la particion
        Partition partition = Disk.get_mount(id_mount, &bin_path);

        //verificar si existe el disco
        MBR disco;
        FILE *file = fopen(bin_path.c_str(), "rb+");
        if (file == NULL) {
            throw runtime_error("disco no existente");
        }

        //leer superblock
        fseek(file, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, file);

        //leer inodos
        fseek(file, super.s_inode_start, SEEK_SET);
        fread(&inodes, sizeof(Inodes), 1, file);

        fseek(file, partition.part_start + sizeof(Superblock), SEEK_SET);
        fread(&journal, sizeof(Journaling), 1, file);

        char ch_inode = 'x';
        //econtrar inodos usados
        fseek(file, super.s_bm_inode_start, SEEK_SET);
        for (int i = 0; i < super.s_inodes_count; i++) {
            fread(&ch_inode, sizeof(ch_inode), 1, file);
            if (ch_inode == '0') {
                free_inodes = i;
                break;
            }
        }

        char ch_block = 'x';
        //encontrar bloques usados
        fseek(file, super.s_bm_block_start, SEEK_SET);
        for (int i = 0; i < super.s_blocks_count; i++) {
            fread(&ch_block, sizeof(ch_block), 1, file);
            if (ch_block == '0') {
                free_blocks = i;
                break;
            }
        }

        rewind(file);
        fread(&disco, sizeof(MBR), 1, file);

        //inicializar particiones
        Partition partitions[4];
        partitions[0] = disco.mbr_Partition_1;
        partitions[1] = disco.mbr_Partition_2;
        partitions[2] = disco.mbr_Partition_3;
        partitions[3] = disco.mbr_Partition_4;

        //verificar tipo de reporte
        if(lower(name_typ) == "mbr"){
            mbr_report(rep_path, bin_path, file_dot, disco, partitions);
        }else if(lower(name_typ) == "disk"){
            dsk_report(rep_path, bin_path, file_dot, disco, partitions);
        }else if(lower(name_typ) == "inode"){
            ino_report(rep_path, *file, file_dot, super, inodes, free_inodes);
        }else if(lower(name_typ) == "block"){
            blk_report(rep_path, *file, file_dot, super, inodes, free_inodes);
        }else if (lower(name_typ) == "bm_inode"){
            bmi_report(rep_path, *file, file_txt, super);
        }else if (lower(name_typ) == "bm_bloc"){
            bmb_report(rep_path, *file, file_txt, super);
        }else if (lower(name_typ) == "tree"){
            tre_report(rep_path, *file, file_dot, super, inodes, free_inodes);
        }else if (lower(name_typ) == "sb"){
            spr_report(rep_path, file_dot, super);
        }else if (lower(name_typ) == "journaling"){
            jrl_report(rep_path, *file, file_dot, super, journal, partition);
        }else if (lower(name_typ) == "file"){
            fle_report();
        }else if (lower(name_typ) == "ls"){
            lsf_report();
        }else{
            throw runtime_error("tipo de reporte no existente");
        }

        fclose(file);
    
    } catch (const std::exception& e) {
        cout << e.what() << endl;
    }
    
}

//reporte mbr
void manager::mbr_report(string rep_path, string bin_path, string file_dot, MBR disco, Partition partitions[4]){

    try{
        string strGrafica;
        strGrafica += "digraph G{ \n graph [ dpi = \"800\" ]; \n node [shape = plaintext]; \n mbr [label = < \n";
        strGrafica += "<table border='0' cellborder='1' cellspacing='0'>\n";
        strGrafica += "<tr><td colspan='2'> <b>MBR y EBR</b></td></tr>";
        strGrafica += "<tr><td>mbr_tamano</td><td><b>"+to_string(disco.mbr_tamano)+"</b></td></tr>";
        strGrafica += "<tr><td>mbr_fecha_creacion</td><td>"+ get_date(disco.mbr_fecha_creacion) +"</td></tr>";
        strGrafica += "<tr><td>mbr_disk_signature</td><td>"+to_string(disco.mbr_dsk_signature)+"</td></tr>";
        for (int i = 0; i < 4; ++i) {
            if (partitions[i].part_status == '1') {
                strGrafica += "<tr><td colspan='2'><b>Particion</b></td></tr>";
                strGrafica += "<tr><td>part_name</td><td><b>";
                strGrafica += partitions[i].part_name;
                strGrafica += "</b></td></tr>";
                strGrafica += "<tr><td>part_type</td><td><b>";
                strGrafica += partitions[i].part_type;
                strGrafica += "</b></td></tr>";
                strGrafica += "<tr><td>part_status</td><td>";
                strGrafica += partitions[i].part_status;
                strGrafica += "</td></tr>";
                strGrafica += "<tr><td>part_fit</td><td>";
                strGrafica += partitions[i].part_fit;
                strGrafica += "</td></tr>";
                strGrafica += "<tr><td>part_start</td><td>"+to_string(partitions[i].part_start)+"</td></tr>";
                strGrafica += "<tr><td>part_size</td><td>"+to_string(partitions[i].part_s)+"</td></tr>";
                if (partitions[i].part_type == 'E') {
                    vector<EBR> ebrs = Disk.get_type_logic(partitions[i], bin_path);
                    for (EBR ebr : ebrs) {
                        strGrafica += "<tr><td colspan='2'><b>Particion Logica</b></td></tr>";
                        strGrafica += "<tr><td>part_name</td><td><b>";
                        strGrafica += ebr.part_name;
                        strGrafica += "</b></td></tr>";
                        strGrafica += "<tr><td>part_status</td><td>";
                        strGrafica += ebr.part_status;
                        strGrafica += "</td></tr>";
                        strGrafica += "<tr><td>part_fit</td><td>";
                        strGrafica += ebr.part_fit;
                        strGrafica += "</td></tr>";
                        strGrafica += "<tr><td>part_next</td><td>"+to_string(ebr.part_next)+"</td></tr>";
                        strGrafica += "<tr><td>part_start</td><td>"+to_string(ebr.part_start)+"</td></tr>";
                        strGrafica += "<tr><td>part_size</td><td>"+to_string(ebr.part_s)+"</td></tr>";
                    }
                }
            }
        }

        strGrafica += "\n</table>\n>]; \n}";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    }catch(exception e){
        cout << e.what() << endl;
    }
    
}

//reporte disk
void manager::dsk_report(string rep_path, string bin_path, string file_dot, MBR disco, Partition partitions[4]){
   
    try{

        int size_logics = 0;
        int cont_logic = 0;

        string strGrafica;
        string strGrafica_aux;

        strGrafica += "digraph G{ \n graph [ dpi = \"800\" ]; \n node [shape = plaintext]; \n mbr [label = < \n ";
        strGrafica += "<table  cellpadding='20' border='0' cellborder='1' cellspacing='0'>\n";
        strGrafica += "<tr>\n";
        strGrafica += "<td rowspan='2' height='200'><b>MBR</b></td> \n";

        strGrafica_aux = "<tr>\n";
        for (int i = 0; i < 4; ++i) {
            if (partitions[i].part_status == '1' && partitions[i].part_type == 'E') {
                vector<EBR> ebrs = Disk.get_type_logic(partitions[i], bin_path);
                for (EBR ebr : ebrs) {
                    size_logics += ebr.part_s;
                    if(size_logics < partitions[i].part_s){
                        cont_logic += 2;
                        float porcentaje = (float) ebr.part_s / ((float) disco.mbr_tamano - (float) sizeof(MBR));
                        porcentaje = round(porcentaje * 10000.00F) / 100.00F;
                        strGrafica_aux += "<td><b>EBR</b></td> \n";
                        strGrafica_aux += "<td><b>Logica</b> <br/>" + to_string(porcentaje) + "% del disco</td>\n";
                    }
                }
                if(size_logics < partitions[i].part_s){
                    cont_logic += 1;
                    float porcentaje = ((float) (partitions[i].part_s - size_logics) / ((float) disco.mbr_tamano - (float) sizeof(MBR)));
                    porcentaje = round(porcentaje * 10000.00F) / 100.00F;
                    strGrafica_aux += "<td><b>Libre</b> <br/>" + to_string(porcentaje) + "% del disco</td>\n";
                }   
            }  
        }
        strGrafica_aux += "</tr>\n\n";

        //sacar total del peso de las particionces primarias y extendidas
        int size_primaries = 0;
        for (int i = 0; i < 4; ++i) {
            if (partitions[i].part_status == '1') {
                size_primaries += partitions[i].part_s;
            }
        }

        //sacar el porcentaje de las particiones primarias y extendidas
        for (int i = 0; i < 4; ++i) {
            if (partitions[i].part_status == '1' && partitions[i].part_type == 'E') {
                strGrafica += "<td colspan='" + to_string(cont_logic) + "'> <b>Extendida</b> </td> \n";
            }else if(partitions[i].part_status == '1' && partitions[i].part_type == 'P'){
                float porcentaje = ((float) partitions[i].part_s / ((float) disco.mbr_tamano - (float) sizeof(MBR)));
                porcentaje = round(porcentaje * 10000.00F) / 100.00F;
                strGrafica += "<td rowspan='2'> <b>Primaria</b> <br/>" + to_string(porcentaje) + "% del disco</td> \n";
            }
        }

        //sacar el porcentaje de espacio libre
        if(size_primaries < disco.mbr_tamano){
            int libre = disco.mbr_tamano - size_primaries;
            float resto = ((float) libre / ((float) disco.mbr_tamano - (float) sizeof(MBR)));
            resto = round(resto * 10000.00F) / 100.00F;
            strGrafica += "<td rowspan='2'> <b>Libre</b> <br/>" + to_string(resto) + "% del disco</td> \n";
        }
    
        strGrafica += "</tr>\n\n";
        strGrafica += strGrafica_aux;
        strGrafica += "</table>>];\n}\n";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    }catch(exception e){
        cout << e.what() << endl;
    }
}

//reporte inodos
void manager::ino_report(string rep_path, FILE &file, string file_dot,  Superblock super, Inodes inodes, int free_inodes){    
    try {
        
        string strGrafica;
        strGrafica += "digraph G{ \n rankdir=LR; \n graph [ dpi = \"800\" ]; \n node [shape = plaintext]; \n";

        for (int i = 0; i < free_inodes; ++i) {
            strGrafica += "inode" + to_string(i) + " [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
            strGrafica += "<tr><td colspan = '2' > <b> i-Nodo " + to_string(i) + " </b> </td></tr>\n";

            strGrafica += "<tr>\n <td><b>i_type</b></td> <td> <b>" + to_string(inodes.i_type) + "</b></td>\n </tr>\n";

            strGrafica += "<tr>\n <td>i_uid</td> <td>" + to_string(inodes.i_uid) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_gid</td> <td>" + to_string(inodes.i_gid) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_size</td> <td>" + to_string(inodes.i_s) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_atime</td> <td>" + get_date(inodes.i_atime) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_ctime</td> <td>" + get_date(inodes.i_ctime) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_mtime</td> <td>" + get_date(inodes.i_mtime) + "</td>\n </tr>\n";

            for (int j = 0; j < 15; ++j) {
                strGrafica += "<tr>\n <td>i_block_" + to_string(j+1) + "</td> <td>" + to_string(inodes.i_block[j]) + "</td>\n </tr>\n";
            }
            
            strGrafica += "<tr>\n <td><b>i_perm</b></td> <td><b>" + to_string(inodes.i_perm) + "</b></td>\n </tr>\n";
            strGrafica += "</table>>];\n";

            if (i != 0) {
                strGrafica += "inode" + to_string(i - 1) + "-> inode" + to_string(i) + "\n";
            }

            fseek(&file, super.s_inode_start + (sizeof(Inodes) * (i + 1)), SEEK_SET);
            fread(&inodes, sizeof(Inodes), 1, &file);
        }

        strGrafica += "\n\n}\n";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }


 }

//reporte bloque
void manager::blk_report(string rep_path, FILE &file, string file_dot,  Superblock super, Inodes inodes, int free_inodes){
   try {

        Folderblock fb;
        Fileblock flb;

        string strGrafica;
        strGrafica += "digraph G{ \n rankdir=LR; \n graph [ dpi = \"800\" ]; \n node [shape = plaintext]; \n";

        int last = -1;
        for (int i = 0; i < free_inodes; i++) {
            for (int j = 0; j < 15; j++) {
                if (inodes.i_block[j] != -1) {
                    if (j < 12) {
                        if (inodes.i_type == 1) {
                            fseek(&file, super.s_block_start + (sizeof(Fileblock) * inodes.i_block[j]),SEEK_SET);
                            fread(&flb, sizeof(Fileblock), 1, &file);
                            strGrafica += "block" + to_string(inodes.i_block[j]) + " [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
                            strGrafica += "<tr><td colspan = '2' ><b>Block " + to_string(inodes.i_block[j]) + "</b></td></tr>\n";
                            strGrafica += "<tr><td colspan = '2'>";
                            strGrafica += flb.b_content;
                            strGrafica += "</td></tr>\n</table>>];\n";
                            if (last != -1) {
                                strGrafica += "block" + to_string(last) + "-> block" + to_string(inodes.i_block[j]) + "\n";
                            }
                            last = inodes.i_block[j];
                        } else {
                            fseek(&file, super.s_block_start + (sizeof(Folderblock) * inodes.i_block[j]),SEEK_SET);
                            fread(&fb, sizeof(Fileblock), 1, &file);
                            strGrafica += "block" + to_string(inodes.i_block[j]) + "  [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
                            strGrafica += "<tr><td colspan = '2' ><b>Block " + to_string(inodes.i_block[j]) + "</b></td></tr>\n";
                            for (int k = 0; k < 4; k++) {
                                string aux;
                                if (fb.b_content[k].b_name[0] == '\0') {
                                    aux = " ";
                                } else {
                                    aux = fb.b_content[k].b_name;
                                }
                                strGrafica += "<tr>\n <td>" + aux + "</td>\n <td>" + to_string(fb.b_content[k].b_inodo) + "</td>\n </tr>\n";
                            }
                            strGrafica += "</table>>];\n";
                            if (last != -1) {
                                strGrafica += "block" + to_string(last) + "-> block" + to_string(inodes.i_block[j]) + "\n";
                            }
                            last = inodes.i_block[j];
                        }
                    }
                }
            }
            fseek(&file, super.s_inode_start + (sizeof(Inodes) * (i + 1)), SEEK_SET);
            fread(&inodes, sizeof(Inodes), 1, &file);
        }

        strGrafica += "\n\n}\n";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

//reporte bitmap inodos
void manager::bmi_report(string rep_path, FILE &file, string file_txt, Superblock super) {

    try {

        string content;
        char ch = 'x';
        int count = 0;
        fseek(&file, super.s_bm_inode_start, SEEK_SET);
        for (int i = 0; i < super.s_inodes_count; i++) {
            fread(&ch, sizeof(ch), 1, &file);
            if (count < 19) {
                content += ch;
                count++;
            } else {
                content += ch;
                content += "\n";
                count = 0;
            }
        }

        ofstream outfile(file_txt);
        outfile << content.c_str() << endl;
        outfile.close();
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }

}

//reporte bitmap bloques
void manager::bmb_report(string rep_path, FILE &file, string file_txt,  Superblock super) {

    try {

        string content;
        char ch = 'x';
        int count = 0;
        fseek(&file, super.s_bm_block_start, SEEK_SET);
        for (int i = 0; i < super.s_blocks_count; i++) {
            fread(&ch, sizeof(ch), 1, &file);
            if (count < 19) {
                content += ch;
                count++;
            } else {
                content += ch;
                content += "\n";
                count = 0;
            }
        }
        ofstream outfile(file_txt);
        outfile << content.c_str() << endl;
        outfile.close();
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }

}

//reporte tree
void manager::tre_report(string rep_path, FILE &file, string file_dot,  Superblock super, Inodes inodes, int free_inodes) {
    try {

        Folderblock fb;
        Fileblock flb;

        string strGrafica;
        strGrafica += "digraph G{ \n rankdir=LR; \n graph [ dpi = \"800\" ]; \n node [shape = plaintext]; \n";

        for (int i = 0; i < free_inodes; ++i) {
            strGrafica += "inode" + to_string(i) + " [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
            strGrafica += "<tr><td colspan = '2' > <b> i-Nodo " + to_string(i) + " </b></td></tr>\n";

            strGrafica += "<tr>\n <td><b>i_type</b></td> <td> <b>" + to_string(inodes.i_type) + "</b></td>\n </tr>\n";

            strGrafica += "<tr>\n <td>i_uid</td> <td>" + to_string(inodes.i_uid) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_gid</td> <td>" + to_string(inodes.i_gid) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_size</td> <td>" + to_string(inodes.i_s) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_atime</td> <td>" + get_date(inodes.i_atime) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_ctime</td> <td>" + get_date(inodes.i_ctime) + "</td>\n </tr>\n";
            strGrafica += "<tr>\n <td>i_mtime</td> <td>" + get_date(inodes.i_mtime) + "</td>\n </tr>\n";

            for (int j = 0; j < 15; ++j) {
                strGrafica += "<tr>\n <td>i_block_" + to_string(j+1) + "</td> <td port=\"" + to_string(j) + "\">" + to_string(inodes.i_block[j]) + "</td>\n </tr>\n";
            }

            strGrafica += "<tr>\n <td><b>i_perm</b></td> <td><b>" + to_string(inodes.i_perm) + "</b></td>\n </tr>\n";

            strGrafica += "</table>>];\n";

            if (inodes.i_type == 0) {
                for (int j = 0; j < 15; j++) {
                    if (inodes.i_block[j] != -1) {
                        strGrafica += "inode" + to_string(i) + ":" + to_string(j) + "-> block" + to_string(inodes.i_block[j]) + ":n\n";
                        fseek(&file, super.s_block_start + (sizeof(Folderblock) * inodes.i_block[j]),SEEK_SET);
                        fread(&fb, sizeof(Folderblock), 1, &file);
                        strGrafica += "block" + to_string(inodes.i_block[j]) + "  [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
                            strGrafica += "<tr><td colspan = '2' > <b> block " + to_string(inodes.i_block[j]) + "</b></td></tr>\n";
                            for (int k = 0; k < 4; k++) {
                                string aux;
                                if (fb.b_content[k].b_name[0] == '\0') {
                                    aux = " ";
                                } else {
                                    aux = fb.b_content[k].b_name;
                                }
                                strGrafica += "<tr>\n <td>" + aux + "</td>\n <td port=\"" + to_string(k) + "\">" + to_string(fb.b_content[k].b_inodo) + "</td>\n </tr>\n";
                            }
                            strGrafica += "</table>>];\n";

                        for (int b = 0; b < 4; b++) {
                            if (fb.b_content[b].b_inodo != -1) {
                                if (fb.b_content[b].b_name[0] != '\0' && fb.b_content[b].b_name[0] != '.') {
                                    strGrafica += "block" + to_string(inodes.i_block[j]) + ":" + to_string(b) + " -> inode" + to_string(fb.b_content[b].b_inodo) + ":n\n";
                                }
                            }
                        }
                    }
                }
            } else {
                for (int j = 0; j < 15; j++) {
                    if (inodes.i_block[j] != -1) {
                        if (j < 12) {
                            strGrafica += "inode" + to_string(i) + ":" + to_string(j) + "-> block" + to_string(inodes.i_block[j]) + ":n\n";
                            fseek(&file, super.s_block_start + (sizeof(Fileblock) * inodes.i_block[j]), SEEK_SET);
                            fread(&flb, sizeof(Fileblock), 1, &file);
                            strGrafica += "block" + to_string(inodes.i_block[j]) + " [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
                            strGrafica += "<tr><td colspan = '2' ><b> block " + to_string(inodes.i_block[j]) + "</b></td></tr>\n";
                            strGrafica += "<tr><td colspan = '2'>"; 
                            strGrafica += flb.b_content;
                            strGrafica += "</td></tr>\n"; 
                            strGrafica += "</table>>];\n";
                        }
                    }
                }
            }
            fseek(&file, super.s_inode_start + (sizeof(Inodes) * (i + 1)), SEEK_SET);
            fread(&inodes, sizeof(Inodes), 1, &file);
        }

        strGrafica += "\n\n}\n";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

//reporte superblock
void manager::spr_report(string rep_path, string file_dot, Superblock super) {
    try {
        
        string strGrafica;
        strGrafica += "digraph G{ \n graph [ dpi = \"800\" ]; \n node [shape = plaintext]; \n mbr [label = < \n";
        strGrafica += "<table border='0' cellborder='1' cellspacing='0'>\n";
        strGrafica += "<tr><td colspan = '2' ><b>Superblock</b></td></tr>\n";
        strGrafica += "<tr>\n <td><b>s_filesystem_type</b></td> <td><b>" + to_string(super.s_filesystem_type) + "</b></td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_inodes_count</td>\n<td>" + to_string(super.s_inodes_count) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_free_inodes_count</td>\n<td>" + to_string(super.s_free_inodes_count) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_blocks_count</td>\n<td>" + to_string(super.s_blocks_count) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_free_blocks_count</td>\n<td>" + to_string(super.s_free_blocks_count) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_mtime</td>\n<td>" + get_date(super.s_mtime) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_umtime</td>\n<td>" + get_date(super.s_umtime) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_mnt_count</td>\n<td>" + to_string(super.s_mnt_count) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_magic</td>\n<td>" + to_string(super.s_magic) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_inode_size</td>\n<td>" + to_string(super.s_inode_s) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_block_size</td>\n<td>" + to_string(super.s_block_s) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_first_ino</td>\n<td>" + to_string(super.s_first_ino) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_first_blo</td>\n<td>" + to_string(super.s_first_blo) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_bm_inode_start</td>\n<td>" + to_string(super.s_bm_inode_start) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_bm_block_start</td>\n<td>" + to_string(super.s_bm_block_start) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_inode_start</td>\n<td>" + to_string(super.s_inode_start) + "</td>\n </tr>\n";
        strGrafica += "<tr>\n <td>s_block_start</td>\n<td>" + to_string(super.s_block_start) + "</td>\n </tr>\n";
        strGrafica += "</table>>];";
        strGrafica += "\n\n}\n";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

//reporte journaling
void manager::jrl_report(string rep_path, FILE &file, string file_dot,  Superblock super, Journaling journal, Partition partition) {
    try {

        if(super.s_filesystem_type == 2){
            throw runtime_error("sistema de archivos ext3 requerido");
        }

        string strGrafica;
        strGrafica += "digraph G{ \n rankdir=LR; graph [ dpi = \"800\" ]; \n node [shape = plaintext];";

        int i = 0;
        do {
            if (journal.type == -1){
                break;
            }
            strGrafica += "jor" + to_string(i) + "  [label = <<table border='0' cellborder='1' cellspacing='0'>\n";
            strGrafica += "<tr><td colspan = '2' ><b>Journaling " + to_string(i) + "</b></td></tr>\n";
            strGrafica += "<tr><td >operation</td><td >";
            strGrafica += journal.operation;
            strGrafica += "</td></tr>\n";
            strGrafica += "<tr><td ><b>type</b></td><td ><b>" + to_string(journal.type) + "</b></td></tr>\n";
            strGrafica += "<tr><td >path</td><td >";
            strGrafica += journal.path;
            strGrafica += "</td></tr>\n";
            strGrafica += "<tr><td >content</td><td >";
            strGrafica += journal.content;
            strGrafica += "</td></tr>\n";
            strGrafica += "<tr><td >date</td><td >" + get_date(journal.date) + "</td></tr>\n";
            strGrafica += "<tr><td >size</td><td >" + to_string(journal.size) + "</td></tr>\n";
            strGrafica += "</table>>];\n";
            if (i != 0) {
                strGrafica += "jor" + to_string(i - 1) + "-> jor" + to_string(i) + "\n";
            }
            i++;
            fseek(&file, partition.part_start + sizeof(Superblock) + (i * sizeof(Journaling)), SEEK_SET);
            fread(&journal, sizeof(Journaling), 1, &file);
        } while (true);

        strGrafica += "\n\n}\n";
        ofstream outfile(file_dot);
        outfile << strGrafica.c_str() << endl;
        outfile.close();
        string function = "dot -Tjpg " + file_dot + " -o " + rep_path;
        system(function.c_str());
        function = "rm \"" + file_dot + "\"";
        system(function.c_str());
        cout << "Reporte generado en: " << rep_path << endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

//reporte file
void manager::fle_report(){

}

//reporte ls
void manager::lsf_report(){

}

