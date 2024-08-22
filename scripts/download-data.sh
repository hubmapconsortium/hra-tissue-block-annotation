#!/bin/bash


rm -rf model
mkdir -p model

off_url=https://cdn.humanatlas.io/hra-glb-off-releases/hra-glb-off.v2.1.zip
as_url=https://grlc.io/api-git/hubmapconsortium/ccf-grlc/subdir/mesh-collision/anatomical-structures.csv
patch_url=https://grlc.io/api-git/hubmapconsortium/ccf-grlc/subdir/mesh-collision/placement-patches.csv

curl $off_url -o model/hra-glb-off.zip
unzip model/hra-glb-off.zip -d model/all_preprocessed_off_models_cgal
rm -f model/hra-glb-off.zip

curl $as_url -o model/asct-b-grlc.csv
curl $patch_url -o model/reference-organ-grlc.csv
