#include <iostream>
#include <fstream>
#include "mymesh.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;


void check_manifold(std::string &body_path, std::string &output_path)
{
    std::ofstream output_file;
    output_file.open(output_path);
    output_file << "organ,mesh,2-manifold,closed,self-intersection,watertight\n";

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
                std::cout << organ_name << " " << name << " is not 2-manifold" << std::endl;
                output_file << organ_name << "," << name << "," << "No,-,-\n";
            } 
            else
            {
                Surface_mesh &raw_mesh = m.get_raw_mesh();

                bool is_self_intersected = PMP::does_self_intersect(raw_mesh);
                bool is_closed = CGAL::is_closed(raw_mesh);
                
                output_file << organ_name << "," << name << ",Yes,";
                if (!is_closed) 
                {
                    std::cout << organ_name << " " << name << " is not closed" << std::endl;
                    output_file << "No,";
                }
                else
                {
                    output_file << "Yes,"; 
                }

                if (is_self_intersected)
                {
                    std::cout << organ_name << " " << name << " has-intersections";
                    output_file << "Yes,";
                }
                else
                {
                    output_file << "No,";
                }

                if (!is_self_intersected && is_closed) output_file << "Yes\n";
                else output_file << "No\n";               

            }
        }
    }
}



int main(int argc, char* argv[])
{
    // std::string body_path = "/home/catherine/data/model/plain_with_holes/";
    if (argc < 3)
    {
        std::cout << "Please provide body_path and output_file name (csv file)!" << std::endl;
        return 0;
    }

    std::string body_path = std::string(argv[1]);
    std::string output_file = std::string(argv[2]);

    check_manifold(body_path, output_file);
    std::cout << "end" << std::endl;
    return 0;
}