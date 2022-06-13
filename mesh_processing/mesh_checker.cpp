#include <iostream>
#include <fstream>
#include "mymesh.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;


void check_manifold(std::string body_path)
{
    std::ofstream output_file;
    output_file.open("output.csv");
    output_file << "organ,mesh,2-manifold,closed\n";

    for (directory_entry& organ_path : directory_iterator(body_path)) 
    {
        std::cout << std::endl << organ_path << std::endl;
        std::string organ_name = organ_path.path().stem().string();

        for (directory_entry& entry : directory_iterator(organ_path)) 
        {
            Mymesh m;
            std::string path = entry.path().string();
            std::string name = entry.path().stem().string();

            bool success = m.load_from_off(path);
            // std::cout << name << std::endl;
            if (!success) 
            {
                std::cout << name << " is not 2-manifold" << std::endl;
                output_file << organ_name <<"," << name << "," << "No" << "," << "-\n";
            } 
            else
            {
                bool is_closed = CGAL::is_closed(m.get_raw_mesh());
                if (!is_closed) 
                {
                    std::cout << name << " is not closed" << std::endl;
                    output_file << organ_name << "," << name <<"," << "Yes" << "," << "No\n";
                }
                else
                {
                    output_file << organ_name << "," << name << "," << "Yes" << "," <<"Yes\n"; 
                }


            }
        }
    }
}


int main()
{
    std::string body_path = "/home/luchen/plain";
    check_manifold(body_path);
    std::cout << "end" << std::endl;
    return 0;
}