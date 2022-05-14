# quick script to read files written by fbm_test.c
import sys

def read_file(filename):
    with open(filename, "r") as f:
        floats = [float(n) for n in f.read().split()]
        avg = (sum(floats) / len(floats))
        fmin = min(floats)
        fmax = max(floats)
        print(f'{filename}\nAverage: {avg}\nMin: {fmin}\nMax: {fmax}\n')

if __name__ == "__main__":
    if len(sys.argv) > 1:
        for fname in sys.argv[1:]:
            read_file(fname)
