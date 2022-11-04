//
// Created by creeper on 22-11-1.
//
#include <string>
#include <iostream>
#include <fstream>

void parse_xml(const std::string &path)
{
    std::fstream fin(path);
    std::string str;
    char ch;
    while(fin >> ch)
    {
        bool is_close = false;
        if(ch == '<')
        {
            fin >> ch;
            if(ch == '/')
                is_close = true;
            else
            {
                str += ch;
                while(fin >> ch && ch != '>') str += ch;
                if(str[str.length() - 1] != '>')
                {
                    std::cerr << "Failed to parse the scene description file: please check the format." << std::endl;
                    return ;
                }
            }
        }
        else
        {
            std::cerr << "Failed to parse the scene description file: please check the format." << std::endl;
            return ;
        }
    }
}
