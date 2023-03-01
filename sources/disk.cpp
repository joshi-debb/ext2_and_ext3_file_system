
#include "../headers/disk.h"
#include "../headers/manager.h"

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

manager Manager;

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
        if (Manager.lower(tk) == "fit"){
            if (Manager.lower(token) == "bf" || Manager.lower(token) == "ff" || Manager.lower(token) == "wf"){
                fit = Manager.lower(token);
            }else{
                cout << "Parametro fit no valido" << endl; 
            }
        }else if(Manager.lower(tk) == "unit"){
            if (Manager.lower(token)  == "k" || Manager.lower(token) == "m"){
                unit = Manager.lower(token);  
            }else{
                cout<< "Parametro unit no valido" << endl; 
            }
        }else if (Manager.lower(tk) == "size"){
            size = stoi(token);
            if( size <= 0){
                cout << "Parametro size no valido" << endl;
                return;
            }
        }else if (Manager.lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }
            //verificar extension del archivo.dsk
            if(Manager.lower(path.substr(path.find_last_of(".") + 1)) != "dsk"){
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

    //obtener fecha y hora
    time_t t = time(0);     
    struct tm *now = localtime(&t);     
    string fecha;     
    string hora;     
    string FomatoFH;     
    fecha = to_string(now->tm_mday) + '-' + to_string((now->tm_mon + 1)) + '-' + to_string((now->tm_year + 1900));     
    hora = to_string( now->tm_hour) + ':' + to_string(now->tm_min) + ':' + to_string(now->tm_sec);     
    FomatoFH = fecha + ' ' + hora;
    char fyh[100];  
    strcpy(fyh, FomatoFH.c_str());

    //inicializar disco
    fit = fit.substr(0,2);

    if (unit == "m"){
        size = 1024*1024*size;
    }else if (unit =="k"){
        size = 1024*size;
    }

    disco.mbr_tamano = size;
    strcpy(disco.mbr_fecha_creacion, fyh);
    disco.disk_fit = tolower(fit[0]);
    disco.mbr_disk_signature = rand() % 9999 + 100;

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

int startValue;

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
        if (Manager.lower(tk) == "fit"){
            if (Manager.lower(token) == "bf" || Manager.lower(token) == "ff" || Manager.lower(token) == "wf"){
                fit = Manager.lower(token);
            }else{
                cout << "Parametro fit no valido" << endl; 
            }
            cout << "fit: " << fit << endl;
        }else if(Manager.lower(tk) == "unit"){
            if (Manager.lower(token) == "b" || Manager.lower(token)  == "k" || Manager.lower(token) == "m"){
                unit = Manager.lower(token);  
            }else{
                cout<< "Parametro unit no valido" << endl; 
            }
            
            cout << "unit: " << unit << endl;
        }else if (Manager.lower(tk) == "size"){
            size = stoi(token);
            if( size <= 0){
                cout << "Parametro size no valido" << endl;
                return;
            }
            cout << "size: " << size << endl;
        }else if (Manager.lower(tk) == "path"){
            //si trae comillas extraerlas
            if (token.substr(0, 1) == "\""){
                path = token.substr(1, token.length() - 2);
            }else{
                path = token;
            }

            cout << "path: " << path << endl;
        }else if (Manager.lower(tk) == "type") {
            if (Manager.lower(token) == "p" || Manager.lower(token)  == "e" || Manager.lower(token) == "l"){
                type = Manager.lower(token);
                cout << "type: " << type << endl;
            }else{
                cout<< "Parametro type no valido" << endl; 
            }
        } else if (Manager.lower(tk) == "name") {
            if (token.substr(0, 1) == "\""){
                name = token.substr(1, token.length() - 2);
            }else{
                name = token;
            }
            cout << "name: " << name << endl;
        } else if (Manager.lower(tk) == "add") {
            add = stoi(token);
            cout << "add: " << add << endl;
        }else if (Manager.lower(tk) == "delete") {
            if (Manager.lower(token) == "full"){
                suprim = Manager.lower(token);  
            }else{
                cout<< "Parametro delete no valido" << endl; 
            }
            cout << "delete: " << suprim << endl;
        }else{
            cout << "No se esperaba el parametro: " << tk << endl;
        }
            
    }

    if (suprim != "full") {

        if (add == 0) {
           
            try{

                startValue = 0;

                if (unit == "m"){
                    size = 1024*1024*size;
                }else if (unit =="k"){
                    size = 1024*size;
                }else if (unit == "b"){
                    size = size;
                }

                MBR disco;

                FILE *file = fopen(path.c_str(), "rb+");
                if(file!=NULL){
                    rewind(file);
                    fread(&disco, sizeof(disco),1,file);
                }else{
                    cout << "No se encontro el disco" << endl;
                    return;
                }
                fclose(file);

                vector<Partition> partitions;
                partitions.push_back(disco.mbr_Partition_1);
                partitions.push_back(disco.mbr_Partition_2);
                partitions.push_back(disco.mbr_Partition_3);
                partitions.push_back(disco.mbr_Partition_4);

                vector<Stepper> between;

                int used = 0, ext = 0, control = 1, base = sizeof(disco);

                Partition extended;
                for(Partition prttn: partitions){
                    if(prttn.part_status == '1'){
                        Stepper trn;
                        trn.partition = control;
                        trn.start = prttn.part_start;
                        trn.end = prttn.part_start + prttn.part_size;

                        trn.before = trn.start - base;
                        base = trn.end;
                        
                        if(used !=0){
                            between.at(used-1).after = trn.start - (between.at(used-1).end);
                        }
                        between.push_back(trn);
                        used++;

                        if(prttn.part_type == 'e' || prttn.part_type == 'E'){
                            ext++;
                            extended = prttn;
                        }
                    }
                    if(used == 4 && Manager.lower(type) != "l" ){
                        throw runtime_error("Limite de particiones alcanzado");
                    }else if(ext==1 && Manager.lower(type) == "e"){
                        throw runtime_error("Solo se puede crear una particion Extendida");
                    }
                    control++;
                }

                if (ext == 0 && Manager.lower(type) == "l") {
                    throw runtime_error("No existe particion Extendida para crear la Logica");
                }
                if (used != 0) {
                    between.at(between.size() - 1).after = disco.mbr_tamano - between.at(between.size() - 1).end;
                }

                try {
                    tracker(disco, name, path);
                    cout<< ">FDISK - Este nombre ya esta en uso" << endl;
                    return;
                } catch (exception &e) {
                }

                Partition transition;
                transition.part_status = '1';
                transition.part_size = size;
                transition.part_type = toupper(type[0]);
                transition.part_fit = toupper(fit[0]);
                strcpy(transition.part_name, name.c_str());

                if (Manager.lower(type) == "l") {

                    EBR nlogic;
                    nlogic.part_status = '1';
                    nlogic.part_fit = transition.part_fit;
                    nlogic.part_size = transition.part_size;
                    nlogic.part_next = -1;
                    strcpy(nlogic.part_name, transition.part_name);

                    FILE *file = fopen(path.c_str(), "rb+");
                    rewind(file);
                    EBR tmp;
                    fseek(file, extended.part_start, SEEK_SET);
                    fread(&tmp, sizeof(EBR), 1, file);
                    int size;
                    do {
                        size += sizeof(EBR) + tmp.part_size;
                        if (tmp.part_status == '0' && tmp.part_next == -1) {
                            nlogic.part_start = tmp.part_start;
                            nlogic.part_next = nlogic.part_start + nlogic.part_size + sizeof(EBR);
                            if ((extended.part_size - size) <= nlogic.part_size) {
                                throw runtime_error("no hay espacio para más particiones lógicas");
                            }
                            fseek(file, nlogic.part_start, SEEK_SET);
                            fwrite(&nlogic, sizeof(EBR), 1, file);
                            fseek(file, nlogic.part_next, SEEK_SET);
                            EBR addLogic;
                            addLogic.part_status = '0';
                            addLogic.part_next = -1;
                            addLogic.part_start = nlogic.part_next;
                            fseek(file, addLogic.part_start, SEEK_SET);
                            fwrite(&addLogic, sizeof(EBR), 1, file);
                            cout << ">FDISK - particion logica creada correctamente" << endl;
                            fclose(file);
                            return;
                        }
                        fseek(file, tmp.part_next, SEEK_SET);
                        fread(&tmp, sizeof(EBR), 1, file);
                    } while (true);
                    return;
                }


                if (used == 0) {
                    transition.part_start = sizeof(disco);
                    startValue = transition.part_start;
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

                        if (toupper(disco.disk_fit) == 'F') {
                            if (toUse.before >= transition.part_size || toUse.after >= transition.part_size) {
                                break;
                            }
                            toUse = tr;
                        } else if (toupper(disco.disk_fit) == 'B') {
                            if (toUse.before < transition.part_size || toUse.after < transition.part_size) {
                                toUse = tr;
                            } else {
                                if (tr.before >= transition.part_size || tr.after >= transition.part_size) {
                                    int b1 = toUse.before - transition.part_size;
                                    int a1 = toUse.after - transition.part_size;
                                    int b2 = tr.before - transition.part_size;
                                    int a2 = tr.after - transition.part_size;

                                    if ((b1 < b2 && b1 < a2) || (a1 < b2 && a1 < a2)) {
                                        control++;
                                        continue;
                                    }
                                    toUse = tr;
                                }
                            }
                        } else if (toupper(disco.disk_fit) == 'W') {
                            if (!(toUse.before >= transition.part_size) || !(toUse.after >= transition.part_size)) {
                                toUse = tr;
                            } else {
                                if (tr.before >= transition.part_size || tr.after >= transition.part_size) {
                                    int b1 = toUse.before - transition.part_size;
                                    int a1 = toUse.after - transition.part_size;
                                    int b2 = tr.before - transition.part_size;
                                    int a2 = tr.after - transition.part_size;

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
                    if (toUse.before >= transition.part_size || toUse.after >= transition.part_size) {
                        if (toupper(disco.disk_fit) == 'F') {
                            if (toUse.before >= transition.part_size) {
                                transition.part_start = (toUse.start - toUse.before);
                                startValue = transition.part_start;
                            } else {
                                transition.part_start = toUse.end;
                                startValue = transition.part_start;
                            }
                        } else if (toupper(disco.disk_fit) == 'B') {
                            int b1 = toUse.before - transition.part_size;
                            int a1 = toUse.after - transition.part_size;

                            if ((toUse.before >= transition.part_size && b1 < a1) || !(toUse.after >= transition.part_start)) {
                                transition.part_start = (toUse.start - toUse.before);
                                startValue = transition.part_start;
                            } else {
                                transition.part_start = toUse.end;
                                startValue = transition.part_start;
                            }
                        } else if (toupper(disco.disk_fit) == 'W') {
                            int b1 = toUse.before - transition.part_size;
                            int a1 = toUse.after - transition.part_size;

                            if ((toUse.before >= transition.part_size && b1 > a1) || !(toUse.after >= transition.part_start)) {
                                transition.part_start = (toUse.start - toUse.before);
                                startValue = transition.part_start;
                            } else {
                                transition.part_start = toUse.end;
                                startValue = transition.part_start;
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

                FILE *bfile = fopen(path.c_str(), "rb+");
                if (bfile != NULL) {
                    fseek(bfile, 0, SEEK_SET);
                    fwrite(&disco, sizeof(MBR), 1, bfile);
                    if (Manager.lower(type) == "e") {
                        EBR ebr;
                        ebr.part_start = startValue;
                        fseek(bfile, startValue, SEEK_SET);
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
                        if (Manager.lower(partitions[i].part_name) == Manager.lower(name)) {
                            if ((partitions[i].part_size + (add)) > 0) {
                                if (i != 3) {
                                    if (partitions[i + 1].part_start != 0) {
                                        if (((partitions[i].part_size + (add) +
                                            partitions[i].part_start) <=
                                            partitions[i + 1].part_start)) {
                                            partitions[i].part_size += add;
                                            break;
                                        } else {
                                            throw runtime_error("se sobrepasa el límite");
                                        }
                                    }
                                }
                                if ((partitions[i].part_size + add + partitions[i].part_start) <= disk.mbr_tamano) {
                                    partitions[i].part_size += add;
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
                        if (Manager.lower(partitions[i].part_name) == Manager.lower(name)) {
                            past = partitions[i];
                            partitions[i] = Partition();
                            fll = true;
                            break;
                        }
                    } else {
                        if (Manager.lower(partitions[i].part_name) == Manager.lower(name)) {
                            past = partitions[i];
                            partitions[i] = Partition();
                            fll = true;
                            break;
                        }
                        vector<EBR> ebrs = get_type_logic(partitions[i], path);
                        int count = 0;
                        for (EBR ebr : ebrs) {
                            if (Manager.lower(ebr.part_name) == Manager.lower(name)) {
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
                int num = static_cast<int>(past.part_size / 2);
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

void disk::rep(){

    cout<< ">REP" << endl;

    string path = "/home/why/Desktop/Disco1.dsk";

    FILE *imprimir = fopen(path.c_str(), "r");
    if(imprimir!=NULL){

        MBR discoI;
        fseek(imprimir, 0, SEEK_SET);
        fread(&discoI,sizeof(MBR), 1,imprimir);

        cout << "   MBR" << endl;
        cout << "Tamaño del disco:  "<< discoI.mbr_tamano << endl;
        cout << "Fecha de creacion:  "<< discoI.mbr_fecha_creacion << endl;
        cout << "Disk Signature:  "<< discoI.mbr_disk_signature << endl;

        if(discoI.mbr_Partition_1.part_status == '1'){
            cout << "   Particion 1 " << endl;
            cout << " Nombre:  "<< discoI.mbr_Partition_1.part_name<<endl;
            cout << " Tipo:  "<< discoI.mbr_Partition_1.part_type<<endl;
            cout << " Tamaño:  "<< discoI.mbr_Partition_1.part_size<<endl;
            cout << " Inicio:  "<< discoI.mbr_Partition_1.part_start<<endl;
            cout << " Status: " << discoI.mbr_Partition_1.part_status<<endl;
        }

        if(discoI.mbr_Partition_2.part_status == '1'){
            cout << "   Particion 2 " << endl;
            cout << " Nombre:  "<< discoI.mbr_Partition_2.part_name<<endl;
            cout << " Tipo:  "<< discoI.mbr_Partition_2.part_type<<endl;
            cout << " Tamaño:  "<< discoI.mbr_Partition_2.part_size<<endl;
            cout << " Inicio:  "<< discoI.mbr_Partition_2.part_start<<endl;
            cout << " Status: " << discoI.mbr_Partition_2.part_status<<endl;
        }
        
        if(discoI.mbr_Partition_3.part_status == '1'){
            cout << "   Particion 3 " << endl;
            cout << " Nombre:  "<< discoI.mbr_Partition_3.part_name<<endl;
            cout << " Tipo:  "<< discoI.mbr_Partition_3.part_type<<endl;
            cout << " Tamaño:  "<< discoI.mbr_Partition_3.part_size<<endl;
            cout << " Inicio:  "<< discoI.mbr_Partition_3.part_start<<endl;
            cout << " Status: " << discoI.mbr_Partition_3.part_status<<endl;
        }

        if(discoI.mbr_Partition_4.part_status == '1'){
            cout << "   Particion 4 " << endl;
            cout << " Nombre:  "<< discoI.mbr_Partition_4.part_name<<endl;
            cout << " Tipo:  "<< discoI.mbr_Partition_4.part_type<<endl;
            cout << " Tamaño:  "<< discoI.mbr_Partition_4.part_size<<endl;
            cout << " Inicio:  "<< discoI.mbr_Partition_4.part_start<<endl;
            cout << " Status: " << discoI.mbr_Partition_4.part_status<<endl;
        }
        
    }
    fclose(imprimir);
}


Partition disk::tracker(MBR mbr, string name, string path) {
    Partition partitions[4];
    partitions[0] = mbr.mbr_Partition_1;
    partitions[1] = mbr.mbr_Partition_2;
    partitions[2] = mbr.mbr_Partition_3;
    partitions[3] = mbr.mbr_Partition_4;

    bool ext = false;
    Partition extended;
    for (auto &partition : partitions) {
        if (partition.part_status == '1') {
            if (Manager.lower(partition.part_name) == Manager.lower(name)) {
                return partition;
            } else if (partition.part_type == 'e') {
                ext = true;
                extended = partition;
            }
        }
    }
    if (ext) {
        vector<EBR> ebrs = get_type_logic(extended, path);
        for (EBR ebr : ebrs) {
            if (ebr.part_status == '1') {
                if (Manager.lower(ebr.part_name) == Manager.lower(name)) {
                    Partition tmp;
                    tmp.part_status = '1';
                    tmp.part_type = 'l';
                    tmp.part_fit = ebr.part_fit;
                    tmp.part_start = ebr.part_start;
                    tmp.part_size = ebr.part_size;
                    strcpy(tmp.part_name, ebr.part_name);
                    return tmp;
                }
            }
        }
    }
    throw runtime_error("la partición no existe");
}

vector<EBR> disk::get_type_logic(Partition partition, string p) {
    vector<EBR> ebrs;

    FILE *file = fopen(p.c_str(), "rb+");
    rewind(file);
    EBR tmp;
    fseek(file, partition.part_start, SEEK_SET);
    fread(&tmp, sizeof(EBR), 1, file);
    do {
        if (!(tmp.part_status == '0' && tmp.part_next == -1)) {
            if (tmp.part_status != '0') {
                ebrs.push_back(tmp);
            }
            fseek(file, tmp.part_next, SEEK_SET);
            fread(&tmp, sizeof(EBR), 1, file);
        } else {
            fclose(file);
            break;
        }
    } while (true);
    return ebrs;
}
