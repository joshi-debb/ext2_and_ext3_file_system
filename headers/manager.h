
#ifndef MANAGER_H
#define MANAGER_H


#include <vector>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class manager{
    public:
        
        void cmd();
        void search(string tk,vector<string> tks);
        void execute(vector<string> tks);
        string lower(string str);
        string token(string text);                      
        vector<string> split_tokens(string text);
        void pause();
    
};

#endif // MANAGER_H