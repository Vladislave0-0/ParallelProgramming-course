import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('./output/results.csv')

for n, group in df.groupby('n'):
    plt.figure()
    plt.plot(group['threads'], group['qsort_time'], label='qsort')
    plt.plot(group['threads'], group['parallel_time'], label='parallel')
    plt.xlabel('Число потоков')
    plt.ylabel('Время, сек')
    plt.title(f'Сравнение для n={n}')
    plt.legend()
    plt.savefig(f'./output/plot_{n}.png')
    plt.close()
    