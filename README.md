# multi-pc-multi-thread

## 安装教程
1.  编译myServer.cpp (可以直接使用vscode, `ctrl+shift+B`, 已经配置好`task.json`)
2.  编译myClient.cpp (同上)
3.  注意在编译中加入，以解决线程库问题："-lpthread"
4.  注意在编译中加入，以解决内存不足的问题："-mcmodel=large"
5.  详情见报告"6. 重现时注意"（./doc/xxx.docx or ./doc/xxx.pdf）

## 使用说明
1.  先运行myServer: `$ ./myServer`
2.  待myServer将单机多线程算完以后，将开启网络监听。
3.  此时再运行myClient: `$ ./myClient`

具体结果详见报告的测试部分。
