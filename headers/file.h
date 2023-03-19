
#ifndef FILE_H
#define FILE_H

#include "../headers/disk.h"
#include "../headers/user.h" 

#include <string>
#include <bits/stdc++.h>

using namespace std;

class file {

    public:
        void mkdir(vector<string> tks, user Usr, disk Dsk);
        void mkfile(vector<string> tks, user Usr, disk Dsk);

        void make_dir_file(vector<string> tmp, bool r, Partition partition, string file_path, string new_file, string contenido);
        string lower(string String);
        vector<string> split_path(string s);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    private:
        disk Disk;
        user User;

};


#endif // FILE_H