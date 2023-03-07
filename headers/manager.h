
#ifndef MANAGER_H
#define MANAGER_H

#include "../headers/disk.h"
#include "../headers/user.h"

#include <string>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

class manager{
    public:
        void cmd();

    private:
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