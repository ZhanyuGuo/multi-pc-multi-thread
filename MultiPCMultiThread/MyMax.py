import os
import math
import ctypes
import numpy as np
from time import time
from threading import Thread
from multiprocessing import Process, Array

MAX_THREADS = 64
SUBDATANUM = 2000000
DATANUM = MAX_THREADS * SUBDATANUM


def fnThreadMax(who):
    global rawDoubleData, doubleResults

    startIndex = who * SUBDATANUM
    endIndex = startIndex + SUBDATANUM

    result = -math.inf
    for i in range(startIndex, endIndex):
        a = math.log10(math.sqrt(rawDoubleData[i]))
        if a > result:
            result = a

    doubleResults[who] = result


def maxMerge(data):
    result = -math.inf
    for item in data:
        if item > result:
            result = item

    return result


def maxNormal(ls):
    result = -math.inf
    for item in ls:
        a = math.log10(math.sqrt(item))
        if a > result:
            result = a

    return result


def main():
    # fmt: off
    global rawDoubleData, doubleResults


    # load library
    lib_name = "max_test"
    lib_path = os.path.join(os.path.dirname(__file__), "./{}.so".format(lib_name))
    lib = ctypes.cdll.LoadLibrary(os.path.abspath(lib_path))

    # init data, from cpp to py
    lib.initData.restype = ctypes.POINTER(ctypes.c_double)
    data = lib.initData()
    rawDoubleData = [data[i] for i in range(DATANUM)]

    print("Calculating...(about 15s on my machine)")
    # python normal
    start = time()
    result = maxNormal(rawDoubleData)
    end = time()
    time_nm = end - start
    print("python single threads: answer = {:.3f}, duration = {:.4f} s".format(result, time_nm))

    # 多线程是假的多线程，只能使用一个核，使用多进程可以加速
    # python acc 多线程
    threads = []
    doubleResults = [0 for _ in range(MAX_THREADS)]
    for i in range(MAX_THREADS):
        t = Thread(target=fnThreadMax, args=(i, ))
        t.setDaemon(True)
        threads.append(t)

    start = time()
    for t in threads:
        t.start()

    for t in threads:
        t.join()

    result = maxMerge(doubleResults)
    end = time()
    time_acc = end - start
    print("python multip threads: answer = {:.3f}, duration = {:.4f} s".format(result, time_acc))

    # python acc 多进程
    threads = []
    doubleResults = Array('d', [0 for _ in range(MAX_THREADS)])
    for i in range(MAX_THREADS):
        t = Process(target=fnThreadMax, args=(i, ))
        t.daemon = True
        threads.append(t)

    start = time()
    for t in threads:
        t.start()

    for t in threads:
        t.join()

    result = maxMerge(doubleResults)
    end = time()
    time_acc = end - start
    print("python multip process: answer = {:.3f}, duration = {:.4f} s".format(result, time_acc))

    # numpy
    arr = np.array(rawDoubleData)
    start = time()
    result = np.amax(np.log10(np.sqrt(arr)))
    end = time()
    time_np = end - start
    print("numpy: answer = {:.3f}, duration = {:.4f} s".format(result, time_np))

    # cpp normal
    lib.maxNormal.restype = ctypes.c_double
    start = time()
    result = lib.maxNormal()
    end = time()
    time_cpp_normal = end - start
    print("cpp single thread: answer = {:.3f}, duration = {:.4f} s".format(result, time_cpp_normal))

    # cpp acc
    lib.maxAcc.restype = ctypes.c_double
    start = time()
    result = lib.maxAcc()
    end = time()
    time_cpp_acc = end - start
    print("cpp multip thread: answer = {:.3f}, duration = {:.4f} s".format(result, time_cpp_acc))


if __name__ == "__main__":
    main()
