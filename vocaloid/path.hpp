#pragma once
#include <string>
#include <regex>
using namespace std;

string GetExtension(const string path){
    auto last_dot_index = path.find_last_of(".");
    if(last_dot_index < 0)return path;
    return path.substr(last_dot_index, path.length() - last_dot_index + 1);
}

string GetFileName(const string path){
    auto last_sub_path_index = path.find_last_of("\\");
    if(last_sub_path_index < 0)return path;
    return path.substr(last_sub_path_index + 1, path.length() - last_sub_path_index);
}

string GetSubDirectory(const string path){
    auto last_sub_path_index = path.find_last_of("\\");
    if(last_sub_path_index < 0)return path;
    return path.substr(0, last_sub_path_index);
}

string CombinePath(const string path, const string child_path){
    auto first_p = child_path.find_first_of("\\");
    auto last_p = path.find_last_of("\\");
    string output = path;
    if(first_p > 0){
        if((first_p == 2 && child_path.substr(0, first_p + 1) == "..") ||
            (first_p == 1 && child_path.substr(0, first_p + 1) == ".")){
            if(last_p == path.length() - 1){
                output = path.substr(0, last_p);
            }
            output += child_path.substr(first_p + 1, child_path.length() - first_p);
            return output;
        } else{
            if(last_p == path.length() - 1)return path + child_path;
            else return path + "\\" + child_path;
        }
    }else if(first_p == 0){
        if(last_p == path.length() - 1)
            return path + child_path.substr(first_p + 1, child_path.length() - first_p);
        return path + child_path;
    }else{
        if(last_p == path.length() - 1)
            return path + child_path;
        return path + "\\" + child_path;
    }
}