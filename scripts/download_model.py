import urllib.request
import csv
import os
import argparse


def convert_url_to_file(url):
    illegal_chars = set(['/', ':', '@', '&', '*'])
    glb_file = ""
    for c in url:        
        if c in illegal_chars:
            glb_file += "_"
        else:
            glb_file += c

    return glb_file

def download(asct_b_grlc_file, model_dir):
    
    if not os.path.exists(model_dir):
        os.makedirs(model_dir)
    
    with open(asct_b_grlc_file) as f:
        reader = csv.DictReader(f, delimiter=',')
        for row in reader:
            glb_url = row['glb_file']
            glb_path = os.path.join(model_dir, convert_url_to_file(glb_url))
            print(glb_url)
            print(glb_path)
            if not os.path.exists(glb_path):
                urllib.request.urlretrieve(glb_url, glb_path)


if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument("asct_b_grlc_file", type=str, help="asct_b")
    parser.add_argument("model_dir", type=str, help="download diretory for the 3d models")
    args = parser.parse_args()
    asct_b_grlc_file = args.asct_b_grlc_file
    model_dir = args.model_dir
    
    download(asct_b_grlc_file, model_dir)


