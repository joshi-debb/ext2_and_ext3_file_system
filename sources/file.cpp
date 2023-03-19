
#include "../headers/file.h"
#include "../headers/user.h"

#include <string>
#include <iostream>
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

void file::mkdir(vector<string> tks, user Usr, disk Dsk) {

    bool r = false;
    string path;
    string file_path;
    User = Usr;
    Disk = Dsk;

    //extraer parametros
    for (string token : tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length() + 1);
        if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
        }else if (lower(tk) == "r"){
            r = true;
        }
    }

    Partition partition = Disk.get_mount(User.active_user.id, &file_path);

    vector<string> tmp = split_path(path);

    make_dir_file(tmp, r, partition, file_path, "", "");

    cout << "Se ha creado el directorio" << endl;

}

void file::mkfile(vector<string> tks, user Usr, disk Dsk){

    bool r = false;
    string path;
    int size = 0;
    string cont_path;
    string file_path;
    User = Usr;
    Disk = Dsk;

    string new_file;
    int father_num = 0;

    //extraer parametros
    for (string token : tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length() + 1);
        if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
        }else if (lower(tk) == "r"){
            r = true;
        }else if (lower(tk) == "size"){
            size = stoi(token);
        }else if (lower(tk) == "cont"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                cont_path = token.substr(1, token.length() - 2);
            }else{
                cont_path = token;
            }
        }
    }

    //obtener el nombre de la carpeta padre del archivo
    vector<string> aux_tmp = split_path(path);
    aux_tmp.pop_back();
    new_file = aux_tmp.back();

    Partition partition = Disk.get_mount(User.active_user.id, &file_path);

    vector<string> tmp = split_path(path);

    //crear contenido segun parametro size
    string contenido = "";
    
    if (cont_path != ""){

        FILE *cont_file = fopen(cont_path.c_str(), "rb+");
        if (cont_file != NULL){
            char ch;
            while ((ch = fgetc(cont_file)) != EOF){
                contenido += ch;
            }
            fclose(cont_file);
        }

    }else{
        
        for (int i = 0; i < size; i++) {
            contenido += to_string(i % 10);
        }

    }

    make_dir_file(tmp, r, partition, file_path, new_file, contenido);

    cout << "Se ha creado el archivo" << endl;
}

void file::make_dir_file(vector<string> tmp, bool r, Partition partition, string file_path, string new_file, string content) {
    try {

        bool is_file = false;

        Superblock spr;
        Inodes inode, inodetmp;
        Folderblock folder, foldertmp;
        Pointerblock pointer;
        
        FILE *bfile = fopen(file_path.c_str(), "rb+");

        fseek(bfile, partition.part_start, SEEK_SET);
        fread(&spr, sizeof(Superblock), 1, bfile);

        fseek(bfile, spr.s_inode_start, SEEK_SET);
        fread(&inode, sizeof(Inodes), 1, bfile);

        char ch = 'x', one = '1';

        bool fnd = false;
        int father = 0;
        int past, bi, bb;

        string stack, new_file_dir;
        new_file_dir = tmp.back();
        tmp.pop_back();


        for (int v = 0; v < tmp.size(); ++v) {
            fnd = false;
            for (int i = 0; i < 15; ++i) {
                if (inode.i_block[i] != -1) {
                    //apuntadores directos  
                    if (i < 12){
                        folder = Folderblock();
                        fseek(bfile, spr.s_block_start + (sizeof(Folderblock) * inode.i_block[i]), SEEK_SET);
                        fread(&folder, sizeof(Folderblock), 1, bfile);
                        for (auto &j : folder.b_content) {
                            if (lower(j.b_name) == lower(tmp.at(v))) {
                                if (j.b_name == new_file){
                                    is_file = true;
                                }
                                fnd = true;
                                father = j.b_inodo;
                                inode = Inodes();
                                fseek(bfile,spr.s_inode_start + (sizeof(Inodes) * j.b_inodo),SEEK_SET);
                                fread(&inode, sizeof(Inodes), 1, bfile);
                                break;
                            }
                        }
                    }else{
                        //apuntadores indirectos
                        pointer = Pointerblock();
                        fseek(bfile, spr.s_block_start + (sizeof(Pointerblock) * inode.i_block[i]), SEEK_SET);
                        fread(&pointer, sizeof(Pointerblock), 1, bfile);

                        Pointerblock pointer2;
                        fseek(bfile, spr.s_block_start + (sizeof(Pointerblock) * inode.i_block[i]),SEEK_SET);
                        fread(&pointer2, sizeof(Pointerblock), 1, bfile);

                        Pointerblock pointer3;
                        fseek(bfile, spr.s_block_start + (sizeof(Pointerblock) * inode.i_block[i]),SEEK_SET);
                        fread(&pointer3, sizeof(Pointerblock), 1, bfile);

                        for (int b_pointer : pointer.b_pointers) {
                            if (b_pointer != -1) {
                                folder = Folderblock();
                                fseek(bfile, spr.s_block_start + (sizeof(Folderblock) * b_pointer),SEEK_SET);
                                fread(&folder, sizeof(Folderblock), 1, bfile);

                                //indirecto simple
                                if(i == 13){
                                    for (auto &j : folder.b_content) {
                                        if (lower(j.b_name) == lower(tmp.at(v))) {
                                            fnd = true;
                                            father = j.b_inodo;
                                            inode = Inodes();
                                            fseek(bfile,spr.s_inode_start + (sizeof(Inodes) * j.b_inodo),SEEK_SET);
                                            fread(&inode, sizeof(Inodes), 1, bfile);
                                            break;
                                        }
                                    }
                                //indirecto doble
                                }else if(i == 14){
                                    for (int b_pointer2 : pointer2.b_pointers) {
                                        if (b_pointer2 != -1) {
                                            for (auto &j : folder.b_content) {
                                                if (lower(j.b_name) == lower(tmp.at(v))) {
                                                    fnd = true;
                                                    father = j.b_inodo;
                                                    inode = Inodes();
                                                    fseek(bfile,spr.s_inode_start + (sizeof(Inodes) * j.b_inodo),SEEK_SET);
                                                    fread(&inode, sizeof(Inodes), 1, bfile);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                //indirecto triple
                                }else if(i == 15){
                                    for (int b_pointer2 : pointer2.b_pointers) {
                                        if (b_pointer2 != -1) {
                                            for (int b_pointer3 : pointer3.b_pointers) {
                                                if (b_pointer3 != -1) {
                                                    for (auto &j : folder.b_content) {
                                                        if (lower(j.b_name) == lower(tmp.at(v))) {
                                                            fnd = true;
                                                            father = j.b_inodo;
                                                            inode = Inodes();
                                                            fseek(bfile,spr.s_inode_start + (sizeof(Inodes) * j.b_inodo),SEEK_SET);
                                                            fread(&inode, sizeof(Inodes), 1, bfile);
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (!fnd) {
                if (r) {
                    stack += "/" + tmp.at(v);
                    make_dir_file(split_path(stack), false, partition, file_path,new_file, content);
                    v = -1;
                    fseek(bfile, spr.s_inode_start, SEEK_SET);
                    fread(&inode, sizeof(Inodes), 1, bfile);
                } else {
                    throw runtime_error("no es posible crear el directorio");
                }

            }
        }

        Fileblock files;

        fnd = false;
        for (int i = 0; i < 15; ++i) {
            if (inode.i_block[i] != -1) {
                if (i < 12) {
                    fseek(bfile, spr.s_block_start + (sizeof(Folderblock) * inode.i_block[i]), SEEK_SET);
                    fread(&folder, sizeof(Folderblock), 1, bfile);
                    for (int j = 0; j < 4; ++j) {
                        if (folder.b_content[j].b_inodo == -1) {
                            past = inode.i_block[i];

                            //obtener el inodo y bloque libre
                            fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
                            for (int i = 0; i < spr.s_inodes_count; i++) {
                                fread(&ch, sizeof(ch), 1, bfile);
                                if (ch == '0') {
                                    bi = i;
                                    break;
                                }
                            }
                            
                            fseek(bfile, spr.s_bm_block_start, SEEK_SET);
                            for (int i = 0; i < spr.s_blocks_count; i++) {
                                fread(&ch, sizeof(ch), 1, bfile);
                                if (ch == '0') {
                                    bb = i;
                                    break;
                                }
                            }

                            inodetmp.i_uid = 1;
                            inodetmp.i_gid = 1;
                            inodetmp.i_atime = spr.s_umtime;
                            inodetmp.i_ctime = spr.s_umtime;
                            inodetmp.i_mtime = spr.s_umtime;
                            inodetmp.i_perm = 664;
                            inodetmp.i_block[0] = bb;

                            if (is_file) {
                                inodetmp.i_type = 1;
                                inodetmp.i_s = sizeof(content.c_str()) + sizeof(Folderblock);
                                strcpy(foldertmp.b_content[0].b_name, content.c_str() );
                                foldertmp.b_content[0].b_inodo = bi;
                                foldertmp.b_content[1].b_inodo = father;
                                folder.b_content[j].b_inodo = bi;
                                strcpy(folder.b_content[j].b_name, new_file_dir.c_str());
                            } else {
                                inodetmp.i_type = 0;
                                inodetmp.i_s = sizeof(sizeof(Folderblock));
                                strcpy(foldertmp.b_content[0].b_name, ".");
                                foldertmp.b_content[0].b_inodo = bi;
                                strcpy(foldertmp.b_content[1].b_name, "..");
                                foldertmp.b_content[1].b_inodo = father;
                                strcpy(foldertmp.b_content[2].b_name, "-");
                                strcpy(foldertmp.b_content[3].b_name, "-");
                                folder.b_content[j].b_inodo = bi;
                                strcpy(folder.b_content[j].b_name, new_file_dir.c_str());
                            }

                            fnd = true;
                            i = 20;
                            break;
                        }
                    }

                }
            }
        }

        if (!fnd) {
            for (int i = 0; i < 15; ++i) {
                if (inode.i_block[i] == -1) {
                    if (i < 12) {

                        //obtener el inodo y bloque libre
                        fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
                        for (int i = 0; i < spr.s_inodes_count; i++) {
                            fread(&ch, sizeof(ch), 1, bfile);
                            if (ch == '0') {
                                bi = i;
                                break;
                            }
                        }

                        fseek(bfile, spr.s_bm_block_start, SEEK_SET);
                        for (int i = 0; i < spr.s_blocks_count; i++) {
                            fread(&ch, sizeof(ch), 1, bfile);
                            if (ch == '0') {
                                past = i;
                                break;
                            }
                        }

                        folder = Folderblock();
                        strcpy(folder.b_content[0].b_name, ".");
                        folder.b_content[0].b_inodo = bi;
                        strcpy(folder.b_content[1].b_name, "..");
                        folder.b_content[1].b_inodo = father;
                        folder.b_content[2].b_inodo = bi;
                        strcpy(folder.b_content[2].b_name, new_file_dir.c_str());
                        strcpy(folder.b_content[3].b_name, "-");

                        inode.i_block[i] = past;

                        //updatebm
                        int num;
                        fseek(bfile, spr.s_bm_block_start, SEEK_SET);
                        for (int i = 0; i < spr.s_blocks_count; i++) {
                            fread(&ch, sizeof(ch), 1, bfile);
                            if (ch == '0') {
                                num = i;
                                break;
                            }
                        }
                        fseek(bfile, spr.s_bm_block_start + num, SEEK_SET);
                        fwrite(&one, sizeof(one), 1, bfile);

                        //obtener el bloque libre
                        fseek(bfile, spr.s_bm_block_start, SEEK_SET);
                        for (int i = 0; i < spr.s_blocks_count; i++) {
                            fread(&ch, sizeof(ch), 1, bfile);
                            if (ch == '0') {
                                bb = i;
                                break;
                            }
                        }

                        inodetmp.i_uid = 1;
                        inodetmp.i_gid = 1;
                        inodetmp.i_s = sizeof(sizeof(Folderblock));
                        inodetmp.i_atime = spr.s_umtime;
                        inodetmp.i_ctime = spr.s_umtime;
                        inodetmp.i_mtime = spr.s_umtime;
                        inodetmp.i_type = 0;
                        inodetmp.i_perm = 664;
                        inodetmp.i_block[0] = bb;

                        strcpy(foldertmp.b_content[0].b_name, ".");
                        foldertmp.b_content[0].b_inodo = bi;
                        strcpy(foldertmp.b_content[1].b_name, "..");
                        foldertmp.b_content[1].b_inodo = father;
                        strcpy(foldertmp.b_content[2].b_name, "-");
                        strcpy(foldertmp.b_content[3].b_name, "-");

                        fseek(bfile, spr.s_inode_start + (sizeof(Inodes) * father), SEEK_SET);
                        fwrite(&inode, sizeof(Inodes), 1, bfile);
                        break;
                    }
                }
            }


        }

        //crear inodos y bloques
        fseek(bfile, spr.s_inode_start + (sizeof(Inodes) * bi), SEEK_SET);
        fwrite(&inodetmp, sizeof(Inodes), 1, bfile);

        fseek(bfile, spr.s_block_start + (sizeof(Folderblock) * bb), SEEK_SET);
        fwrite(&foldertmp, sizeof(Folderblock), 1, bfile);

        fseek(bfile, spr.s_block_start + (sizeof(Folderblock) * past), SEEK_SET);
        fwrite(&folder, sizeof(Folderblock), 1, bfile);

        //actualizar bitmap inodods y bloques
        int num = 0;

        fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
        for (int i = 0; i < spr.s_inodes_count; i++) {
            fread(&ch, sizeof(ch), 1, bfile);
            if (ch == '0') {
                num = i;
                break;
            }
        }
        fseek(bfile, spr.s_bm_inode_start + num, SEEK_SET);
        fwrite(&one, sizeof(one), 1, bfile);

        num = 0;

        fseek(bfile, spr.s_bm_block_start, SEEK_SET);
        for (int i = 0; i < spr.s_blocks_count; i++) {
            fread(&ch, sizeof(ch), 1, bfile);
            if (ch == '0') {
                num = i;
                break;
            }
        }
        fseek(bfile, spr.s_bm_block_start + num, SEEK_SET);
        fwrite(&one, sizeof(one), 1, bfile);

        //si es ext3 se crea el journaling
        if (spr.s_filesystem_type == 3) {

            string event;
            for (string ttpp: tmp) {
                event += "/" + ttpp;
            }
            event += "/" + new_file_dir;
            Journaling journaling;

            if (is_file){
                strcpy(journaling.content, content.c_str());
                strcpy(journaling.path, event.data());
                journaling.type = 1;
                journaling.size = sizeof(content) + sizeof(Folderblock);
                strcpy(journaling.operation, "mkfile");
            }else{
                strcpy(journaling.content, event.c_str());   
                strcpy(journaling.path, event.data());
                journaling.type = 0;
                journaling.size = sizeof(event) + sizeof(Folderblock);
                strcpy(journaling.operation, "mkdir");
            }
            
            journaling.date = spr.s_umtime;

            FILE *file_aux = fopen(file_path.c_str(), "rb+");
            fseek(file_aux, partition.part_start + sizeof(Superblock), SEEK_SET);

            Journaling journal_aux;

            fseek(file_aux, partition.part_start + sizeof(Superblock), SEEK_SET);
            fread(&journal_aux, sizeof(Journaling), 1, file_aux);

            int i = 0;
            do {
                if (journal_aux.type == -1){
                    break;
                }
                i++;
                fseek(file_aux, partition.part_start + sizeof(Superblock) + (i * sizeof(Journaling)), SEEK_SET);
                fread(&journal_aux, sizeof(Journaling), 1, file_aux);
            } while (true);

            fseek(bfile, partition.part_start + sizeof(Superblock) + (sizeof(Journaling) * i ),SEEK_SET);
            fwrite(&journaling, sizeof(Journaling), 1, bfile);
            fclose(file_aux);
        }

        fclose(bfile);
    }
    catch (exception &e) {
        cout << e.what() << endl;
        return;
    }

}

void file::cat(vector<string> tks, user Usr, disk Dsk){

    string path1;
    string content;
    string file_path;
    
    User = Usr;
    Disk = Dsk;

    //extraer parametros
    for (string token : tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length() + 1);
        if (lower(tk) == "file1"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path1 = token.substr(1, token.length() - 2);
            }else{
                path1 = token;
            }
        }
    }

    try{
    
        Partition partition = Disk.get_mount(User.active_user.id, &file_path);

        vector<string> tmp = split_path(path1);

        string name = tmp.back();
        
        int block_num = 0;

        int free_inodes = -1;
        int free_blocks = -1;

        Superblock super;
        Inodes inodes;
        Journaling journal;


        FILE *file = fopen(file_path.c_str(), "rb+");

        //leer superblock
        fseek(file, partition.part_start, SEEK_SET);
        fread(&super, sizeof(Superblock), 1, file);

        //leer inodos
        fseek(file, super.s_inode_start, SEEK_SET);
        fread(&inodes, sizeof(Inodes), 1, file);

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

        Folderblock fb;
        Fileblock flb;

        for (int i = 0; i < free_inodes; i++) {
            for (int j = 0; j < 15; j++) {
                if (inodes.i_block[j] != -1) {
                    if (j < 12) {
                        if (inodes.i_type == 1 && block_num != 0) {
                            fseek(file, super.s_block_start + (sizeof(Fileblock) * inodes.i_block[j]),SEEK_SET);
                            fread(&flb, sizeof(Fileblock), 1, file);
                            cout << "#" << flb.b_content << endl;
                            return;
                        } else {
                            fseek(file, super.s_block_start + (sizeof(Folderblock) * inodes.i_block[j]),SEEK_SET);
                            fread(&fb, sizeof(Fileblock), 1, file);

                            for (int k = 0; k < 4; k++) {
                                string aux;
                                if (fb.b_content[k].b_name[0] == '\0') {
                                    aux = " ";
                                } else {
                                    aux = fb.b_content[k].b_name;
                                }
                                if (lower(aux) == lower(name)) {
                                    block_num = inodes.i_block[j];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            fseek(file, super.s_inode_start + (sizeof(Inodes) * (i + 1)), SEEK_SET);
            fread(&inodes, sizeof(Inodes), 1, file);
        }
        fclose(file);



    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

vector<string> file::split_path(string path) {
    vector<string> result;
    if (path.empty()) {
        return result;
    }

    stringstream ss(path);
    string item;
    while (getline(ss, item, '/')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

//funcion para convertir a minusculas
string file::lower(string String){
    string aux;
    locale loc;
    for (int i = 0; i < String.length(); i++){
        aux += tolower(String[i], loc);
    }
    return aux;
}