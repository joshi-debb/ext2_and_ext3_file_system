
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
};

struct MBR
{
    int mbr_tamano;
    char mbr_fecha_creacion[20];
    int mbr_disk_signature;
    char disk_fit;
    Partition mbr_Partition_1;
    Partition mbr_Partition_2;
    Partition mbr_Partition_3;
    Partition mbr_Partition_4;
};

struct EBR {
    char part_status = '0';
    char part_fit;
    int part_start;
    int part_size = 0;
    int part_next = -1;
    char part_name[24];
};

struct Stepper {
    int partition;
    int start;
    int end;
    int before;
    int after;
};


using namespace std;

class disk{

    public:

        void mkdisk(vector<string> tks);
        void fdisk(vector<string> tks);
        Partition tracker(MBR mbr, string name, string path);
        vector<EBR> get_type_logic(Partition partition, string path);
        void rep();

};

#endif // DISK_H