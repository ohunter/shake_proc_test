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
        uint64_t id = 0;

        /**
         * @brief Szudzik's pairing function for unsigned integers used for calculating a unique ID from composite lines
         * 
         * @param x The first integer
         * @param y The second integer
         * @return uint64_t The unique ID
         */
        uint64_t Szudzik(uint x, uint y) {
                if (x != std::max(x, y))
                        return y * y + x;

                return x * x + x + y;
        }

        public:
                std::string file_name = "";
                mutable uint line_num = 0;
                mutable std::string content = "";

                mutable std::string checksum = "";
                mutable std::map<std::string, uint> count;

                /**
                 * @brief Construct a new line object
                 * 
                 * @param fn The file name which the line belongs to
                 * @param i First part of the ID used to represent them in a set
                 * @param j Second part of the ID used to represent them in a set
                 */
                line(std::string fn, uint i, uint j = 0) : id(Szudzik(j, i)), file_name(fn) {}

                /**
                 * @brief Takes the content of the class and outputs a JSON formatted string
                 * 
                 * @param indentation the number of spaces to indent with
                 * @return std::string A JSON representation of the class
                 */
                std::string jsonify(int indentation = 0)
                {
                        std::stringstream ss("");
                        std::string indent(indentation, ' ');

                        ss << indent << "\"file\": \"" << this->file_name << "\"," << std::endl;

                        if (this->line_num != 0)
                                ss << indent << "\"line\":" << this->line_num << "," << std::endl;

                        if (this->content != "")
                                ss << indent << "\"text\": \"" << this->content << "\"," << std::endl;

                        if (this->checksum != "")
                                ss << indent << "\"md5sum\": \"" << this->checksum << "\"," << std::endl;

                        if (this->count.size() > 0)
                        {
                                ss << indent << "\"word_counts\": {" << std::endl;
                                for (auto it = this->count.begin(); it != this->count.end(); it++)
                                {
                                        std::string trail = std::next(it) != this->count.end() ? "," : "";
                                        ss << indent << "  \"" << it->first << "\":" << it->second << trail << std::endl;
                                }
                                ss << indent << "}" << std::endl;
                        }

                        return ss.str();
                }

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
                                std::sprintf(buf + i*2, "%02x", md[i]);

                        this->checksum = std::string(buf);

                        delete[] md;
                }

                /**
                 * @brief Merges two line objects
                 * 
                 * @param other The other line object
                 * @return line A new line with the contents of both operands
                 */
                line operator+ (const line& other) const
                {
                        line l(this->file_name, this->id, other.id);

                        std::map<std::string, uint>::const_iterator m1 = this->count.begin();
                        std::map<std::string, uint>::const_iterator m2 = other.count.begin();

                        std::map<std::string, uint>::iterator m3 = l.count.begin();

                        while (m1 != this->count.end() || m2 != other.count.end())
                        {
                                if (m1 != this->count.end() && m2 != other.count.end())
                                {
                                        if (m1->first < m2->first)
                                        {
                                                l.count.emplace_hint(m3, *m1);
                                                m1++;
                                        }
                                        else if (m1->first > m2->first)
                                        {
                                                l.count.emplace_hint(m3, *m2);
                                                m2++;
                                        }
                                        else
                                        {
                                                l.count.emplace_hint(m3, m1->first, m1->second + m2->second);
                                                m1++;
                                                m2++;
                                        }
                                        
                                }
                                else if (m1 != this->count.end())
                                {
                                        l.count.emplace_hint(m3, *m1);
                                        m1++;
                                }
                                else
                                {
                                        l.count.emplace_hint(m3, *m2);
                                        m2++;
                                }

                                m3++;
                        }

                        return l;
                }

                /**
                 * @brief Merges two lines inplace
                 * 
                 * @param other The object which is being merged in
                 * @return line& A reference to the newly merged object
                 */
                line& operator+= (const line& other)
                {
                        std::map<std::string, uint>::const_iterator m1 = this->count.begin();
                        std::map<std::string, uint>::const_iterator m2 = other.count.begin();
                        
                        while (m2 != other.count.end())
                        {
                                if (m1->first < m2->first)
                                        m1++;
                                else if (m1->first > m2->first)
                                {
                                        this->count.emplace_hint(m1, *m2);
                                        m2++;
                                }
                                else
                                {
                                        this->count[m1->first] += m2->second;
                                        m1++;
                                        m2++;
                                }
                        }

                        return *this;
                }

                /**
                 * @brief Computes whether a line is less than another through their IDs. Their IDs are guarunteed to be unique as long as their line numbers are unique through the pairing function defined above.
                 * 
                 * @param rhs The other line 
                 * @return true This is less than the other line
                 * @return false This is greater or equal than the other line
                 */
                bool operator< (const line& rhs) const
                {
                        return this->id < rhs.id;
                }
};