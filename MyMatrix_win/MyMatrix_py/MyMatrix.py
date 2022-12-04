import os
import ctypes
import random
import numpy as np
from time import time

SIZE = 100


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
    lib_name = "multiply_lib"
    lib_path = os.path.join(os.path.dirname(__file__), "../x64/Debug/{}.dll".format(lib_name))
    print(lib_path)
    lib = ctypes.windll.LoadLibrary(os.path.abspath(lib_path))
    #lib.multiply.restype = ctypes.c_double

    start = time()
    test_CPP(lib, 0)
    end = time()
    time_cpp = end - start
    print("cpp:normal time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))

    start = time()
    test_CPP(lib, 1)
    end = time()
    time_cpp = end - start
    print("cpp:ikj time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))

    start = time()
    test_CPP(lib, 2)
    end = time()
    time_cpp = end - start
    print("cpp:jki time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))

    start = time()
    test_CPP(lib, 3)
    end = time()
    time_cpp = end - start
    print("cpp:ikj acc time = {:.4f}".format(time_cpp))
    print("acc = {:.2f}\n".format(time_nm / time_cpp))


if __name__ == "__main__":
    main()
