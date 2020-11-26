// STL C++ Libraries
#include <iostream>
#include <fstream>
#include <vector>
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
 * @param lines A vector that will contain all the different lines from the file
 * @return int 0 on success, -1 on failure.
 */
int preprocess(std::string path, std::vector<line>* lines)
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
                line l(path);
                l.line_num = i;
                l.content = s;

                lines->emplace_back(l);

                i++;
        }
        return 0;
}

/**
 * @brief Performs the processing stage
 *
 * @param lines The vector of lines from the file
 */
void process(std::vector<line>* lines)
{

#if defined(_OPENMP)
#pragma omp parallel for
#endif
        for (line& l : *lines)
                l.process();
}

/**
 * @brief Performs the postprocessing stage
 * 
 * @param lines The vector of lines
 * @return std::map<std::string, size_t> a map which maps all the words to their number of occurances.
 */
std::map<std::string, size_t> postprocess(std::vector<line>* lines)
{

        std::map<std::string, size_t> totalWordCounts;
#pragma omp parallel
        {
                std::map<std::string, size_t> threadWordCounts;

#pragma omp for schedule(static)
                for (size_t i = 0; i < lines->size(); i++)
                {
                        for (auto const& [key, val] : (*lines)[i].count)
                        {
                                if (threadWordCounts.count(key))
                                {
                                        threadWordCounts[key] += val;
                                }
                                else
                                {
                                        threadWordCounts[key] = val;
                                }
                        }
                }

#pragma omp critical
                {
                        for (auto const& [key, val] : threadWordCounts)
                        {
                                if (totalWordCounts.count(key))
                                {
                                        totalWordCounts[key] += val;
                                }
                                else
                                {
                                        totalWordCounts[key] = val;
                                }
                        }
                }
        }

        return totalWordCounts;
}

void output(std::string input_file, std::map<std::string, size_t> count)
{
        std::ofstream out;

        out.open("out.json");
        out << '{' << std::endl;
        out << "  \"file\":" << "\"" << input_file << "\"," << std::endl;
        out << "  \"word_counts\": {" << std::endl;
        for (auto it = count.begin(); it != count.end(); it++)
        {
                out << "    \"" << it->first << "\":" << it->second;

                if (std::next(it) != count.end())
                        out << ",";

                out << std::endl;
        }
        out << "  }" << std::endl;
        out << "}" << std::endl;
        out.close();
}


int main(int argc, char** argv)
{
        std::ofstream out;
        std::vector<line> lines;

        if (preprocess(argv[1], &lines) != 0)
        {
                std::cout << "Error in processing file" << std::endl;
                return -1;
        }

        process(&lines);
        auto m = postprocess(&lines);

        output(argv[1], m);

}