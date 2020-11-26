import json
import sys

if __name__ == "__main__":
    path = sys.argv[1]
    data = None

    with open(path, 'r') as fi:
        data = json.load(fi)

    mc = sorted([(k, v) for k, v in data["word_counts"].items()], key=lambda item: item[1])[-10:]

    print(f"The 10 most common words in {data['file']}:")
    for k, v in mc[::-1]:
        print(f"{k}: {v}")

    with open('out.csv', 'w') as fi:
        fi.write(f"# File: {data['file']}\n")
        fi.write(f"Word,Count\n")
        for k, v in mc[::-1]:
            fi.write(f"{k},{v}\n")