# 3D Geometry-Based Tissue Block Annotation: Collision Detection between Tissue Blocks and Anatomical Structures

**Version:** 1.0.0

**Release date:** 15 June 2022

## Overview:

3D geometry-based tissue block annotation project includes: 
1. a C++ library for http service for collision detection and intersection volume computation between tissue blocks and anatomical structures 
2. a C++ library for checking manifoldness and closedness of meshes as well as hole filling for the unclosed meshes.   
3. a Python library for converting glb files to off (Object File Format) files, which is the underlying 3D model format for collision detection.


## Dependencies
For C++ libraries [1] [2]:
1. CMake
    ```bash
    sudo apt-get install build-essential libssl-dev
    cd /tmp
    wget https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0.tar.gz
    tar -zxvf cmake-3.20.0.tar.gz
    cd cmake-3.20.0
    ./bootstrap
    make
    sudo make install
    ```
2. Boost
    ```bash
    sudo apt-get update
    sudo apt-get install libboost-all-dev
    ```
3. GMP
    ```bash
    sudo apt-get install libgmp-dev
    ```
4. MPFR
    ```bash
    sudo apt-get install libmpfr-dev
    ```
3. CGAL
    ```bash
    sudo apt-get install libcgal-dev
    ```
4. Eigen3
    ```bash
    sudo apt install libeigen3-dev
    ```
5. cpprestsdk
    ```bash
    sudo apt-get install libcpprest-dev
    ```

For Python library [3]:
1. pygltflib
    ```bash
    pip install pygltflib
    ```
## Compilation

We use CMake to configure the program with third-party dependencies and generate the native build system by creating a CMakeLists.txt file. 

1. for collision detection and volume computation:
    ```bash
    cd $collision_detection_project_root
    mkdir build
    cd build
    cmake ..
    make
    ```
2. for hole filling:
    ```bash
    cd $hole_filling_project_root
    mkdir build
    cd build
    cmake ..
    make
    ```
note: you need to change or specify the directory of 3D models. 

## Usage
### Server side: 
1. convert glb files of 3D models to off (Object File Format) files:
    ```bash
    cd $glbparser
    python glb_parser.py
    ```
2. hole filling for every mesh:
    ```bash
    cd $hole_filling_project_root/build
    ./mesh_hole_filling
    ```
3. start http service and receive json request:
    ```bash
    cd $collision_detection_project_root/build
    ./server2
    ``` 

    Or directly download the meshes after step 1 and step 2 from Google Drive(link). So you can skip step 1 and step 2.

note: you need to change or specify the directory of 3D models.

### Client side:

POST http://server_ip:12345/restdemo

- JSON request example
```json
{
    
    "target": "http://purl.org/ccf/latest/ccf.owl#VHFLeftKidneyV1.1",
    "x_dimension": 10,
    "y_dimension": 10,
    "z_dimension": 10,
    "x_scaling": 1,
    "y_scaling": 1,
    "z_scaling": 1,
    "x_rotation": 0,
    "y_rotation": 0,
    "z_rotation": 0,
    "x_translation": 6.508,
    "y_translation": 110.468,
    "z_translation": 38.79
  
  }
  ```
  - JSON response example
  ```json
[
    {
        "OntologyID": "http://purl.obolibrary.org/obo/UBERON_0004200",
        "is_closed": true,
        "label": "renal pyramid",
        "node_name": "VH_F_renal_pyramid_L_a",
        "percentage": 0.010999999999999999,
        "volume": 11
    },
    {
        "OntologyID": "http://purl.obolibrary.org/obo/UBERON_0002015",
        "is_closed": true,
        "label": "kidney capsule",
        "node_name": "VH_F_kidney_capsule_L",
        "percentage": 0.062,
        "volume": 62
    }
]
  ```