# created by Lu Chen
# date: June 14, 2022

import json
import requests

url = 'http://192.168.1.100:12345/get-collisions'

responses = []

with open("./rui_locations.jsonld", "r") as f:
    data = json.load(f)
    graph = data['@graph']

    for person in graph:
        samples = person['samples']
        for sample in samples:
            if sample['sample_type'] == 'Tissue Block':
                rui_location = sample['rui_location']
                placement = rui_location['placement']

                x_dimension = rui_location['x_dimension']
                y_dimension = rui_location['y_dimension']
                z_dimension = rui_location['z_dimension']

                target = placement['target']
                x_rotation = placement['x_rotation']
                y_rotation = placement['y_rotation']
                z_rotation = placement['z_rotation']
                x_translation = placement['x_translation']
                y_translation = placement['y_translation']
                z_translation = placement['z_translation']
                x_scaling = placement['x_scaling']
                y_scaling = placement['y_scaling']
                z_scaling = placement['z_scaling']

                json_dic = {'x_dimension': x_dimension, 'y_dimension': y_dimension, 'z_dimension': z_dimension,
                            'x_rotation': x_rotation, 'y_rotation': y_rotation, 'z_rotation': z_rotation,
                            'x_translation': x_translation, 'y_translation': y_translation, 'z_translation': z_translation,
                            'x_scaling': x_scaling, 'y_scaling': y_scaling, 'z_scaling': z_scaling,
                            'target': target}
                if "#VHFLeftKidney" in target or "#VHFRightKidney" in target or "#VHMLeftKidney" in target or "#VHMRightKidney" in target:
                    r = requests.post(url='http://192.168.1.100:12345/get-collisions', json=json_dic)
                    # print(rui_location['@id'])
                    # print(r.json())

                    response = {'@id': rui_location['@id'], 'result': r.json()}
                    responses.append(response)


print(responses)
with open('collision_result.json', 'w') as write_file:
    json.dump(responses, write_file, indent=4)







