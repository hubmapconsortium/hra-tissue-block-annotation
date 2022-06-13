#ifndef ALGO_H
#define ALGO_H

#include <iostream>
#include <vector>

#include "mymesh.h"

std::vector<std::pair<std::string, double>> collision_detection_single_tissue(std::vector<Mymesh> &organ, Mymesh &tissue, std::vector<Point> &points);

#endif