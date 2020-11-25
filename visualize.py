import json
import sys
import matplotlib.pyplot as plt

if __name__ == "__main__":
    path = sys.argv[1]
    data = None

    with open(path, 'r') as fi:
        data = json.load(fi)

    mc = sorted([(k, v) for k, v in data["word_counts"].items()], key=lambda item: item[1])[-10:][::-1]

    fig = plt.figure()

    plt.bar([k for k, v in mc], [v for k, v in mc])

    plt.show()