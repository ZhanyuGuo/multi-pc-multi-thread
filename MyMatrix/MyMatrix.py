from time import time
import numpy as np
import random

SIZE = 500


class MyMatrix:
    def __init__(self, row=0, column=0):
        self.row = row
        self.column = column
        self.data = None
        self.initMatrix()

    def multiply(self, other):
        result = MyMatrix(self.row, other.column)

        for i in range(self.row):
            for j in range(other.column):
                for k in range(self.column):
                    result.data[i][j] += self.data[i][k] * other.data[k][j]

        return result

    def initMatrix(self):
        self.data = [[] for _ in range(self.row)]
        for i in range(self.row):
            self.data[i] = [0 for _ in range(self.column)]

    @staticmethod
    def randMatrix(row, column, upper):
        result = MyMatrix(row, column)

        for i in range(row):
            for j in range(column):
                result.data[i][j] = random.randint(0, upper - 1)

        return result


def main():
    a = np.random.randint(10, size=(SIZE, SIZE))
    b = np.random.randint(10, size=(SIZE, SIZE))
    start = time()
    a * b
    end = time()
    time_np = end - start
    print("numpy time = ", time_np)

    a = MyMatrix.randMatrix(SIZE, SIZE, 10)
    b = MyMatrix.randMatrix(SIZE, SIZE, 10)
    start = time()
    a.multiply(b)
    end = time()
    time_nm = end - start
    print("normal time = ", time_nm)
    print("acc = ", time_nm / time_np)

    pass


if __name__ == "__main__":
    main()
    pass

