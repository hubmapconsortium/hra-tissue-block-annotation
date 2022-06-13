import os.path
import random


def bb_organ(organ_name):

    x_min = y_min = z_min = float('inf')
    x_max = y_max = z_max = float('-inf')
    path = "./merged_models/" + organ_name + '.off'

    with open(path, 'r') as file:
        line = file.readline()
        line = file.readline()
        v_count, f_count, _ = line.split(' ')
        v_count, f_count = int(v_count), int(f_count)

        while v_count:
            line = file.readline()
            line = line.split(' ')
            x, y, z = float(line[0]), float(line[1]), float(line[2])
            x_min, y_min, z_min = min(x_min, x), min(y_min, y), min(z_min, z)
            x_max, y_max, z_max = max(x_max, x), max(y_max, y), max(z_max, z)
            v_count -= 1

    return x_min, y_min, z_min, x_max, y_max, z_max


def tissue(bb_coordinate, ranges):
    x_min, y_min, z_min, x_max, y_max, z_max = bb_coordinate
    x_center, y_center, z_center = random.uniform(x_min, x_max), random.uniform(y_min, y_max), random.uniform(z_min, z_max)
    print(x_max - x_min, y_max - y_min, z_max - z_min)
    range_x, range_y, range_z = ranges
    x_len, y_len, z_len = random.uniform(range_x[0], range_x[1]), random.uniform(range_y[0], range_y[1]), random.uniform(range_z[0], range_z[1])
    x_min, y_min, z_min = x_center - x_len/2, y_center - y_len/2, z_center - z_len/2
    x_max, y_max, z_max = x_center + x_len/2, y_center + y_len/2, z_center + z_len/2

    return x_min, y_min, z_min, x_max, y_max, z_max


def save_tissue(bb_tissue, file_path):

    x_min, y_min, z_min, x_max, y_max, z_max = bb_tissue
    with open(file_path, 'w') as file:
        file.write('OFF\n')
        file.write('8 6 0\n')
        file.write('{} {} {}\n'.format(x_min, y_min, z_min))
        file.write('{} {} {}\n'.format(x_max, y_min, z_min))
        file.write('{} {} {}\n'.format(x_max, y_max, z_min))
        file.write('{} {} {}\n'.format(x_min, y_max, z_min))
        file.write('{} {} {}\n'.format(x_min, y_min, z_max))
        file.write('{} {} {}\n'.format(x_max, y_min, z_max))
        file.write('{} {} {}\n'.format(x_max, y_max, z_max))
        file.write('{} {} {}\n'.format(x_min, y_max, z_max))

        file.write('4 3 2 1 0\n4 4 5 6 7\n4 4 7 3 0\n4 1 2 6 5\n4 0 1 5 4\n4 2 3 7 6\n')


if __name__ == "__main__":
    organ_name = "heart"
    ranges = [[0.015, 0.020], [0.008, 0.012], [0.008, 0.012]]
    bb_coordinate = bb_organ(organ_name)
    directory = './tissue/' + organ_name
    if not os.path.exists(directory):
        os.mkdir(directory)

    for i in range(600):
        bb_tissue = tissue(bb_coordinate, ranges)
        file_path = './tissue/' + organ_name + '/tissue_' + str(i) + '.off'
        save_tissue(bb_tissue, file_path)
