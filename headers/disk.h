
#ifndef DISK_H
#define DISK_H


#include <string>
#include<vector>


using namespace std;

struct Partition {
    char part_status = '0';
    char part_type;
    char part_fit;
    int part_start = -1;
    int part_s = 0;
    char part_name[16];
};

struct MBR{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_dsk_signature;
    char dsk_fit;
    Partition mbr_Partition_1;
    Partition mbr_Partition_2;
    Partition mbr_Partition_3;
    Partition mbr_Partition_4;
};

struct EBR{
    char part_status = '0';
    char part_fit;
    int part_start;
    int part_s = 0;
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


struct MPartition {
    char id[20];
    char name[20];
    char status = '0';
};

struct MDisk {
    char path[150];
    char status = '0';
    MPartition mounted_p[99];
};

struct Content{
    char b_name[12];
    int b_inodo = -1;
} ;

struct Folderblock{
    Content b_content[4] = {{"", -1}, {"", -1}, {"", -1}, {"", -1}};
};


struct Inodes {
    int i_uid = -1;
    int i_gid = -1;
    int i_s = -1;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    char i_type = -1;
    int i_perm = -1;
};


struct Superblock {
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic = 0xEF53;
    int s_inode_s = sizeof(Inodes);
    int s_block_s = sizeof(Folderblock);
    int s_first_ino = 0;
    int s_first_blo = 0;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
};

struct Fileblock{
    char b_content[64];
};

struct Pointerblock{
    int b_pointers[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
};

struct Journaling {
    char operation[10] = "";
    char type = -1;
    char path[100] = "";
    char content[60] = "";
    time_t date;
    int size = 0;
};

class disk{

    public:

        void mkdisk(vector<string> tks);
        void rmdisk(vector<string> tks);
        void fdisk(vector<string> tks);
        void mount(vector<string> tks);
        void unmount(vector<string> tks);
        void mkfs(vector<string> tks);
        
        Partition get_mount(string id, string *path);
        vector<EBR> get_type_logic(Partition partition, string path);
        Partition tracker(MBR mbr, string name, string path);
    
    private:
        string lower(string str);
        void show_mounted();
        MDisk mountd[99];
        int aux_value;

};

#endif // DISK_H