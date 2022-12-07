import os
import ctypes
import random
import numpy as np
from time import time

SIZE = 500


class MyMatrix:
    def __init__(self, rows=0, cols=0):
        self.rows = rows
        self.cols = cols
        self.data = None
        self.initMatrix()

    def multiply(self, other):
        result = MyMatrix(self.rows, other.cols)

        for i in range(self.rows):
            for j in range(other.cols):
                for k in range(self.cols):
                    result.data[i][j] += self.data[i][k] * other.data[k][j]

        return result

    def initMatrix(self):
        self.data = [[] for _ in range(self.rows)]
        for i in range(self.rows):
            self.data[i] = [0 for _ in range(self.cols)]

    @staticmethod
    def randMatrix(rows, cols, upper):
        result = MyMatrix(rows, cols)

        for i in range(rows):
            for j in range(cols):
                result.data[i][j] = random.randint(0, upper - 1)

        return result

    def __str__(self):
        ret = "["
        for i in range(self.rows):
            if i != 0:
                ret += " "
            for j in range(self.cols):
                ret += str(self.data[i][j])
                if i != self.rows - 1 or j != self.cols - 1:
                    ret += ", "
            if i != self.rows - 1:
                ret += "\n"
        ret += "]"

        return ret


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
    print("python normal: duration = {:.4f} s".format(time_nm))

    # Numpy
    a = np.array(a.data)
    b = np.array(b.data)
    start = time()
    c = a @ b
    # print(c)
    end = time()
    time_np = end - start
    print("numpy: duration = {:.4f} s, acc = {:.2f}".format(
        time_np, time_nm / time_np))

    # CPP
    lib_name = "multiply_test"
    lib_path = os.path.join(os.path.dirname(__file__),
                            "./{}.so".format(lib_name))
    lib = ctypes.cdll.LoadLibrary(os.path.abspath(lib_path))

    # normal
    start = time()
    lib.multiply(SIZE, 0, False)
    end = time()
    time_cpp = end - start
    print("cpp ijk: duration = {:.4f} s, acc = {:.2f}".format(
        time_cpp, time_nm / time_cpp))

    # ikj acc
    start = time()
    lib.multiply(SIZE, 3, False)
    end = time()
    time_cpp = end - start
    print("cpp ikj acc: duration = {:.4f} s, acc = {:.2f}".format(
        time_cpp, time_nm / time_cpp))


if __name__ == "__main__":
    main()
