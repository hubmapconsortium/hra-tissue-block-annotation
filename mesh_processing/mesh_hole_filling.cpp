#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/border.h>
// #include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include <CGAL/IO/OFF_reader.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3                                     Point;
typedef CGAL::Surface_mesh<Point>                           Mesh;
typedef boost::graph_traits<Mesh>::vertex_descriptor        vertex_descriptor;
typedef boost::graph_traits<Mesh>::halfedge_descriptor      halfedge_descriptor;
typedef boost::graph_traits<Mesh>::face_descriptor          face_descriptor;
namespace PMP = CGAL::Polygon_mesh_processing;
namespace fs = boost::filesystem;


bool is_small_hole(halfedge_descriptor h, Mesh & mesh,
                   double max_hole_diam, int max_num_hole_edges)
{
  int num_hole_edges = 0;
  CGAL::Bbox_3 hole_bbox;
  for (halfedge_descriptor hc : CGAL::halfedges_around_face(h, mesh))
  {
    const Point& p = mesh.point(target(hc, mesh));
    hole_bbox += p.bbox();
    ++num_hole_edges;
    // Exit early, to avoid unnecessary traversal of large holes
    if (num_hole_edges > max_num_hole_edges) return false;
    if (hole_bbox.xmax() - hole_bbox.xmin() > max_hole_diam) return false;
    if (hole_bbox.ymax() - hole_bbox.ymin() > max_hole_diam) return false;
    if (hole_bbox.zmax() - hole_bbox.zmin() > max_hole_diam) return false;
  }
  return true;
}

// load non-manifold
void load_non_manifold_mesh(std::string file_path, Mesh &mesh)
{
  std::ifstream input(file_path);
  std::vector<Point> points;
  std::vector<std::vector<std::size_t> > polygons;

  if (!input || !CGAL::read_OFF(input, points, polygons) || points.empty())
  {
    std::cerr << "Cannot open file " << std::endl;
    return;
  }

  CGAL::Polygon_mesh_processing::orient_polygon_soup(points, polygons);
  CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, mesh);

}


void mesh_hole_fill_refine_fair(const fs::path &file_path, const fs::path &output_organ_dir)
{

  if (!fs::exists(output_organ_dir)) fs::create_directory(output_organ_dir);

  Mesh mesh;
  
  std::ifstream input(file_path.string());
  if ( !input || !(input >> mesh) || mesh.is_empty() ) {
    std::cerr << file_path << " Not a manifold mesh." << std::endl;
    //fix non-manifold meshes
    load_non_manifold_mesh(file_path.string(), mesh);

  }
  
  // Both of these must be positive in order to be considered
  double max_hole_diam = 100.0;
  int max_num_hole_edges = 100000000;
  unsigned int nb_holes = 0;
  std::vector<halfedge_descriptor> border_cycles;
  // collect one halfedge per boundary cycle
  PMP::extract_boundary_cycles(mesh, std::back_inserter(border_cycles));

  std::cout << file_path << std::endl;
  std::cout <<"is closed before hole filling: " << CGAL::is_closed(mesh) << std::endl;
  for(halfedge_descriptor h : border_cycles)
  {
    if(max_hole_diam > 0 && max_num_hole_edges > 0 &&
       !is_small_hole(h, mesh, max_hole_diam, max_num_hole_edges))
      continue;
    std::vector<face_descriptor>  patch_facets;
    std::vector<vertex_descriptor> patch_vertices;
    bool success = std::get<0>(PMP::triangulate_refine_and_fair_hole(mesh,
                                                                     h,
                                                                     std::back_inserter(patch_facets),
                                                                     std::back_inserter(patch_vertices)));
    // std::cout << "* Number of facets in constructed patch: " << patch_facets.size() << std::endl;
    // std::cout << "  Number of vertices in constructed patch: " << patch_vertices.size() << std::endl;
    // std::cout << "  Is fairing successful: " << success << std::endl;
    ++nb_holes;
  }
  std::cout << nb_holes << " holes have been filled" << std::endl;
  std::cout << "is closed after hole filling: " << CGAL::is_closed(mesh) << std::endl;
  fs::path output_file = output_organ_dir / file_path.stem(); 
  std::ofstream out(output_file.string() + ".off");
  out.precision(17);
  out << mesh << std::endl;
  

}

// Incrementally fill the holes that are no larger than given diameter
// and with no more than a given number of edges (if specified).
int main(int argc, char* argv[])
{
  // std::string body_path = "/home/catherine/data/model/plain_with_holes/";
  // std::string output_dir = "/home/catherine/data/model/plain_filling_hole";

  if (argc < 3)
  {
    std::cout << "Please provide body_path and output_dir!" << std::endl;
    return 0;
  }

  std::string body_path = std::string(argv[1]);
  std::string output_dir = std::string(argv[2]);
  
  if (!fs::exists(output_dir)) fs::create_directory(output_dir);
  fs::path dir(output_dir);
  for (fs::directory_entry& organ_path : fs::directory_iterator(body_path)) 
  {
        std::cout << std::endl << organ_path << std::endl;
        fs::path organ_name = organ_path.path().stem();
        fs::path output_organ_dir = dir / organ_name;
        
        for (fs::directory_entry& AS : fs::directory_iterator(organ_path)) 
        {
          mesh_hole_fill_refine_fair(AS.path(), output_organ_dir);

        }
  }

  return 0;
}

