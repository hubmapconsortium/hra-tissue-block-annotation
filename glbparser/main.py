# Lu Chen, 03/29/2022
import collections
import csv
import os.path
import random

from pygltflib import GLTF2
import numpy as np


# load ASCT+B table mapping_total[organ][AS] = set of nodes in glb
def load_ASCT_B_mapping(organ=""):
    file = 'C:/Users/catherine/Desktop/Research/ccf-releases/v1.1/models/ASCT-B_3D_Models_Mapping.csv'
    mapping_total = collections.defaultdict()
    with open(file, encoding='utf-8') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')

        if organ != "":
            mapping_total[organ] = collections.defaultdict(set)
            mapping = mapping_total[organ]

            for i, row in enumerate(csv_reader):
                if i <= 10:
                    continue

                if row[6] == organ:
                    node = row[2]
                    AS = row[3]
                    mapping[AS].add(node)

        else:
            for i, row in enumerate(csv_reader):
                if i <= 10:
                    continue

                organ = row[6]
                node = row[2]
                AS = row[3]

                if organ not in mapping_total:
                    mapping_total[organ] = collections.defaultdict(set)
                if AS != '-':
                    mapping_total[organ][AS].add(node)

    print(mapping_total)
    return mapping_total


def glb_parser(organ: str, mapping, output_dir=None):
    data_type_dict = {5121: 'uint8', 5123: 'uint16', 5125: 'uint32', 5126: 'float32'}
    number_of_components = {'SCALAR': 1, 'VEC2': 2, 'VEC3': 3, 'VEC4': 4, 'MAT2': 4, 'MAT3': 9, 'MAT4': 16}

    models_dir = 'C:/Users/catherine/Desktop/Research/ccf-releases/v1.1/models/'
    file = organ + '.glb'
    if file not in os.listdir(models_dir):
        print('{} is not exist'.format(file))
        return

    glb = GLTF2.load(os.path.join(models_dir, file))
    binary_blob = glb.binary_blob()

    dict_name = {}

    for i, node in enumerate(glb.nodes):
        print(i, node.name)
        dict_name[node.name] = i
    meshes = post_traversal(glb.nodes)
    mapping_mesh_AS = collections.defaultdict(set)

    for AS, nodes in mapping.items():
        meshes_set_AS = set()
        for name in nodes:
            if name in dict_name:
                node_index = dict_name[name]
                meshes_set_AS.update(meshes[node_index])
        mapping_mesh_AS[AS] = meshes_set_AS

    for AS, meshes in mapping_mesh_AS.items():

        triangles_list = []
        points_list = []
        for i in meshes:
            triangles_accessor = glb.accessors[glb.meshes[i].primitives[0].indices]
            triangles_buffer_view = glb.bufferViews[triangles_accessor.bufferView]
            dtype = data_type_dict[triangles_accessor.componentType]
            triangles = np.frombuffer(
                binary_blob[
                triangles_buffer_view.byteOffset
                + triangles_accessor.byteOffset: triangles_buffer_view.byteOffset
                                                 + triangles_buffer_view.byteLength
                ],
                dtype=dtype,
                count=triangles_accessor.count,
            ).reshape((-1, 3))

            points_accessor = glb.accessors[glb.meshes[i].primitives[0].attributes.POSITION]
            points_buffer_view = glb.bufferViews[points_accessor.bufferView]
            dtype = data_type_dict[points_accessor.componentType]

            points = np.frombuffer(
                binary_blob[
                points_buffer_view.byteOffset
                + points_accessor.byteOffset: points_buffer_view.byteOffset
                                              + points_buffer_view.byteLength
                ],
                dtype=dtype,
                count=points_accessor.count * 3,
            ).reshape((-1, 3))

            triangles_list.append(triangles)
            points_list.append(points)

        output_dir = "C:/Users/catherine/Desktop/Research/hubmap/models/AS/" + organ
        if not os.path.exists(output_dir):
            os.mkdir(output_dir)

        save_multiple_meshes(triangles_list, points_list, AS, output_dir)


def save_multiple_meshes(triangles_list, points_list, name, output_dir):
    count_p = 0
    count_t = 0

    flatten_triangles = []
    flatten_points = []

    for triangles, points in zip(triangles_list, points_list):

        flatten_points.extend(points)
        tmp_triangles = []
        for triangle in triangles:
            a, b, c = triangle
            tmp_triangles.append([a + count_p, b + count_p, c + count_p])
        flatten_triangles.extend(tmp_triangles)

        count_p += len(points)
        count_t += len(triangles)

    file = "_".join(name.split(' '))
    output_path = os.path.join(output_dir, file + '.off')
    color = [random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)]
    with open(output_path, 'w') as f:
        f.write("COFF\n")
        f.write("{} {} 0\n".format(len(flatten_points), len(flatten_triangles)))

        for point in flatten_points:
            f.write("{} {} {} {} {} {} 255\n".format(point[0], point[1], point[2], color[0], color[1], color[2]))

        for triangle in flatten_triangles:
            f.write("3 {} {} {}\n".format(triangle[0], triangle[1], triangle[2]))

        print("{} has {} points, {} triangle faces\n".format(name, len(flatten_points), len(flatten_triangles)))


def post_traversal(nodes):
    if len(nodes) == 0:
        print('empty glb file!')

    result = [[] for _ in range(len(nodes))]
    visited = [0 for _ in range(len(nodes))]

    for i in range(len(nodes)):
        post_traversal_helper(nodes, i, result, visited)

    for i in range(len(result)):
        print(i, result[i])

    return result


def post_traversal_helper(nodes, root_index, result, visited):
    if visited[root_index]:
        return

    visited[root_index] = 1
    root_node = nodes[root_index]
    meshes = []

    if not root_node.children:
        result[root_index] = [root_node.mesh]
        return [root_node.mesh]

    for child in root_node.children:
        descendants = post_traversal_helper(nodes, child, result, visited)
        meshes.extend(descendants)

    result[root_index] = meshes
    return meshes


if __name__ == "__main__":
    # single organ
    # organ = 'VH_F_Eye_L'
    # mapping_total = load_ASCT_B_mapping(organ)
    # mapping = mapping_total[organ]
    # glb_parser(organ, mapping)

    # all organs
    mapping_total = load_ASCT_B_mapping()

    for organ in mapping_total:
        print(organ)
        glb_parser(organ, mapping_total[organ])

