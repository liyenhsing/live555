# UsageEnvironment源码阅读

> 一些说明

## UsageEnvironment 继承关系

> UsageEnvironment 继承关系：UsageEnvironment -> BasicUsageEnvironment0 -> BasicUsageEnvironment

> TaskScheduler 继承关系：TaskScheduler -> BasicTaskScheduler0 -> BasicTaskScheduler。

> 其中UsageEnvironment以及BasicUsageEnvironment0中含有纯虚函数，正真对外提供接口的为BasicUsageEnvironment，且由于BasicUsageEnvironment的构造函数为protected因此BasicUsageEnvironment只能通过其内部static函数createNew创建。

> TaskSheduler以及BasicTaskScheduler0中也包含纯虚函数，对外提供接口的为BasicTaskScheduler。且和UsageEnvironment类似，由于其构造函数是protected的所以其创建只能通过createNew完成。TaskSheduler采用socket中的select模型，select模型貌似有其自身的限制因此taskSheduler为单线程模型。nodejs以及libuv同样也是采用这种方式，只是libuv使用的是epoll或完成端口模型。（select模型的具体局限请查阅相关文档）

## 基础的数据结构

## UsageEnvironment 重要接口说明

## TaskScheduler 重要接口

## 参考网址

## 说明

> 具体的请参考live555生成的文档。可以查看UsageEnvironment和TaskScheduler类说明。