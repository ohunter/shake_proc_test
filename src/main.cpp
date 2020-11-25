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

/**
 * @brief Performs the preprocess step.
 * 
 * @param path The path to the file
 * @param lines A set that will contain all the different lines from the file
 * @return int 0 on success, -1 on failure.
 */
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

/**
 * @brief Performs the processing stage
 * 
 * @param lines The set of lines from the file
 */
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

/**
 * @brief Performs the postprocessing stage in a sequential manner
 * 
 * @param start The start of the sequence the function will be operating on
 * @param end The end of the sequence the function will be operating on
 * @param true_end Whether the last element is valid or not
 * @param dist The distance between start and end
 * @return line the line representing all the different lines in the sequence but merged together
 */
line postprocess_seq(std::set<line>::iterator start, std::set<line>::iterator end, bool true_end, int dist = 1)
{
        if (dist == 1 && true_end)
                return *start;
        else if (dist == 1)
                return *start + *end;
        else
                return postprocess_seq(start, std::next(start, dist / 2), false, dist / 2) + postprocess_seq(std::next(start, dist / 2), end, true_end, dist / 2);
}

#if defined(_OPENMP)
/**
 * @brief Performs the postprocessing stage in a parallelized manner
 *
 * @param start The start of the sequence the function will be operating on
 * @param end The end of the sequence the function will be operating on
 * @param true_end Whether the last element is valid or not
 * @param dist The distance between start and end
 * @return line the line representing all the different lines in the sequence but merged together
 */
line postprocess_par(std::set<line>::iterator start, std::set<line>::iterator end, bool true_end, int dist = 1)
{

        if (dist <= 65536)
                return postprocess_seq(start, end, true_end, dist);

        line l1("", 0);
        line l2("", 0);

#pragma omp task shared(l1)
        {l1 = postprocess_par(start, std::next(start, dist / 2), false, dist / 2);}

#pragma omp task shared(l2)
        {l2 = postprocess_par(std::next(start, dist / 2), end, true_end, dist / 2);}

#pragma omp taskwait
        return l1 + l2;
}
#endif

/**
 * @brief Performs the postprocessing stage choosing between a parallel or sequential evaluation method depending on the libraries linked
 * 
 * @param start The start of the sequence the function will be operating on
 * @param end The end of the sequence the function will be operating on
 * @param true_end Whether the last element is valid or not
 * @return line the line representing all the different lines in the sequence but merged together
 */
line postprocess(std::set<line>::iterator start, std::set<line>::iterator end, bool true_end)
{
        int dist = std::distance(start, end);

#if defined(_OPENMP)
        return postprocess_par(start, end, true_end, dist);
#else
        return postprocess_seq(start, end, true_end, dist);
#endif
}


int main(int argc, char** argv)
{
        std::ofstream out;
        std::set<line> lines;
        line l(argv[1], 0);

        if (preprocess(argv[1], &lines) != 0)
        {
                std::cout << "Error in processing file" << std::endl;
                return -1;
        }
        #if defined(_OPENMP)
        #pragma omp parallel num_threads(12)
        #pragma omp single
        #endif
        {
                process(&lines);
                l = postprocess(lines.begin(), lines.end(), true);
        }

        out.open("out.json");
        out << "{\n" << l.jsonify(2) << "}\n";
        out.close();

}