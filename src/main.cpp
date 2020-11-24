// STL C++ Libraries
#include <iostream>
#include <fstream>
#include <set>
#include <numeric>

// STL C Wrappers


// LIBC Headers


// Foreign Dependencies
#include <omp.h>

// Local headers
#include "../include/line.hpp"

// Local definitions

int preprocess(std::string path, std::set<line> *lines)
{
        std::ifstream file(path, std::ios::in);
        std::string s;
        int i = 1;

        if (!file.is_open())
        {
                std::cerr << "Could not open file " << path << std::endl;
                return -1;
        }

        while (std::getline(file, s))
        {
                line l(path, i);
                l.line_num = i;
                l.content = s;

                lines->emplace(l);

                i++;
        }
        return 0;
}

void process(std::set<line>* lines)
{

#if defined(_OPENMP)
        int dist = std::distance(lines->begin(), lines->end());

        #pragma omp parallel for
                for (int i = 0; i < dist; i++)
                        std::next(lines->begin(), i)->process();
#else
        for (std::set<line>::iterator it = lines->begin(); it != lines->end(); it++)
                it->process();
#endif
}

line postprocess(std::set<line>::iterator start, std::set<line>::iterator end, bool true_end)
{
        int dist = std::distance(start, end);

        if (dist == 1 && true_end)
                return *start;
        else if (dist == 1)
                return *start + *end;
        else
        {
        #if defined(_OPENMP)
                line l1("", 0);
                line l2("", 0);

                #pragma omp task shared(l1)
                l1 = postprocess(start, std::next(start, dist / 2), false);

                #pragma omp task shared(l2)
                l2 = postprocess(std::next(start, dist / 2), end, true_end);

                #pragma omp taskwait
                return l1 + l2;
        #else
                return postprocess(start, std::next(start, dist / 2), false) + postprocess(std::next(start, dist / 2), end, true_end);
        #endif
        }
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
        line l = postprocess(lines.begin(), lines.end(), true);
        std::cout << "{\n" << l.jsonify(2) << "}\n";

}