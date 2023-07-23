TEST_NAMES = ['base', 'jhash2', 'hsiphash', 'siphash']


def get_average(filename):
    f = open(filename)
    d = {}

    for line in f.readlines():
        size, throughput = line.split(':')
        size = int(size)
        throughput = float(throughput)

        if size not in d:
            d[size] = []

        d[size].append(throughput)

    for size in d:
        d[size] = sum(d[size]) / len(d[size])

    return d


def main():
    data = {}

    for test_name in TEST_NAMES:
        data[test_name] = get_average(test_name + '.txt')

    print('size\tbase\t\t' + ''.join(map(lambda s: s.ljust(24), TEST_NAMES[1:])))

    for size in data['base']:
        print('{}\t{:6.2f}\t'.format(size, data['base'][size]), end='')

        for test_name in TEST_NAMES[1:]:
            print('{:6.2f} ({:3.2f})\t'.format(data[test_name][size],
                                     data[test_name][size] * 100 / data['base'][size]),
                  end='')

        print()


if __name__ == '__main__':
    main()
