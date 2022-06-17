#include "algo.h"
#include "utils.h"

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <string>

//global variables
std::map<utility::string_t, utility::string_t> dictionary;
std::unordered_map<std::string, Eigen::Vector3d> organ_origins;                     //origins of organs
std::unordered_map<std::string, std::string> mapping;                               //mapping from standard organ name(e.g., #VHFLeftKidney) to glb file name without suffix(e.g., VH_F_Kidney_L)
std::unordered_map<std::string, std::vector<Mymesh>> total_body;                    //mapping from organ name(glb file name) to vector of meshes of a certain organ
std::unordered_map<std::string, SpatialEntity> mapping_node_spatial_entity;         // mapping from AS to its information in asct-b table 

//display json
void display_json(
   json::value const & jvalue,
   utility::string_t const & prefix)
{
   std::cout << prefix << jvalue.serialize() << std::endl;
}

//parse json
void parse_json(json::value const &jvalue, json::value &answer)
{

   std::unordered_map<std::string, double> params;


   auto target = jvalue.at("target").as_string();
   auto tmp = organ_split(target); 
   std::cout << "target url: " << target << " target: " << tmp << " " << std::endl;
   
   // only test for kidneys, will test other organs soon.
   if (!(tmp == "#VHFLeftKidney" || tmp == "#VHFRightKidney" || tmp == "#VHMLeftKidney" || tmp == "#VHMRightKidney"))
   {
      answer[U("error_message")] = json::value::string(U("only test tissue blocks in kidneys"));
      return;
   }

   //extract parameters from json request
   params["x_dimension"] = jvalue.at("x_dimension").as_double();
   params["y_dimension"] = jvalue.at("y_dimension").as_double();
   params["z_dimension"] = jvalue.at("z_dimension").as_double();
   params["x_scaling"] = jvalue.at("x_scaling").as_double();
   params["y_scaling"] = jvalue.at("y_scaling").as_double();
   params["z_scaling"] = jvalue.at("z_scaling").as_double();
   params["x_translation"] = jvalue.at("x_translation").as_double();
   params["y_translation"] = jvalue.at("y_translation").as_double();
   params["z_translation"] = jvalue.at("z_translation").as_double();
   params["x_rotation"] = jvalue.at("x_rotation").as_double();
   params["y_rotation"] = jvalue.at("y_rotation").as_double();
   params["z_rotation"] = jvalue.at("z_rotation").as_double();
   

   std::string organ_name = mapping[organ_split(target)];
   std::cout << "target url: " << target << " target: " << tmp << " " << "standard: " << organ_name << std::endl;

   Eigen::Vector3d origin = organ_origins[organ_name];
   params["x_origin"] = origin(0);
   params["y_origin"] = origin(1);
   params["z_origin"] = origin(2);

   
   Surface_mesh tissue_mesh;
   std::vector<Point> points; //center of voxels inside the tissue block
   tissue_transform(params, tissue_mesh, points, 10);

   Mymesh my_tissue(tissue_mesh);
   my_tissue.create_aabb_tree();

   //core function
   std::vector<std::pair<std::string, double>> result = collision_detection_single_tissue(total_body[organ_name], my_tissue, points);

   //print result
   std::cout << "result:\nlabel         percentage" << std::endl;
   for (auto s: result) {std::cout << s.first << " " << s.second << std::endl;}
   
   //construct the response
   double tissue_volume = params["x_dimension"] * params["y_dimension"] * params["z_dimension"];
   for (int i = 0; i < result.size(); i++)
   {
      auto res = result[i];
      json::value AS;
      
      auto node_name = res.first;
      SpatialEntity &se = mapping_node_spatial_entity[node_name];
      AS[U("node_name")] = json::value::string(U(node_name));
      AS[U("label")] = json::value::string(U(se.label));
      AS[U("representation_of")] = json::value::string(U(se.representation_of));
      AS[U("id")] = json::value::string("http://purl.org/ccf/latest/ccf.owl" + se.source_spatial_entity + "_" + node_name);

      if (res.second < 0) 
      {
         AS[U("percentage")] = json::value(0);
         AS[U("is_closed")] = json::value(false);
      }
      else
      {
         AS[U("percentage")] = json::value(res.second);
         AS[U("volume")] = json::value(res.second * tissue_volume);
         AS[U("is_closed")] = json::value(true);
      }
      
      answer[i] = AS;
   }

}


void handle_get(http_request request)
{

   std::cout << "\nhandle GET" << std::endl;

   auto answer = json::value::object();

   for (auto const & p : dictionary)
   {
      answer[p.first] = json::value::string(p.second);
   }

   display_json(json::value::null(), "R: ");
   display_json(answer, "S: ");

   request.reply(status_codes::OK, answer);
}

void handle_request(http_request request, std::function<void(json::value const &, json::value &)> action)
{
   auto answer = json::value::object();
   // json::value answer;

   request
      .extract_json()
      .then([&answer, &action](pplx::task<json::value> task) {
         try
         {
            auto const & jvalue = task.get();
            display_json(jvalue, "R: ");

            if (!jvalue.is_null())
            {
               action(jvalue, answer);
            }
            
            display_json(answer, "S: ");
         }
         catch (http_exception const & e)
         {
            std::cout << e.what() << std::endl;
         }
      })
      .wait();

   if (answer != json::value::null())
      request.reply(status_codes::OK, answer);
   else
      request.reply(status_codes::OK, json::value::array());
}

void handle_post(http_request request)
{
   // TRACE("\nhandle POST\n");
   std::cout << "\nhandle POST" << std::endl;

   handle_request(
      request,
      parse_json
   );
}


int main()
{
   //origins are stored in a json file
   std::string whole_model_path = "";
   std::string asct_b_file_path = "/home/catherine/data/model/ASCT-B_3D_Models_Mapping.csv";
   std::string body_path = "/home/catherine/data/model/plain_filling_hole";

   // load origins
   gen_origin(whole_model_path, organ_origins);
   load_ASCT_B(asct_b_file_path, mapping, mapping_node_spatial_entity);
   load_all_organs(body_path, total_body);

   http_listener listener("http://192.168.1.100:12345/get-collisions");

   listener.support(methods::GET,  handle_get);
   listener.support(methods::POST, handle_post);


   try
   {
      listener
         .open()
         .then([&listener]() {
            // TRACE("\nstarting to listen\n"); 
            std::cout << "\nstarting to listen" << std::endl;
            })
         .wait();

      while (true);
   }
   catch (std::exception const & e)
   {
      std::cout << e.what() << std::endl;
   }

   return 0;

}