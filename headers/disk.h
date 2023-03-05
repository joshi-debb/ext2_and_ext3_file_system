
#ifndef DISK_H
#define DISK_H

#include "../headers/disk.h"
#include "../headers/manager.h"

#include<vector>
#include <string>
#include <algorithm>


struct Partition {
    char part_status = '0';
    char part_type;
    char part_fit;
    int part_start = -1;
    int part_size = 0;
    char part_name[16];
    int part_number = -1;
};

struct MBR{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    char disk_fit;
    Partition mbr_Partition_1;
    Partition mbr_Partition_2;
    Partition mbr_Partition_3;
    Partition mbr_Partition_4;
};

struct EBR{
    char part_status = '0';
    char part_fit;
    int part_start;
    int part_size = 0;
    int part_next = -1; 
    char part_name[16];
};

struct Stepper {
    int partition;
    int start;
    int end;
    int before;
    int after;
};

struct Mounted_P{
    char id[20];
    char name[20];
    char status = '0';
};

struct Mounted_D{
    char path[150];
    char status = '0';
    Mounted_P mounted_p[99];
};

using namespace std;

class disk{

    public:

        void mkdisk(vector<string> tks);
        void rmdisk(vector<string> tks);
        void fdisk(vector<string> tks);
        void mount(vector<string> tks);
        void unmount(vector<string> tks);
        void mounted();
        void rep();
        Partition tracker(MBR mbr, string name, string path);
        vector<EBR> get_type_logic(Partition partition, string path);

};

#endif // DISK_H