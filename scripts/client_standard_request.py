import urllib.request
import json
import requests
import csv

table = []
url_tissue_rui = "https://ccf-api.hubmapconsortium.org/v1/hubmap/rui_locations.jsonld"
url_reference_organ_data = "https://hubmapconsortium.github.io/ccf-ui/rui/assets/reference-organ-data.json"
url_collision_detection = "http://192.168.1.100:8080/get-collisions"
maxWidth = 0

scaling_transformation = {}

responses = []

with urllib.request.urlopen(url_reference_organ_data) as content:
    data = json.loads(content.read().decode())
    print(data.keys())
    placement = data['placementPatches']

    for _, mapping in placement.items():
        source = mapping["source"]
        target = mapping["target"]
        x_scaling = mapping["x_scaling"]
        y_scaling = mapping["y_scaling"]
        z_scaling = mapping["z_scaling"]
        target_scaling = {"target": target, "x_scaling": x_scaling, "y_scaling": y_scaling, "z_scaling": z_scaling}
        scaling_transformation[source] = target_scaling


with urllib.request.urlopen(url_tissue_rui) as content:
    data = json.loads(content.read().decode())
    graph = data['@graph']

    for person in graph:
        samples = person['samples']
        for sample in samples:
            if sample['sample_type'] == 'Tissue Block':
                rui_location = sample['rui_location']
                placement = rui_location['placement']

                target = placement['target']
                organ = target

                if organ in scaling_transformation:
                    mapping = scaling_transformation[organ]
                    organ = mapping["target"]
                    placement['target'] = organ
                    # print("old organ after mapping:{}", organ)

                    placement["x_translation"] *= mapping["x_scaling"]
                    placement["y_translation"] *= mapping["y_scaling"]
                    placement["z_translation"] *= mapping["z_scaling"]

                r = requests.post(url=url_collision_detection, json=rui_location)
                if r:
                    try:
                        print(rui_location)
                        print(r.content)
                        print(r.json())
                    except:
                        pass
                    # response = {'@id': rui_location['@id'], 'result': r.json()}
                    # responses.append(response)

#                 row = [rui_location['@id']]
#                 width = 0
#                 percentage_sum = 0
#                 for element in r.json():
#                     row.append(element['node_name'])
#                     row.append(element['label'])
#                     row.append(element['representation_of'])
#                     row.append(element['percentage'])
#                     row.append(element['volume'])
#                     width += 5
#                     percentage_sum += element['percentage']
#
#                 maxWidth = max(maxWidth, width)
#                 row.insert(1, percentage_sum)
#                 print(row)
#                 table.append(row)
#
# print("max width of rows: {}".format(maxWidth))
#
# max_collision_AS = maxWidth // 5
#
# header = ['tissue block id', 'total percentage']
#
# for i in range(1, max_collision_AS + 1):
#     header.append('node_name' + '/' + str(i))
#     header.append('ccf_annotation' + '/' + str(i))
#     header.append('representation_of' + '/' + str(i))
#     header.append('percentage' + '/' + str(i))
#     header.append('volume' + '/' + str(i))
#
# with open('VHMRightKidney_tabular.csv', 'w', encoding='UTF8', newline='') as f:
#     writer = csv.writer(f)
#
#     # write the header
#     writer.writerow(header)
#
#     # write multiple rows
#     writer.writerows(table)