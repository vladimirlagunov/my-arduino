#!/usr/bin/env python3
import argparse
import os
import sys
from matplotlib import pyplot, ticker


def main():
    parser = argparse.ArgumentParser(description='Input expects micros, output in millis')
    parser.add_argument('-i', '--input', type=argparse.FileType('r'), default=sys.stdin)
    parser.add_argument('--cases-delay-millis', type=int, default=500)
    parser.add_argument('--out-dir', default='dump')
    parser.add_argument('labels', nargs=argparse.REMAINDER)
    args = parser.parse_args()

    raw_data = read_from_stream(args.input)
    data = (
        d for d in (
            shift_data(filter_back(d))
            for d in regroup_data(raw_data, args.cases_delay_millis * 1000)
        ) if d
    )
    dump_data(args.out_dir, data, args.labels)


def read_from_stream(stream):
    try:
        for line in stream:
            try:
                line = line.strip()
                x, y = map(int, line.split())
                yield (x, y)
            except (TypeError, ValueError):
                continue
    except KeyboardInterrupt:
        print('Keyboard interrupt')
    except EnvironmentError as err:
        print(err)


def filter_back(data):
    """
    >>> filter_back([
    ...     (0, 0),
    ...     (10, 0),
    ...     (10, 1),
    ...     (15, 1),
    ...     (15, 0),
    ...     (19, 0),
    ...     (19, 1),
    ...     (20, 1),
    ...     (20, 0),
    ...     (30, 0),
    ...     (40, 0),
    ... ])
    [(0, 0), (10, 0), (10, 1), (15, 1), (15, 0), (19, 0), (19, 1), (20, 1), (20, 0)]
    >>> filter_back([(0, 1), (10, 1), (10, 0)])
    [(0, 1), (10, 1), (10, 0)]
    >>> filter_back([(0, 1), (10, 1)])
    [(0, 1), (10, 1)]
    >>> filter_back([(0, 0), (10, 0)])
    []
    """
    while len(data) > 1 and not data[-2][1] and not data[-1][1]:
        del data[-1]
    if any(d[1] for d in data):
        return data
    else:
        return []


def shift_data(data):
    """
    >>> shift_data([
    ...     (100, 0),
    ...     (101, 0),
    ...     (110, 1),
    ...     (120, 1),
    ...     (120, 0),
    ...     (125, 0),
    ...     (125, 1),
    ...     (130, 1),
    ...     (130, 0),
    ...     (140, 0),
    ... ])
    [(0, 1), (10, 1), (10, 0), (15, 0), (15, 1), (20, 1), (20, 0), (30, 0)]
    >>> shift_data([
    ...     (110, 1),
    ...     (120, 1),
    ...     (120, 0),
    ...     (125, 0),
    ...     (125, 1),
    ...     (130, 1),
    ...     (130, 0),
    ...     (140, 0),
    ... ])
    [(0, 1), (10, 1), (10, 0), (15, 0), (15, 1), (20, 1), (20, 0), (30, 0)]
    >>> shift_data([(50, 0), (60, 0)])
    []
    >>> shift_data([(50, 0)])
    []
    >>> shift_data([])
    []
    """
    for idx, (offset, signal) in enumerate(data):
        if signal:
            return [(micros - offset, s) for micros, s in data[idx:]]
    return []


def regroup_data(raw_data, cases_delay_micros):
    """
    >>> list(regroup_data([
    ...    (1000, 123),
    ...    (1001, 456),
    ...    (1005, 789),
    ...    (1060, 135),
    ...    (1075, 246),
    ...    (2000, 357),
    ...    (2100, 468),
    ...    (2101, 579),
    ... ], 50))
    ... # doctest: +NORMALIZE_WHITESPACE
    [[(1000, 123), (1001, 456), (1005, 789)],
     [(1060, 135), (1075, 246)],
     [(2000, 357)],
     [(2100, 468), (2101, 579)]]
    """
    buf = []
    for micros, signal in raw_data:
        if not buf or buf[-1][0] + cases_delay_micros >= micros:
            buf.append((micros, signal))
        else:
            yield buf
            buf = [(micros, signal)]
    if buf:
        yield buf


def dump_data(dst_dir, data_gen, labels):
    if not os.path.isdir(dst_dir):
        os.mkdir(dst_dir)
    labels = list(labels)
    for idx, data in enumerate(data_gen):
        try:
            label = labels[idx]
        except IndexError:
            label = 'btn' + str(idx + 1)
        print("Start generating {}, don't press any button...\r".format(label))
        with open(os.path.join(dst_dir, label + '.txt'), 'w') as f:
            f.writelines('{}\t{}\n'.format(*d) for d in data)
        fig = pyplot.figure(figsize=(16, 6))
        pyplot.title(label)
        pyplot.grid(color='#888888', linestyle='dashed', linewidth=1, which='major')
        pyplot.grid(color='#cccccc', linestyle='dotted', linewidth=1, which='minor')
        fig.axes[0].xaxis.set_minor_locator(ticker.AutoMinorLocator(n=10))
        xticks = range(max(d[0] for d in data) // 1000)
        pyplot.xticks([t for t in xticks if t % 10 == 0])
        pyplot.yticks([0, 1])
        pyplot.ylim([-0.1, 1.1])
        pyplot.plot(
            [d[0] / 1000.0 for d in data],
            [d[1] for d in data],
            'b-',
        )
        pyplot.savefig(os.path.join(dst_dir, label + '.png'), bbox_inches='tight')
        pyplot.close('all')
        print("Done generating {}\r".format(label))


if __name__ == '__main__':
    main()
