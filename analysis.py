"""
Creation of graphics
"""
import pandas
import seaborn
from matplotlib import pyplot


def main():
    """
    Main function
    """

    dataframe = pandas.read_csv('data.csv')
    seaborn.lmplot(x='expr_len', y='time', hue="parser", data=dataframe,
                   truncate=True, size=8, aspect=1.2)
    pyplot.xlim(0, 16)
    pyplot.yticks(range(0, 16))
    # pyplot.yscale("log")
    pyplot.grid(True)
    # pyplot.show()
    pyplot.savefig('benchmark.png')


if __name__ == "__main__":
    main()
