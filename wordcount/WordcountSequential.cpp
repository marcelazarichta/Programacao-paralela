#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <bits/stdc++.h>
#include <omp.h>
#include <algorithm>

using namespace std;

void word_count(string path)
{
    vector<string> words;
    unordered_map<string, int> dict;
    fstream file;
    int zero=0;
    int count=0;
    string w;

    file.open(path, ios::in);
    if (file.is_open()){
        string word;
        while(getline(file, word, ' ')){
            // Remove all instances of '\n'
            word.erase(remove(word.begin(), word.end(), '\n'), word.end());
            words.push_back(word);
        }
        file.close();
    }

    int i = 0;
    for(auto c : words){
        cout << i++ << " "<< c << endl;
    }

    for (int i = 0; i < words.size(); i++)
    {
        w = words[i];
        dict[w]+= 1;
    }

    w = "a";
    cout << dict[w] << endl;
}

int main(int argc, char *argv[])
{
    string path = argv[1];
    word_count(path);
    return 0;
}