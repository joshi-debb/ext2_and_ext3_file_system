
#include "../headers/disk.h"

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>

using namespace std;

//comando mkdisk
void disk::mkdisk(vector<string> tks){

    //inicializar variables
    int size = 0;
    string path = "";
    string aux_path = "";
    string fit = "ff";
    string unit = "m";

    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "fit"){
            if (lower(token) == "bf" || lower(token) == "ff" || lower(token) == "wf"){
                fit = lower(token);
            }else{
                cout << "Parametro fit no valido" << endl; 
            }
        }else if(lower(tk) == "unit"){
            if (lower(token)  == "k" || lower(token) == "m"){
                unit = lower(token);  
            }else{
                cout<< "Parametro unit no valido" << endl; 
            }
        }else if (lower(tk) == "size"){
            size = stoi(token);
            if( size <= 0){
                cout << "Parametro size no valido" << endl;
                return;
            }
        }else if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
            //verificar extension del archivo.dsk
            if(lower(path.substr(path.find_last_of(".") + 1)) != "dsk"){
                cout << "Error: Extensión de archivo no valida" << endl;
                return;
            }
            //obtener ruta de carpetas
            aux_path = path;
            for (int i = aux_path.length() - 1; i >= 0; i--){
                if (path[i] == '/'){
                    aux_path = aux_path.substr(0, i);
                    break;
                }
            }

        }else{
            cout << "No se esperaba el parametro: " << tk << endl;
            break;
        }
    }

    //verificar si existe el directorio
    DIR* dir = opendir(aux_path.c_str());
    if (dir) {
        closedir(dir);
    } else {
        string cmds = "mkdir -p \"" + aux_path + "\"";
        system(cmds.c_str());
    }

    //crear disco
    MBR disco;

    //inicializar disco
    fit = fit.substr(0,2);

    if (unit == "m"){
        size = 1024*1024*size;
    }else if (unit =="k"){
        size = 1024*size;
    }

    disco.mbr_tamano = size;
    disco.mbr_fecha_creacion = time(nullptr);
    disco.dsk_fit = tolower(fit[0]);
    disco.mbr_dsk_signature = rand() % 9999 + 100;

    //inicilizar particiones
    disco.mbr_Partition_1 = Partition();
    disco.mbr_Partition_2 = Partition();
    disco.mbr_Partition_3 = Partition();
    disco.mbr_Partition_4 = Partition();

    //crear archivo binario
    FILE *file = fopen(path.c_str(), "w+b");
    if (file!=NULL){
        fwrite("\0", 1, 1, file);
        fseek(file, size - 1, SEEK_SET);
        fwrite("\0", 1, 1, file);
        rewind(file);
        fwrite(&disco, sizeof(MBR), 1, file);
        fclose(file);
    }
    cout << ">MKDISK - Disco Creado en: " << path << endl;
}

//comando rmdisk
void disk::rmdisk(vector<string> tks){
    string path = "";
    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
            //verificar extension del archivo.dsk
            if(lower(path.substr(path.find_last_of(".") + 1)) != "dsk"){
                cout << "Error: Extensión de archivo no valida" << endl;
                return;
            }
        }
    
    }
    //si la ruta no esta vacia
    if (!path.empty()){
        try{
            //verificar si el archivo existe
            FILE *file = fopen(path.c_str(), "r");
            if (file != NULL){
                fclose(file);
                cout << "¿Desea eliminar el archivo? [Y/N]" << endl;
                string respuesta;
                getline(cin,respuesta);
                if (lower(respuesta) == "y"){
                    if (remove(path.c_str()) == 0){
                        cout << "Disco eliminado correctamente" << endl;
                        return;
                    }
                }else{
                    cout << "Operación cancelada" << endl;
                    return;
                }
            }
            cout << "Error: El disco que desea eliminar no existe en la ruta indicada" << endl;
        }catch(const std::exception& e){
            cout << "Error: " << e.what() << endl;
        }
        
    }

}

//comando fdisk
void disk::fdisk(vector<string> tks){ 
    //inicializar variables
    string fit = "wf";
    string unit = "k";
    int size = 0;
    string path = "";
    string type = "p";
    string name = "";
    int add  = 0;
    string suprim = "";

    //extraer parametros 
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "fit"){
            if (lower(token) == "bf" || lower(token) == "ff" || lower(token) == "wf"){
                fit = lower(token);
            }else{
                cout << "Parametro fit no valido" << endl; 
            }
        }else if(lower(tk) == "unit"){
            if (lower(token) == "b" || lower(token)  == "k" || lower(token) == "m"){
                unit = lower(token);  
            }else{
                cout<< "Parametro unit no valido" << endl; 
            }
        }else if (lower(tk) == "size"){
            size = stoi(token);
            if( size <= 0){
                cout << "Parametro size no valido" << endl;
                return;
            }
        }else if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
        }else if (lower(tk) == "type") {
            if (lower(token) == "p" || lower(token)  == "e" || lower(token) == "l"){
                type = lower(token);
            }else{
                cout<< "Parametro type no valido" << endl; 
            }
        } else if (lower(tk) == "name") {
            if (token.substr(0, 1) == "\""){
                name = token.substr(1, token.length() - 2);
            }else{
                name = token;
            }
        } else if (lower(tk) == "add") {
            add = stoi(token);
        }else if (lower(tk) == "delete") {
            if (lower(token) == "full"){
                suprim = lower(token);  
            }else{
                cout<< "Parametro delete no valido" << endl; 
            }
        }else{
            cout << "No se esperaba el parametro: " << tk << endl;
        }
            
    }

    if (suprim != "full") {

        if (add == 0) {
           
            try{
                aux_value = 0;

                if (unit == "m"){
                    size *= 1024*1024;
                } else if (unit =="k"){
                    size *= 1024;
                }

                MBR disco;
                FILE *file = fopen(path.c_str(), "rb+");
                if (file == NULL){
                    cout << "No se encontro el disco" << endl;
                    return;
                }
                rewind(file);
                fread(&disco, sizeof(disco),1,file);
                fclose(file);

                vector<Partition> partitions = {disco.mbr_Partition_1, disco.mbr_Partition_2, disco.mbr_Partition_3, disco.mbr_Partition_4};
                vector<Stepper> between;

                int used = 0, ext = 0, control = 1, base = sizeof(disco);
                Partition extended;

                for (Partition prttn : partitions) {
                    if (prttn.part_status == '1') {
                        Stepper trn = {control, prttn.part_start, prttn.part_start + prttn.part_s, trn.start - base};
                        base = trn.end;

                        if (used != 0) {
                            between.at(used-1).after = trn.start - (between.at(used-1).end);
                        }
                        between.push_back(trn);
                        used++;

                        if (prttn.part_type == 'e' || prttn.part_type == 'E') {
                            ext++;
                            extended = prttn;
                        }
                    }

                    if (used == 4 && lower(type) != "l") {
                        throw runtime_error("Limite de particiones alcanzado");
                    } else if (ext == 1 && lower(type) == "e") {
                        throw runtime_error("Solo se puede crear una particion Extendida");
                    }
                    control++;
                }

                if (ext == 0 && lower(type) == "l") {
                    throw runtime_error("No existe particion Extendida para crear la Logica");
                }
                if (used != 0) {
                    between.at(between.size() - 1).after = disco.mbr_tamano - between.at(between.size() - 1).end;
                }

                try {
                    tracker(disco, name, path);
                    cout << ">FDISK - Este nombre ya esta en uso" << endl;
                    return;
                } catch (exception &e) {}

                //primer marcador
                Partition transition;
                transition.part_status = '1';
                transition.part_s = size;
                transition.part_type = toupper(type[0]);
                transition.part_fit = toupper(fit[0]);
                strcpy(transition.part_name, name.c_str());

                if (lower(type) == "l") {
                    EBR nlogic;
                    nlogic.part_status = '1';
                    nlogic.part_fit = transition.part_fit;
                    nlogic.part_s = transition.part_s;
                    nlogic.part_next = -1;
                    strcpy(nlogic.part_name, transition.part_name);

                    FILE* file = fopen(path.c_str(), "rb+");
                    fseek(file, extended.part_start, SEEK_SET);
                    EBR tmp;
                    int s = 0;
                    while (fread(&tmp, sizeof(EBR), 1, file)) {
                        s += sizeof(EBR) + tmp.part_s;
                        if (tmp.part_status == '0' && tmp.part_next == -1) {
                            nlogic.part_start = tmp.part_start;
                            nlogic.part_next = nlogic.part_start + nlogic.part_s + sizeof(EBR);
                            if ((extended.part_s - s) <= nlogic.part_s) {
                                throw runtime_error("no hay espacio para más particiones lógicas");
                            }
                            fseek(file, nlogic.part_start, SEEK_SET);
                            fwrite(&nlogic, sizeof(EBR), 1, file);
                            fseek(file, nlogic.part_next, SEEK_SET);
                            EBR addLogic;
                            addLogic.part_status = '0';
                            addLogic.part_next = -1;
                            addLogic.part_start = nlogic.part_next;
                            fwrite(&addLogic, sizeof(EBR), 1, file);
                            cout << ">FDISK - particion logica creada correctamente" << endl;
                            fclose(file);
                            return;
                        }
                        fseek(file, tmp.part_next, SEEK_SET);
                    }
                    return;
                }

                //segundo marcador
                if (used == 0) {
                    transition.part_start = sizeof(disco);
                    aux_value = transition.part_start;
                    disco.mbr_Partition_1 = transition;
                } else {
                    Stepper toUse;
                    int control = 0;
                    for (Stepper tr : between) {
                        if (control == 0) {
                            toUse = tr;
                            control++;
                            continue;
                        }

                        if (toupper(disco.dsk_fit) == 'F') {
                            if (toUse.before >= transition.part_s || toUse.after >= transition.part_s) {
                                break;
                            }
                            toUse = tr;
                        } else if (toupper(disco.dsk_fit) == 'B') {
                            if (toUse.before < transition.part_s || toUse.after < transition.part_s) {
                                toUse = tr;
                            } else {
                                if (tr.before >= transition.part_s || tr.after >= transition.part_s) {
                                    int b1 = toUse.before - transition.part_s;
                                    int a1 = toUse.after - transition.part_s;
                                    int b2 = tr.before - transition.part_s;
                                    int a2 = tr.after - transition.part_s;

                                    if ((b1 < b2 && b1 < a2) || (a1 < b2 && a1 < a2)) {
                                        control++;
                                        continue;
                                    }
                                    toUse = tr;
                                }
                            }
                        } else if (toupper(disco.dsk_fit) == 'W') {
                            if (!(toUse.before >= transition.part_s) || !(toUse.after >= transition.part_s)) {
                                toUse = tr;
                            } else {
                                if (tr.before >= transition.part_s || tr.after >= transition.part_s) {
                                    int b1 = toUse.before - transition.part_s;
                                    int a1 = toUse.after - transition.part_s;
                                    int b2 = tr.before - transition.part_s;
                                    int a2 = tr.after - transition.part_s;

                                    if ((b1 > b2 && b1 > a2) || (a1 > b2 && a1 > a2)) {
                                        control++;
                                        continue;
                                    }
                                    toUse = tr;
                                }
                            }
                        }
                        control++;
                    }
                    if (toUse.before >= transition.part_s || toUse.after >= transition.part_s) {
                        if (toupper(disco.dsk_fit) == 'F') {
                            if (toUse.before >= transition.part_s) {
                                transition.part_start = (toUse.start - toUse.before);
                                aux_value = transition.part_start;
                            } else {
                                transition.part_start = toUse.end;
                                aux_value = transition.part_start;
                            }
                        } else if (toupper(disco.dsk_fit) == 'B') {
                            int b1 = toUse.before - transition.part_s;
                            int a1 = toUse.after - transition.part_s;

                            if ((toUse.before >= transition.part_s && b1 < a1) || !(toUse.after >= transition.part_start)) {
                                transition.part_start = (toUse.start - toUse.before);
                                aux_value = transition.part_start;
                            } else {
                                transition.part_start = toUse.end;
                                aux_value = transition.part_start;
                            }
                        } else if (toupper(disco.dsk_fit) == 'W') {
                            int b1 = toUse.before - transition.part_s;
                            int a1 = toUse.after - transition.part_s;

                            if ((toUse.before >= transition.part_s && b1 > a1) || !(toUse.after >= transition.part_start)) {
                                transition.part_start = (toUse.start - toUse.before);
                                aux_value = transition.part_start;
                            } else {
                                transition.part_start = toUse.end;
                                aux_value = transition.part_start;
                            }
                        }
                        Partition partitionss[4];
                        for (int i = 0; i < partitions.size(); i++) {
                            partitionss[i] = partitions.at(i);
                        }
                        for (auto &partition : partitionss) {
                            if (partition.part_status == '0') {
                                partition = transition;
                                break;
                            }
                        }

                        Partition aux;
                        for (int i = 3; i >= 0; i--) {
                            for (int j = 0; j < i; j++) {
                                if ((partitionss[j].part_start > partitionss[j + 1].part_start)) {
                                    aux = partitionss[j + 1];
                                    partitionss[j + 1] = partitionss[j];
                                    partitionss[j] = aux;
                                }
                            }
                        }

                        for (int i = 3; i >= 0; i--) {
                            for (int j = 0; j < i; j++) {
                                if (partitionss[j].part_status == '0') {
                                    aux = partitionss[j];
                                    partitionss[j] = partitionss[j + 1];
                                    partitionss[j + 1] = aux;
                                }
                            }
                        }
                        disco.mbr_Partition_1 = partitionss[0];
                        disco.mbr_Partition_2 = partitionss[1];
                        disco.mbr_Partition_3 = partitionss[2];
                        disco.mbr_Partition_4 = partitionss[3];

                    } else {
                        throw runtime_error("no hay espacio suficiente");
                    }
                }

                

                //tercer marcador
                FILE *bfile = fopen(path.c_str(), "rb+");
                if (bfile != NULL) {
                    fseek(bfile, 0, SEEK_SET);
                    fwrite(&disco, sizeof(MBR), 1, bfile);
                    if (lower(type) == "e") {
                        EBR ebr;
                        ebr.part_start = aux_value;
                        fseek(bfile, aux_value, SEEK_SET);
                        fwrite(&ebr, sizeof(EBR), 1, bfile);
                    }
                    fclose(bfile);
                    cout << ">FDISK: partición creada correctamente" << endl;
                }
            }
            catch (invalid_argument &e) {
                cout << ">FDISK: " <<e.what() << endl;
                return;
            }
            catch (exception &e) {
                cout << ">FDISK: " <<e.what() << endl;
                return;
            }
        } else {

            try {

                if (unit == "m"){
                    add = 1024*1024*add;
                }else if (unit =="k"){
                    add = 1024*add;
                }else if (unit == "b"){
                    add = add;
                }

                FILE *file = fopen(path.c_str(), "rb+");
                if (file == NULL) {
                    throw runtime_error("disco no existente");
                }

                MBR disk;
                rewind(file);
                fread(&disk, sizeof(MBR), 1, file);

                tracker(disk, name, path);

                Partition partitions[4];
                partitions[0] = disk.mbr_Partition_1;
                partitions[1] = disk.mbr_Partition_2;
                partitions[2] = disk.mbr_Partition_3;
                partitions[3] = disk.mbr_Partition_4;


                for (int i = 0; i < 4; i++) {
                    if (partitions[i].part_status == '1') {
                        if (lower(partitions[i].part_name) == lower(name)) {
                            if ((partitions[i].part_s + (add)) > 0) {
                                if (i != 3) {
                                    if (partitions[i + 1].part_start != 0) {
                                        if (((partitions[i].part_s + (add) +
                                            partitions[i].part_start) <=
                                            partitions[i + 1].part_start)) {
                                            partitions[i].part_s += add;
                                            break;
                                        } else {
                                            throw runtime_error("se sobrepasa el límite");
                                        }
                                    }
                                }
                                if ((partitions[i].part_s + add + partitions[i].part_start) <= disk.mbr_tamano) {
                                    partitions[i].part_s += add;
                                    break;
                                } else {
                                    throw runtime_error("se sobrepasa el límite");
                                }

                            }
                        }
                    }
                }

                disk.mbr_Partition_1 = partitions[0];
                disk.mbr_Partition_2 = partitions[1];
                disk.mbr_Partition_3 = partitions[2];
                disk.mbr_Partition_4 = partitions[3];

                rewind(file);
                fwrite(&disk, sizeof(MBR), 1, file);
                cout << ">FDISK: la partición se ha aumentado/reducido correctamente" << endl;
                fclose(file);
            }
            catch (exception &e) {
                cout<< ">FDISK: " << e.what() << endl;
                return;
            }

        }

    } else if(suprim == "full") {

        try {

            FILE *file = fopen(path.c_str(), "rb+");
            if (file == NULL) {
                throw runtime_error("disco no existente");
            }

            MBR disk;
            rewind(file);
            fread(&disk, sizeof(MBR), 1, file);

            tracker(disk, name, path);

            Partition partitions[4];
            partitions[0] = disk.mbr_Partition_1;
            partitions[1] = disk.mbr_Partition_2;
            partitions[2] = disk.mbr_Partition_3;
            partitions[3] = disk.mbr_Partition_4;

            Partition past;
            bool fll = false;
            for (int i = 0; i < 4; i++) {
                if (partitions[i].part_status == '1') {
                    if (partitions[i].part_type == 'p') {
                        if (lower(partitions[i].part_name) == lower(name)) {
                            past = partitions[i];
                            partitions[i] = Partition();
                            fll = true;
                            break;
                        }
                    } else {
                        if (lower(partitions[i].part_name) == lower(name)) {
                            past = partitions[i];
                            partitions[i] = Partition();
                            fll = true;
                            break;
                        }
                        vector<EBR> ebrs = get_type_logic(partitions[i], path);
                        int count = 0;
                        for (EBR ebr : ebrs) {
                            if (lower(ebr.part_name) == lower(name)) {
                                ebr.part_status = '0';
                            }
                            fseek(file, ebr.part_start, SEEK_SET);
                            fwrite(&ebr, sizeof(EBR), 1, file);
                            count++;
                        }
                        cout << ">FDISK: partición eliminada correctamente -" + suprim << endl;
                        return;
                    }
                }
            }

            Partition aux;
            for (int i = 3; i >= 0; i--) {
                for (int j = 0; j < i; j++) {
                    if (partitions[j].part_status == '0') {
                        aux = partitions[j];
                        partitions[j] = partitions[j + 1];
                        partitions[j + 1] = aux;
                    }
                }
            }

            disk.mbr_Partition_1 = partitions[0];
            disk.mbr_Partition_2 = partitions[1];
            disk.mbr_Partition_3 = partitions[2];
            disk.mbr_Partition_4 = partitions[3];

            rewind(file);
            fwrite(&disk, sizeof(MBR), 1, file);
            if (fll) {
                fseek(file, past.part_start, SEEK_SET);
                int num = static_cast<int>(past.part_s / 2);
                fwrite("\0", sizeof("\0"), num, file);
            }
            cout << ">FDISK: partición eliminada correctamente " << endl;
            fclose(file);
        }
        catch (exception &e) {
            cout << ">FDISK: " << e.what() << endl;
            return;
        }
    }
}

//comando mount
void disk::mount(vector<string> tks) {
    string path = "";
    string name = "";
    string id = "53";

    //extraer parametros
    for (string token:tks){
        string tk = token.substr(0, token.find("="));
        token.erase(0,tk.length()+1);
        if (lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
            //verificar extension del archivo.dsk
            if(lower(path.substr(path.find_last_of(".") + 1)) != "dsk"){
                cout << "Error: Extensión de archivo no valida" << endl;
                return;
            }
        }else if (lower(tk) == "name"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                name = token.substr(1, token.length() - 2);
            }else{
                name = token;
            }
        }
    }

    if (tks.empty()) {
        show_mounted();
        return;
    }

    //verificar que los parametros obligatorios esten
    if (path.empty() || name.empty()) {
        cout << "Error: Faltan parametros obligatorios" << endl;
        return;
    }

    try {
        FILE *validate = fopen(path.c_str(), "r");
        if (validate == NULL) {
            throw runtime_error("disco no existente");
        }
        
        string name_disk = path.substr(path.find_last_of("/") + 1);
        name_disk = name_disk.substr(0, name_disk.find("."));

        MBR disk;
        rewind(validate);
        fread(&disk, sizeof(MBR), 1, validate);
        fclose(validate);

        Partition partition = tracker(disk, name, path);
        if (partition.part_type == 'E') {
            vector<EBR> ebrs = get_type_logic(partition, path);
            if (!ebrs.empty()) {
                EBR ebr = ebrs.at(0);
                name = ebr.part_name;
            } else {
                throw runtime_error("no se puede montar una particion extendida");
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mountd[i].path == path) {
                for (int j = 0; j < 99; j++) {
                    if (mountd[i].mounted_p[j].status == '0') {
                        mountd[i].mounted_p[j].status = '1';
                        strcpy(mountd[i].mounted_p[j].name, name.c_str());
                        id += to_string(j + 1) += name_disk;
                        strcpy(mountd[i].mounted_p[j].id, id.c_str());
                        cout << "se ha realizado correctamente el mount " << endl;
                        return;
                    }
                }
            }
        }
        
        for (int i = 0; i < 99; i++) {
            if (mountd[i].status == '0') {
                mountd[i].status = '1';
                strcpy(mountd[i].path, path.c_str());
                for (int j = 0; j < 99; j++) {
                    if (mountd[i].mounted_p[j].status == '0') {
                        mountd[i].mounted_p[j].status = '1';
                        strcpy(mountd[i].mounted_p[j].name, name.c_str());
                        id += to_string(j + 1) += name_disk;
                        strcpy(mountd[i].mounted_p[j].id, id.c_str());
                        cout << "se ha realizado correctamente el mount "  << endl;
                        return;
                    }
                }
            }
        }
    }
    catch (exception &e) {
        cout << e.what() << endl;
        return;
    }
}

//comando unmount
void disk::unmount(vector<string> tks) {

    string id = "";
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
        }
    }

    try {
        for (int i = 0; i < 99; i++){
            for (int j = 0; j < 99; j++) {
                if (mountd[i].mounted_p[j].status == '1') {
                    if (mountd[i].mounted_p[j].id == id) {
                        MPartition mp = MPartition();
                        mountd[i].mounted_p[j] = mp;
                        cout << "se ha realizado correctamente el unmount id=" << id << endl;
                        return;
                    }
                }
            }
        }
        throw runtime_error("id no existente");
    }
    catch (invalid_argument &e) {
        cout << e.what() << endl;
        return;
    }
}

//comando mkfs
void disk::mkfs(vector<string> tks) {
    string path;
    string id;
    string type = "Full";
    string fs = "2fs";

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
        }else if(lower(tk) == "type"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                type = token.substr(1, token.length() - 2);
            }else{
                type = token;
            }
            if (!(lower(type) == "fast"|| lower(type) == "full")) {
            throw runtime_error("valor de type no válido");
            }
        }else if (lower(tk) == "fs"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                fs = token.substr(1, token.length() - 2);
            }else{
                fs = token;
            }
            if (!(lower(fs) == "2fs"|| lower(fs) == "3fs")) {
            throw runtime_error("valor de fs no válido");
            }
        }
    }


    try {

        Partition partition = get_mount(id, &path);

        int n;
        if (lower(fs) == "2fs") {
            n = floor((partition.part_s - sizeof(Superblock)) / (sizeof(Inodes) + 3 * sizeof(Fileblock)));
        } else if (lower(fs) == "3fs"){
            n = floor((partition.part_s - sizeof(Superblock)) / (sizeof(Journaling) + sizeof(Inodes) + 3 * sizeof(Fileblock) + 1));
        }

        Superblock spr;
        spr.s_inodes_count = n;
        spr.s_free_inodes_count = n;
        spr.s_blocks_count = 3 * n;
        spr.s_free_blocks_count = 3 * n;
        spr.s_mtime = time(nullptr);
        spr.s_umtime = time(nullptr);
        spr.s_mnt_count = 1;
        if (lower(fs) == "2fs") {
            //sistema de archivos ext2
            spr.s_filesystem_type = 2;
            spr.s_bm_inode_start = partition.part_start + sizeof(Superblock);
            spr.s_bm_block_start = spr.s_bm_inode_start + n;
            spr.s_inode_start = spr.s_bm_block_start + (3 * n);
            spr.s_block_start = spr.s_inode_start + (n * sizeof(Inodes));

            FILE *bfile = fopen(path.c_str(), "rb+");
            fseek(bfile, partition.part_start, SEEK_SET);
            fwrite(&spr, sizeof(Superblock), 1, bfile);

            char zero = '0';
            fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
            for (int i = 0; i < n; i++) {
                fwrite(&zero, sizeof(zero), 1, bfile);
            }

            fseek(bfile, spr.s_bm_block_start, SEEK_SET);
            for (int i = 0; i < (3 * n); i++) {
                fwrite(&zero, sizeof(zero), 1, bfile);
            }

            Inodes inode;
            fseek(bfile, spr.s_inode_start, SEEK_SET);
            for (int i = 0; i < n; i++) {
                fwrite(&inode, sizeof(Inodes), 1, bfile);
            }

            Folderblock folder;
            fseek(bfile, spr.s_block_start, SEEK_SET);
            for (int i = 0; i < (3 * n); i++) {
                fwrite(&folder, sizeof(Folderblock), 1, bfile);
            }
            fclose(bfile);

            Superblock recuperado;
            FILE *archivo = fopen(path.c_str(), "rb");
            fseek(archivo, partition.part_start, SEEK_SET);
            fread(&recuperado, sizeof(Superblock), 1, archivo);
            fclose(archivo);
            inode.i_uid = 1;
            inode.i_gid = 1;
            inode.i_s = 0;
            inode.i_atime = spr.s_umtime;
            inode.i_ctime = spr.s_umtime;
            inode.i_mtime = spr.s_umtime;
            inode.i_type = 0;
            inode.i_perm = 664;
            inode.i_block[0] = 0;

            Folderblock fb;
            strcpy(fb.b_content[0].b_name, ".");
            fb.b_content[0].b_inodo = 0;
            strcpy(fb.b_content[1].b_name, "..");
            fb.b_content[1].b_inodo = 0;
            strcpy(fb.b_content[2].b_name, "users.txt");
            fb.b_content[2].b_inodo = 1;

            string data = "1,G,root\n1,U,root,root,123\n";
            Inodes inodetmp;
            inodetmp.i_uid = 1;
            inodetmp.i_gid = 1;
            inodetmp.i_s = sizeof(data.c_str()) + sizeof(Folderblock);
            inodetmp.i_atime = spr.s_umtime;
            inodetmp.i_ctime = spr.s_umtime;
            inodetmp.i_mtime = spr.s_umtime;
            inodetmp.i_block[0] = 1;
            inodetmp.i_type = 1;
            inodetmp.i_perm = 664;
            

            inode.i_s = inodetmp.i_s + sizeof(Folderblock) + sizeof(Inodes);

            Fileblock fileb;
            strcpy(fileb.b_content, data.c_str());

            FILE *bfiles = fopen(path.c_str(), "rb+");
            fseek(bfiles, spr.s_bm_inode_start, SEEK_SET);
            char caracter = '1';
            fwrite(&caracter, sizeof(caracter), 1, bfiles);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);

            fseek(bfiles, spr.s_bm_block_start, SEEK_SET);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);

            fseek(bfiles, spr.s_inode_start, SEEK_SET);
            fwrite(&inode, sizeof(Inodes), 1, bfiles);
            fwrite(&inodetmp, sizeof(Inodes), 1, bfiles);

            fseek(bfiles, spr.s_block_start, SEEK_SET);
            fwrite(&fb, sizeof(Folderblock), 1, bfiles);
            fwrite(&fileb, sizeof(Fileblock), 1, bfiles);
            fclose(bfiles);

        } else if (lower(fs) == "3fs"){
            //sisteme de archivos ext3
            spr.s_filesystem_type = 3;
            spr.s_bm_inode_start = partition.part_start + sizeof(Superblock) + (n * sizeof(Journaling));
            spr.s_bm_block_start = spr.s_bm_inode_start + n;
            spr.s_inode_start = spr.s_bm_block_start + (3 * n);
            spr.s_block_start = spr.s_inode_start + (n * sizeof(Inodes));

            FILE *bfile = fopen(path.c_str(), "rb+");
            fseek(bfile, partition.part_start, SEEK_SET);
            fwrite(&spr, sizeof(Superblock), 1, bfile);

            Journaling journaling;
            for (int i = 0; i < n; i++) {
                fwrite(&journaling, sizeof(Journaling), 1, bfile);
            }

            char zero = '0';
            fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
            for (int i = 0; i < n; i++) {
                fwrite(&zero, sizeof(zero), 1, bfile);
            }

            fseek(bfile, spr.s_bm_block_start, SEEK_SET);
            for (int i = 0; i < (3 * n); i++) {
                fwrite(&zero, sizeof(zero), 1, bfile);
            }

            Inodes inode;
            fseek(bfile, spr.s_inode_start, SEEK_SET);
            for (int i = 0; i < n; i++) {
                fwrite(&inode, sizeof(Inodes), 1, bfile);
            }

            Folderblock folder;
            fseek(bfile, spr.s_block_start, SEEK_SET);
            for (int i = 0; i < (3 * n); i++) {
                fwrite(&folder, sizeof(Folderblock), 1, bfile);
            }
            fclose(bfile);

            Superblock recuperado;
            FILE *archivo = fopen(path.c_str(), "rb");
            fseek(archivo, partition.part_start, SEEK_SET);
            fread(&recuperado, sizeof(Superblock), 1, archivo);
            fclose(archivo);

            inode.i_uid = 1;
            inode.i_gid = 1;
            inode.i_s = 0;
            inode.i_atime = spr.s_umtime;
            inode.i_ctime = spr.s_umtime;
            inode.i_mtime = spr.s_umtime;
            inode.i_type = 0;
            inode.i_perm = 664;
            inode.i_block[0] = 0;

            strcpy(journaling.content, "carpeta root");
            strcpy(journaling.path, "/");
            journaling.type = 0;
            journaling.size = sizeof(Folderblock);
            strcpy(journaling.operation, "mkdir");
            journaling.date = spr.s_umtime;

            Folderblock fb;
            strcpy(fb.b_content[0].b_name, ".");
            fb.b_content[0].b_inodo = 0;
            strcpy(fb.b_content[1].b_name, "..");
            fb.b_content[1].b_inodo = 0;
            strcpy(fb.b_content[2].b_name, "users.txt");
            fb.b_content[2].b_inodo = 1;

            string data = "1,G,root\n1,U,root,root,123\n";
            Inodes inodetmp;
            inodetmp.i_uid = 1;
            inodetmp.i_gid = 1;
            inodetmp.i_s = sizeof(data.c_str()) + sizeof(Folderblock);
            inodetmp.i_atime = spr.s_umtime;
            inodetmp.i_ctime = spr.s_umtime;
            inodetmp.i_mtime = spr.s_umtime;
            inodetmp.i_block[0] = 1;
            inodetmp.i_type = 1;
            inodetmp.i_perm = 664;
            

            inode.i_s = inodetmp.i_s + sizeof(Folderblock) + sizeof(Inodes);

            Journaling joutmp;
            strcpy(joutmp.content, data.c_str());
            strcpy(joutmp.path, "/users.txt");
            joutmp.type = 1;
            joutmp.size = sizeof(data) + sizeof(Folderblock);
            strcpy(joutmp.operation, "mkfile");
            joutmp.date = spr.s_umtime;

            Fileblock fileb;
            strcpy(fileb.b_content, data.c_str());

            FILE *bfiles = fopen(path.c_str(), "rb+");

            fseek(bfiles, partition.part_start + sizeof(Superblock), SEEK_SET);
            fwrite(&journaling, sizeof(Journaling), 1, bfiles);
            fwrite(&joutmp, sizeof(Journaling), 1, bfiles);

            fseek(bfiles, spr.s_bm_inode_start, SEEK_SET);
            char caracter = '1';
            fwrite(&caracter, sizeof(caracter), 1, bfiles);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);

            fseek(bfiles, spr.s_bm_block_start, SEEK_SET);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);
            fwrite(&caracter, sizeof(caracter), 1, bfiles);

            fseek(bfiles, spr.s_inode_start, SEEK_SET);
            fwrite(&inode, sizeof(Inodes), 1, bfiles);
            fwrite(&inodetmp, sizeof(Inodes), 1, bfiles);

            fseek(bfiles, spr.s_block_start, SEEK_SET);
            fwrite(&fb, sizeof(Folderblock), 1, bfiles);
            fwrite(&fileb, sizeof(Fileblock), 1, bfiles);
            fclose(bfiles);
        }

        cout << "MKFS formateo realizado con éxito" << endl;
    }
    catch (invalid_argument &e) {
        cout << "identificador de disco incorrecto" << endl;
        return;
    }
    catch (exception &e) {
        cout << e.what() << endl;
        return;
    }
}

//funcion para obetener las particiones montadas
Partition disk::get_mount(string id, string *path) {
    for (int i = 0; i < 99; i++){
        for (int j = 0; j < 99; j++) {
            if (mountd[i].mounted_p[j].status == '1') {
                if (mountd[i].mounted_p[j].id == id) {
                    FILE *validate = fopen(mountd[i].path, "r");
                    if (validate == NULL) {
                        throw runtime_error("disco no existente");
                    }
                    MBR disk;
                    rewind(validate);
                    fread(&disk, sizeof(MBR), 1, validate);
                    fclose(validate);
                    *path = mountd[i].path;
                    return tracker(disk, mountd[i].mounted_p[j].name, mountd[i].path);
                }
            }
        }
    }
    throw runtime_error("id no existente ");

}

//funcion para obtener particion
Partition disk::tracker(MBR mbr, string name, string path) {
    Partition partitions[4];
    partitions[0] = mbr.mbr_Partition_1;
    partitions[1] = mbr.mbr_Partition_2;
    partitions[2] = mbr.mbr_Partition_3;
    partitions[3] = mbr.mbr_Partition_4;

    for (auto &partition : partitions) {
        if (partition.part_status == '1') {
            if (lower(partition.part_name) == lower(name)) {
                return partition;
            } else if (partition.part_type == 'E') {
                vector<EBR> ebrs = get_type_logic(partition, path);
                for (EBR ebr : ebrs) {
                    if (ebr.part_status == '1') {
                        if (lower(ebr.part_name) == lower(name)) {
                            Partition tmp;
                            tmp.part_status = '1';
                            tmp.part_type = 'l';
                            tmp.part_fit = ebr.part_fit;
                            tmp.part_start = ebr.part_start;
                            tmp.part_s = ebr.part_s;
                            strcpy(tmp.part_name, ebr.part_name);
                            return tmp;
                        }
                    }
                }
            }
        }
    }
    throw runtime_error("la partición no existe");
}

//funcion para obtener particiones logicas
vector<EBR> disk::get_type_logic(Partition partition, string path) {
    vector<EBR> ebrs;

    FILE* file = fopen(path.c_str(), "rb+");
    if (!file) {
        return ebrs;
    }

    fseek(file, partition.part_start, SEEK_SET);
    EBR tmp;

    while (fread(&tmp, sizeof(EBR), 1, file) == 1) {
        if (tmp.part_status != '0') {
            ebrs.push_back(tmp);
        }
        if (tmp.part_next == -1) {
            break;
        }
        fseek(file, tmp.part_next, SEEK_SET);
    }
    fclose(file);
    return ebrs;
}

//Funcion para convertir a minusculas
string disk::lower(string String){
    string aux;
    locale loc;
    for (int i = 0; i < String.length(); i++){
        aux += tolower(String[i], loc);
    }
    return aux;
}

//funcion para mostrar particiones montadas
void disk::show_mounted() {
    system("clear"); 
    cout << "Listado de particiones montadas" << endl;
    for (int i = 0; i < 99; i++) {
        for (int j = 0; j < 26; j++) {
            if (mountd[i].mounted_p[j].status == '1') {
                cout << "id = " << mountd[i].mounted_p[j].id << " , " << mountd[i].mounted_p[j].name << endl;
            }
        }
    }
}
