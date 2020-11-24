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

class line
{
        uint64_t id = 0;

        //Szudzik's pairing function for unsigned integers
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

                line(std::string fn, uint i, uint j = 0) : id(Szudzik(j, i)), file_name(fn) {}

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
                                for (const auto& it : this->count)
                                        ss << indent << "  \"" << it.first << "\":" << it.second << "," << std::endl;
                                ss << indent << "}" << std::endl;
                        }

                        return ss.str();
                }

                void process() const
                {
                        static std::regex r("\\b\\w+\\b");

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

                bool operator< (const line& rhs) const
                {
                        return this->id < rhs.id;
                }
};