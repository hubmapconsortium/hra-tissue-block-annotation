"""
Author: Lu Chen
Date: 05/18/2024
"""

import json
import requests
import urllib.request
from io import StringIO
import csv

as_url = "https://grlc.io/api-git/hubmapconsortium/ccf-grlc/subdir/mesh-collision/anatomical-structures.csv"
patch_url = "https://grlc.io/api-git/hubmapconsortium/ccf-grlc/subdir/mesh-collision/placement-patches.csv"

print("Downloading ASCT and placement patches")
as_response = requests.get(url=as_url)
patch_response = requests.get(url=patch_url)

as_content = as_response.content.decode()
patch_content = patch_response.content.decode()

with open('./model/asct-b-grlc.csv', 'w') as f:
    f.writelines(as_content)

with open('./model/reference-organ-grlc.csv', 'w') as f:
    f.writelines(patch_content)

print("Sucessfully downloaded!")
