import os
import ctypes
import random
import numpy as np
from time import time


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


def test_CPP(lib, mode):
    return lib.multiply(SIZE, mode, False)


def main():
    print("Calculating...(about 20s on my machine)")
    # Normal
    a = MyMatrix.randMatrix(SIZE, SIZE, 10)
    b = MyMatrix.randMatrix(SIZE, SIZE, 10)
    start = time()
    a.multiply(b)
    end = time()
    time_nm = end - start
    print("normal time = {:.4f}\n".format(time_nm))

    # Numpy
    a = np.random.randint(10, size=(SIZE, SIZE))
    b = np.random.randint(10, size=(SIZE, SIZE))
    start = time()
    a * b
    end = time()
    time_np = end - start
    print("numpy time = {:.4f}".format(time_np))
    print("acc = {:.2f}\n".format(time_nm / time_np))

    # CPP
    lib_name = "multiplt_test"
    lib_path = os.path.join(os.path.dirname(__file__), "./{}.so".format(lib_name))
    lib = ctypes.cdll.LoadLibrary(os.path.abspath(lib_path))
    lib.multiply.restype = ctypes.c_double

    time_cpp = test_CPP(lib, 0)
    print("cpp:normal time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))

    time_cpp = test_CPP(lib, 1)
    print("cpp:ikj time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))

    time_cpp = test_CPP(lib, 2)
    print("cpp:jki time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))


if __name__ == "__main__":
    main()
