# multi-pc-multi-thread

## 介绍
Gitee 是 OSCHINA 推出的基于 Git 的代码托管平台（同时支持 SVN）。专为开发者提供稳定、高效、安全的云端软件开发协作平台无论是个人、团队、或是企业，都能够用 Gitee 实现代码托管、项目管理、协作开发。企业项目请看 [https://gitee.com/enterprises](https://gitee.com/enterprises)}

## 软件架构
软件由客户端和服务端组成。在多机多线程的步骤中，由服务端实现结果的归并。


## 安装教程

1.  编译myServer.cpp
2.  编译myClient.cpp
3.  注意在编译中加入，以解决线程库问题："-lpthread"
4.  注意在编译中加入，以解决内存不足的问题："-mcmodel=large"
5.  详情见报告"6. 重现时注意"（./doc/xxx.docx or ./doc/xxx.pdf）

## 使用说明

1.  先运行myServer: `$ ./myServer`
2.  待myServer将单机多线程算完以后，将开启网络监听。
3.  此时再运行myClient: `$ ./myClient`

---
## 备注

具体结果详见报告的测试部分