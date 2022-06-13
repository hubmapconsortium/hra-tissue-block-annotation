#pragma once
#include <sstream>
#include <iostream>
#include<fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <Eigen/Dense>
#include <math.h>

#include "mymesh.h"

#include<boost/filesystem.hpp>
// using namespace boost::filesystem;
namespace fs = boost::filesystem;

#define PI 3.14159265


//origins, meshes
void load_all_organs(const std::string &body_path, std::unordered_map<std::string, std::vector<Mymesh>> &total_body);

// generate origins from the whole model without any hole-filling.
void gen_origin(const std::string &whole_model_path, std::unordered_map<std::string, Eigen::Vector3d> &organ_origins);

//including x_scaling, x_rotation, x_translation, x_origin 
void tissue_transform(std::unordered_map<std::string, double> &params, Surface_mesh &tissue_mesh, std::vector<Point> &points, int resolution);

std::string organ_split(const std::string &url);

void load_ASCT_B(const std::string &file_path, std::unordered_map<std::string, std::string> &mapping, std::unordered_map<std::string, std::string> &mapping_node_ontology, std::unordered_map<std::string, std::string> &mapping_node_label);