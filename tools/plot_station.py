#!/usr/bin/env python3
import argparse
import csv
import matplotlib.pyplot as plt
import glob
import os


def read_station_csv(path):
    t = []
    h = []
    with open(path, 'r', newline='') as f:
        r = csv.reader(f)
        # try to skip header
        first = next(r, None)
        if first is None:
            return t, h
        # detect header: if non-numeric in first cell
        def is_number(s):
            try:
                float(s)
                return True
            except:
                return False
        if not is_number(first[0]):
            # assume header consumed
            pass
        else:
            # first line contained numeric value -> treat it as data
            try:
                t.append(float(first[0]))
                h.append(float(first[1]))
            except:
                pass
        for row in r:
            if len(row) < 2:
                continue
            try:
                t.append(float(row[0]))
                h.append(float(row[1]))
            except:
                continue
    return t, h


def main():
    p = argparse.ArgumentParser(description='Plot station CSV time series.')
    p.add_argument('files', nargs='+', help='CSV files or glob patterns')
    p.add_argument('-o', '--output', help='Write plot to PNG file')
    p.add_argument('--show', action='store_true', help='Show interactive plot')
    p.add_argument('--title', default='Station time series', help='Plot title')
    args = p.parse_args()

    # expand globs
    files = []
    for f in args.files:
        if any(ch in f for ch in ['*', '?', '[']):
            files.extend(glob.glob(f))
        else:
            files.append(f)

    if not files:
        print('no files found')
        return

    plt.figure(figsize=(8,4))
    for f in files:
        if not os.path.exists(f):
            print('skip non-existing file:', f)
            continue
        t, h = read_station_csv(f)
        if not t:
            print('no data in', f)
            continue
        label = os.path.splitext(os.path.basename(f))[0]
        plt.plot(t, h, label=label)

    plt.xlabel('time (s)')
    plt.ylabel('height (m)')
    plt.title(args.title)
    plt.grid(True)
    if len(files) > 1:
        plt.legend()

    if args.output:
        plt.savefig(args.output, dpi=150, bbox_inches='tight')
        print('wrote', args.output)

    if args.show or not args.output:
        plt.show()

if __name__ == '__main__':
    main()
