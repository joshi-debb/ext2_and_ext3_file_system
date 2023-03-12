
#ifndef MANAGER_H
#define MANAGER_H

#include "../headers/disk.h"
#include "../headers/user.h"

#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <cstdio>

using namespace std;

class manager{
    public:
        void cmd();

    private:
        void make_report(vector<string> tks);
        void mbr_report(string rep_path, string bin_path, string file_dot, string date, MBR disco, Partition partitions[4]);
        void dsk_report(string rep_path, string bin_path, string file_dot, MBR disco, Partition partitions[4]);
        void ino_report(string rep_path, FILE &file, string file_dot, Superblock super, Inodes inodes, int free_inodes);
        void blk_report(string rep_path, FILE &file, string file_dot, Superblock super, Inodes inodes, int free_blocks);
        void bmi_report(string rep_path, FILE &file, string file_txt, Superblock super);
        void bmb_report(string rep_path, FILE &file, string file_txt, Superblock super);
        void tre_report(string rep_path, FILE &file, string file_dot, Superblock super, Inodes inodes, int free_inodes);
        void spr_report(string path_rep, string path_dot, Superblock super);
        void search(string tk,vector<string> tks);
        void execute(vector<string> tks);
        string lower(string str);
        string token(string text);              
        vector<string> split_tokens(string text);
        void pause();
        disk Disk;
        user User;
};

#endif // MANAGER_H