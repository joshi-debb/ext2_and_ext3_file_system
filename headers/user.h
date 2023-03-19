
#ifndef USER_H
#define USER_H

#include "../headers/disk.h"

#include <string>
#include <vector>

using namespace std;

struct User {
    string user;
    string password;
    string id;
    int uid;
};

class user{

    public:

        bool logged_in = false;
        User active_user;

        void login(vector<string> tks, disk Disk);
        void logout();
        void mkgrp(vector<string> tks);
        void rmgrp(vector<string> tks);
        void mkusr(vector<string> tks);
        void rmusr(vector<string> tks);


    private:
        bool user_exists(string user);
        bool group_exists(string group);
        void show_datas_txt();
        vector<string> get_data(string txt, char c);
        string lower(string str);
        Superblock super;
        Fileblock fb;
        disk Disk;
};





#endif // USER_H