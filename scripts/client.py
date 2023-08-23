# created by Lu Chen
# date: June 14, 2022

import json
import requests
import urllib.request

collision_detection_url = 'http://localhost:8080/get-collisions'
enriched_rui_location_url = 'https://github.com/cns-iu/hra-cell-type-populations-supporting-information/blob/main/data/enriched_rui_locations.jsonld'
responses = []

with urllib.request.urlopen(enriched_rui_location_url) as url:
    data = json.load(url)
    graph = data['@graph']

    for person in graph:
        samples = person['samples']
        for sample in samples:
            if sample['sample_type'] == 'Tissue Block':
                rui_location = sample['rui_location']
                r = requests.post(url=collision_detection_url, json=rui_location)
                response = {'@id': rui_location['@id'], 'result': r.json()}
                responses.append(response)


print(responses)
# with open('collision_result.json', 'w') as write_file:
#     json.dump(responses, write_file, indent=4)







