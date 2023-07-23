TEST_NAMES = ['base', 'jhash2', 'hsiphash', 'siphash']
CPUS = ['skylake', 'nehalem']

def get_summary(filename):
    f = open(filename)
    d = {}

    for line in f.read().split('\n')[:8]:
        try:
            key, value = line.split(':')
        except:
            continue

        d[key.strip()] = float(value)

    return d


def main():
    data = {}

    for cpu in CPUS:
        for test_name in TEST_NAMES:
            name = '{}_{}'.format(test_name, cpu)
            data[name] = get_summary(name + '.txt')

    for cpu in CPUS:
        base_name = 'base_' + cpu

        print(cpu.ljust(20) + '\tbase\t\t' + ''.join(map(lambda s: s.ljust(24), TEST_NAMES[1:])))

        for key in data[base_name]:
            print('{}\t{:6.2f}\t\t'.format(key.ljust(20), data[base_name][key]), end='')

            for test_name in TEST_NAMES[1:]:
                name = '{}_{}'.format(test_name, cpu)
                print('{:6.2f} ({:3.2f})\t\t'.format(data[name][key],
                                                   data[name][key] * 100 / data[base_name][key]),
                      end='')

            print()

        print()

if __name__ == '__main__':
    main()
