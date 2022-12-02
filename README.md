# Jchat

## 介绍
*A simple terminal Chat program*  
一个简易的终端聊天室,包含简易的TUI界面

## 软件架构
使用了`libhv`,`cJSON`以及`ncurses`库，其中`libhv`以及`cJSON`需要手动编译，`ncurses`库可以通过各发行版的包管理工具安装

软件支持Windows(x86_x64)、Linux(x86_x64,arm)平台


## 安装教程

1.  安装所需的依赖以及工具
__根据不同的发行版有所不同__
~~~shell
$ pacman -S make cmake g++ ncurses
~~~
2.  拉取仓库
~~~shell
$ git clone https://gitee.com/Janey7695/jchat.git
$ cd jchat
$ git submodule update --init --recursive
~~~
3.  编译
~~~shell
$ make
~~~
> 编译完成后会生成build目录，最终生成的可执行文件在./build/bin中


#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

