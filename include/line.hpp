#pragma once

// STL C++ Libraries
#include <iostream>
#include <map>
#include <sstream>
#include <regex>

// STL C Wrappers


// LIBC Headers


// Foreign Dependencies
#include <openssl/md5.h>

// Local headers


// Local definitions


/**
 * @brief A class which represents a single line in a file, while also allowing for processing of the content.
 *
 */
class line
{
public:
        std::string file_name = "";
        mutable uint line_num = 0;
        mutable std::string content = "";

        mutable std::string checksum = "";
        mutable std::map<std::string, size_t> count;

        /**
         * @brief Construct a new line object
         *
         * @param fn The file name which the line belongs to
         * @param i First part of the ID used to represent them in a set
         * @param j Second part of the ID used to represent them in a set
         */
        line(std::string fn) : file_name(fn) {}

        /**
         * @brief Performs the processing stage for a given line.
         *
         */
        void process() const
        {
                static std::regex r("\\b\\w+(\\S\\w+)?\\b");

                unsigned char* md = new unsigned char[MD5_DIGEST_LENGTH];
                char buf[2 * MD5_DIGEST_LENGTH] = {};

                auto it = std::sregex_iterator(this->content.begin(), this->content.end(), r);
                while (it != std::sregex_iterator())
                {
                        this->count[it->str(0)] += 1;
                        it++;
                }

                MD5(reinterpret_cast<const unsigned char*>(this->content.c_str()), this->content.size(), md);

                for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
                        std::sprintf(buf + i * 2, "%02x", md[i]);

                this->checksum = std::string(buf);

                delete[] md;
        }
};