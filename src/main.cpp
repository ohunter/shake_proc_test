// STL C++ Libraries
#include <iostream>
#include <fstream>
#include <set>

// STL C Wrappers


// LIBC Headers


// Foreign Dependencies


// Local headers
#include "../include/line.hpp"

// Local definitions

int preprocess(std::string path, std::set<line> *lines)
{
        std::ifstream file(path, std::ios::in);
        std::string s;
        int i = 0;

        line l(path);

        if (!file.is_open())
        {
                std::cerr << "Could not open file " << path << std::endl;
                return -1;
        }

        while (std::getline(file, s))
        {
                l.line_num = i;
                l.content = s;

                lines->emplace(l);

                i++;
        }
        return 0;
}

void process(std::set<line>* lines)
{
        for (std::set<line>::iterator it = lines->begin(); it != lines->end(); it++)
        // for (auto& it : *lines)
                it->process();
        
        std::cout << "done\n";
}

line postprocess(std::set<line>* lines)
{
        line l = *(lines->begin());

        for (auto& it : *lines)
                l += it;

        return l;
}

int main(int argc, char** argv)
{
        std::set<line> lines;

        if (preprocess(argv[1], &lines) != 0)
        {
                std::cout << "Error in processing file" << std::endl;
                return -1;
        }

        process(&lines);

        std::cout << "{\n" << postprocess(&lines).jsonify() << "\n}\n";

}