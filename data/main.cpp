#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <thread>
#include <algorithm>

void tokenize(std::string const &str, const char* delim, std::vector<std::string> &out) 
{ 
    char *next_token = nullptr;
    char *token = strtok(const_cast<char*>(str.c_str()), delim); 
    while (token != nullptr) 
    { 
        out.push_back(std::string(token)); 
        token = strtok(nullptr, delim); 
    } 
}


int main(int argc, char **argv)
{
    std::ifstream data_file(argv[1]);
    std::ofstream bin_data_file(argv[2], std::ios::binary);

    if(!data_file) { std::cerr << "Unable to open file" << std::endl; }

    std::vector<std::string> parsed_line;
    parsed_line.reserve(6);
    int i = 0;
    float x_min, x_max, y_min, y_max, z_min, z_max;
    x_min = y_min = z_min = 10000.0f;
    x_max = y_max = z_max = -10000.0f;
    for(std::string line; std::getline(data_file, line);)
    {
        if(!(i++ % (512 * 512)))
        {
            std::cout << i /(512 * 512) << std::endl;
        }
        parsed_line.clear();
        tokenize(line,",", parsed_line);

        assert(parsed_line.size() == 6);
        for(int j = 0; j < 3; j++)
        {
            float token = std::stof(parsed_line.at(j));
            if(j == 0) {
                x_min = std::min(x_min, token);
                x_max = std::max(x_max, token);
            }
            else if(j == 1) {
                y_min = std::min(y_min, token);
                y_max = std::max(y_max, token);
            }
            else if(j == 2) {
                z_min = std::min(z_min, token);
                z_max = std::max(z_max, token);
            }
        }
        for(int j = 3; j < 6; j++)
        {
            float token = std::stof(parsed_line.at(j));
            bin_data_file.write((char*)&token, sizeof(float));
        }
    }
    std::cout << "x_min: " << x_min << " x_max: " << x_max << std::endl;
    std::cout << "y_min: " << y_min << " y_max: " << y_max << std::endl;
    std::cout << "z_min: " << z_min << " z_max: " << z_max << std::endl;

    data_file.close();
    bin_data_file.close();
    return 0;
}