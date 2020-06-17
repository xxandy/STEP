# This file is a copy of https://github.com/hayatoito/google-step-tsp/blob/master/common.py.

def read_input(filename):
    with open(filename) as f:
        cities = []
        for line in f.readlines()[1:]:  # Ignore the first line.
            xy = line.split(',')
            cities.append((float(xy[0]), float(xy[1])))
        return cities


def format_tour(tour):
    return 'index\n' + '\n'.join(map(str, tour))


def print_tour(tour):
    print(format_tour(tour))
