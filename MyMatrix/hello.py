import ctypes

lib = ctypes.cdll.LoadLibrary("./test1.so")
lib.test()
