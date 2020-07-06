# 基于muduo实现的网络库 
其实网络库的大部分内容都是按照书上来设计的，其中日志模块部分的封装思路是参照sylar所写的。后续尽量把example里面的程序都加上性能测试报告~
目前只有memcached有用压测和火焰图分析过。

## 目录结构
```$xslt
/
   /example  几个常用的示范程序
      /http  简单的HTTP服务器，实现了CGI接口但是还没有验证
      /memcached  实现简单的memcached协议，目前用memaslap，性能报告在readme里面贴出来了
      /raft_server raft的服务端
   /skylu  网络库
      /base 基本的一些数据结构和常用的组件 
      /net 网络库的主要内容
```

## 如何使用
cmake >= 2.8.12
g++ >= 4.8
git clone 到项目所在的目录，引入头文件就可以了。


## 关于 Raft
这部分的设计是在实习时后面leader 让我为KV服务器进行多机部署考虑的时候去了解的。网上对于这方面的资料、论文很多。这里就不一一赘述了。
大部分代码写完了，但是还没有时间去验证。 
目前是打算后续用容器+Jepsen框架进行一致性测试。（有时间的话）
 