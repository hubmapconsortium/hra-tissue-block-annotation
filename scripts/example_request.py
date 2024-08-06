'''
Author: Lu Chen
Date: 05/21/2024
'''

import json
import requests
import urllib.request

collision_detection_url = 'http://localhost:8080/get-collisions'

with open('../examples/test-registration.json') as f:
    rui_location = json.load(f)
    r = requests.post(url=collision_detection_url, json=rui_location)
    print(r.json)