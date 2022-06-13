#include "algo.h"

std::vector<std::pair<std::string, double>> collision_detection_single_tissue(std::vector<Mymesh> &organ, Mymesh &tissue, std::vector<Point> &points)
{
    auto aabbtree_tissue = tissue.get_aabb_tree();
    std::vector<std::pair<std::string, double>> result;


    for (int i = 0; i < organ.size(); i++)
    {
        auto AS = organ[i];
        
        if (!AS.is_surface) continue;

        auto aabbtree_AS = AS.get_aabb_tree();
        if (aabbtree_AS->do_intersect(*aabbtree_tissue))
        {

            if (AS.is_closed) 
            {
                double percentage = AS.percentage_points_inside(points);
                result.push_back({AS.label, percentage});
            }
            else
                result.push_back({AS.label, -1.0});
        }
        else
        {

            Surface_mesh &mesh = tissue.get_raw_mesh();
            
            bool is_contain = true;
            for (auto vd: mesh.vertices())
            {
                Point p = mesh.point(vd);
                if (!AS.point_inside(p)) 
                {
                    is_contain = false; 
                    break;
                }
            }

            if (is_contain) result.push_back({AS.label, 1.0});
        }
    }

    return result;
}