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

    def __str__(self):
        ret = "["
        for i in range(self.row):
            if i != 0:
                ret += " "
            for j in range(self.column):
                ret += str(self.data[i][j])
                if i != self.row - 1 or j != self.column - 1:
                    ret += ", "
            if i != self.row - 1:
                ret += "\n"
        ret += "]"

        return ret


def test_CPP(lib, mode):
    return lib.multiply(SIZE, mode, False)


def main():
    print("Calculating...(about 20s on my machine)")
    # Normal
    a = MyMatrix.randMatrix(SIZE, SIZE, 10)
    b = MyMatrix.randMatrix(SIZE, SIZE, 10)
    start = time()
    c = a.multiply(b)
    # print(c)
    end = time()
    time_nm = end - start
    print("normal time = {:.4f}\n".format(time_nm))

    # Numpy
    a = np.array(a.data)
    b = np.array(b.data)
    start = time()
    c = a @ b
    # print(c)
    end = time()
    time_np = end - start
    print("numpy time = {:.4f}".format(time_np))
    print("acc = {:.2f}\n".format(time_nm / time_np))

    # CPP
    lib_name = "multiply_test"
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
