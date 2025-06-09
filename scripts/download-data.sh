#!/bin/bash
set -e

VERSION=v2.3
off_url=https://cdn.humanatlas.io/hra-glb-off-releases/hra-glb-off-data.${VERSION}.zip

rm -rf model
mkdir -p model
curl $off_url -o hra-glb-off-data.zip
unzip -q hra-glb-off-data.zip -d model
rm -f hra-glb-off-data.zip
