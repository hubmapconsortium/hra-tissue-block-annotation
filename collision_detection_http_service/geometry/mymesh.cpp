#include "mymesh.h"

namespace PMP = CGAL::Polygon_mesh_processing;
namespace fs = boost::filesystem;


bool Mymesh::load_from_off(const std::string &file_path) {
    
    std::ifstream input(file_path);

    std::string AS_name = fs::path(file_path).stem().string();

    if (!input ||!(input >> this->mesh) || this->mesh.is_empty()){
        std::cerr << file_path <<  " Not a valid mesh" << std::endl;
        this->is_surface = false;
        this->is_closed = false;
        return false;
    }

    this->label = AS_name;
    this->is_surface = true;
    if (CGAL::is_closed(this->mesh)){
        this->is_closed = true;
        this->volume = PMP::volume(this->mesh) * 1e9;
    }
    return true;

}

bool Mymesh::triangulate_mesh() {

    PMP::triangulate_faces(this->mesh);

    //confirm all faces are triangles

    for (boost::graph_traits<Surface_mesh>::face_descriptor fit : faces(mesh)) {
        
        if (next(next(halfedge(fit, mesh), mesh), mesh) 
                != prev(halfedge(fit, mesh), mesh)){
            std::cerr << "Error: non-triangular face left in mesh." << std::endl;

            return false;
        }
    }

    return true;

}

bool Mymesh::point_inside(Point& query)
{
    Point_inside inside_tester(*aabbTree);
    return inside_tester(query) == CGAL::ON_BOUNDED_SIDE;
}

double Mymesh::percentage_points_inside(std::vector<Point> &query)
{
    int cnt = 0;
    Point_inside inside_tester(*aabbTree);
    for (auto &point: query)
        if (inside_tester(point) == CGAL::ON_BOUNDED_SIDE) cnt++;
    
    double percentage = 1.0 * cnt / query.size();

    return percentage;

}

void Mymesh::create_aabb_tree() {

        // std::unique_ptr<Tree> tree = std::make_unique<Tree> (faces(mesh).first, faces(mesh).second, mesh);
        std::shared_ptr<Tree> tree = std::make_shared<Tree> (faces(mesh).first, faces(mesh).second, mesh);
        // Tree* tree = new Tree(faces(mesh).first, faces(mesh).second, mesh);
        aabbTree = tree;
        aabbTree->accelerate_distance_queries();
}

Surface_mesh& Mymesh::get_raw_mesh()
{
    return mesh;
}

Tree* Mymesh::get_aabb_tree()
{
    return aabbTree.get();
}

Mymesh::Mymesh(const std::string &file_path)
{
    load_from_off(file_path);
    triangulate_mesh();
}

Mymesh::Mymesh(const Surface_mesh sm)
{
    this->mesh = sm;
    triangulate_mesh();    
}

std::string Mymesh::to_wkt() 
{
    std::stringstream ss;
	ss<<"POLYHEDRALSURFACE Z (";
    bool lfirst = true;

    for (face_descriptor f: mesh.faces())
    {
        if (lfirst) lfirst = false;
        else ss << ",";

        ss << "((";
        bool first = true;
        Point firstpoint;

        for (vertex_descriptor v: vertices_around_face(mesh.halfedge(f), mesh))
        {
            if (first) 
            {
                firstpoint = mesh.point(v);
                first = false;
            }
            else ss << ",";
            Point p = mesh.point(v);
            ss << p[0] << " " << p[1] << " " << p[2];
        }

        ss << "," << firstpoint[0] << " " << firstpoint[1] << " " << firstpoint[2];
        ss << "))";

    }
    ss << ")";

    return ss.str();
}