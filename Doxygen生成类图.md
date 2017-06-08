# 使用Doxygen生成类图

> 万事开头难，阅读live555源码总想记录些什么又不知道从何下手于是就从最开始的生成类图讲起吧。因为我也是刚开始接触这个框架。为了便于厘清live555中各个类模块的关系，考虑使用doxygen生成相关文档，便于查看其中类继承关系以及调用关系。整个记录过程线简单粗略以后完成进一步精简详细。以至于对其框架和细节充分了解，最终可以达到对框架进行完善和补充的目的。

---

## Doxygen生成步骤

> 使用Doxygen生成HTML文件主要要配置Wizard、Expert两项，下面详细介绍如何配置这两项。

### 一、配置Wizard

1. 配置Project:设置源文件目录以及生成目录，记住勾选Scan Recursively

2. 配置Output:设置HTML，不用LaTeX

3. 配置Diagrams:设置Use dot tool form the GraphViz package,然后勾选下面的全部选项

### 二、配置Expert

1. 配置build：勾选Extract_All、Extract_PRIVATE、Extract_PACKAGE、Extract_static、Extract_local_classes、Extract_local_methods

2. 配置Dot：勾选Class_Diagrams、勾选Have_dot、设置Dot_path

> 然后run,就可以生成live555相关文档了。

## 类图说明

> 具体类图请参考生成的框架，并补充类图间的相关信息说明，适当的描述live555的架构。现在不太好描述，等以后熟悉了在描述。

## 说明

> 本文只是记录使用Doxygen生成live555的相关文档的文件。具体Doxygen的使用规则请参考其他网上说明