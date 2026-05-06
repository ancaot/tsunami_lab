#!/usr/bin/env python3
import argparse
import csv
import os
from math import sqrt


def read_series(path):
    t = []
    h = []
    with open(path, "r", newline="") as f:
        r = csv.reader(f)
        first = next(r, None)
        if first is None:
            return t, h
        # skip non-numeric header
        try:
            float(first[0])
            float(first[1])
            t.append(float(first[0]))
            h.append(float(first[1]))
        except Exception:
            pass
        for row in r:
            if len(row) < 2:
                continue
            try:
                t.append(float(row[0]))
                h.append(float(row[1]))
            except Exception:
                continue
    return t, h


def nearest_value(t2, h2, t):
    if not t2:
        return None
    best_i = 0
    best_d = abs(t2[0] - t)
    for i in range(1, len(t2)):
        d = abs(t2[i] - t)
        if d < best_d:
            best_d = d
            best_i = i
    return h2[best_i]


def station_csv_path(base, station):
    return os.path.join(base, station, station + ".csv")


def list_stations(base):
    if not os.path.isdir(base):
        return []
    out = []
    for name in os.listdir(base):
        p = os.path.join(base, name)
        if os.path.isdir(p):
            out.append(name)
    return sorted(out)


def compare_station(file_ref, file_cmp):
    tr, hr = read_series(file_ref)
    tc, hc = read_series(file_cmp)
    if not tr or not tc:
        return None

    diffs = []
    for t, h in zip(tr, hr):
        hc_t = nearest_value(tc, hc, t)
        if hc_t is None:
            continue
        diffs.append(h - hc_t)

    if not diffs:
        return None

    mse = sum(d * d for d in diffs) / len(diffs)
    rmse = sqrt(mse)
    max_abs = max(abs(d) for d in diffs)
    mean_abs = sum(abs(d) for d in diffs) / len(diffs)
    return rmse, max_abs, mean_abs, len(diffs)


def main():
    ap = argparse.ArgumentParser(description="Compare station folders (e.g., 1D vs 2D).")
    ap.add_argument("reference", help="Reference station folder (e.g., station_sym_1d)")
    ap.add_argument("comparison", help="Comparison station folder (e.g., station_sym_2d)")
    ap.add_argument("-o", "--output", default="station_comparison.csv", help="Output CSV report")
    args = ap.parse_args()

    ref_st = set(list_stations(args.reference))
    cmp_st = set(list_stations(args.comparison))
    common = sorted(ref_st.intersection(cmp_st))

    if not common:
        print("No common station directories found.")
        return

    rows = []
    for st in common:
        fr = station_csv_path(args.reference, st)
        fc = station_csv_path(args.comparison, st)
        if not os.path.exists(fr) or not os.path.exists(fc):
            continue
        res = compare_station(fr, fc)
        if res is None:
            continue
        rmse, max_abs, mean_abs, n = res
        rows.append((st, rmse, max_abs, mean_abs, n))

    if not rows:
        print("No comparable data found in common stations.")
        return

    with open(args.output, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["station", "rmse", "max_abs", "mean_abs", "n_samples"])
        for r in rows:
            w.writerow(r)

    print("Wrote", args.output)


if __name__ == "__main__":
    main()
