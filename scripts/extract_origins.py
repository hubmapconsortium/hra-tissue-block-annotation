# generate origins from https://raw.githubusercontent.com/hubmapconsortium/hubmap-ontology/master/source_data/generated-reference-spatial-entities.jsonld

# import urllib library
from urllib.request import urlopen
import json
import csv


def gen_origins(url, output_csv):
    # store the URL in url as
    # parameter for urlopen
    # store the response of URL
    response = urlopen(url)

    # storing the JSON response
    # from url in data
    data_json = json.loads(response.read())

    fileds = ['organ', 'x_origin', 'y_origin', 'z_origin']
    with open(output_csv, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(fileds)

        for spatial_entity in data_json:
            if spatial_entity['@type'] == "SpatialPlacement":
                x_translation = -spatial_entity["x_translation"]/1000.0
                y_translation = -spatial_entity["y_translation"]/1000.0
                z_translation = -spatial_entity["z_translation"]/1000.0
                target = spatial_entity["target"]
                csvwriter.writerow([target, x_translation, y_translation, z_translation])



if __name__ == "__main__":

    url = "https://raw.githubusercontent.com/hubmapconsortium/hubmap-ontology/master/source_data/generated-reference-spatial-entities.jsonld"
    csv_file = "./organ_origins_meter.csv"
    gen_origins(url, csv_file)
