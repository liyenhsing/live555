# RTPClient 运行框架

> 测试RTPClient，对live555自带的openRTSP进行分析。首先是完成编译。将live555中RTSPClient例子修改了argc和argv部分的参数然后重新使用断点查看rtpclient堆栈信息。

## 一、RTSPSocket连接创建

> RTSPClient和RTSPServer整个过程是互补的，首先建立运行环境，然后设置任务信息。在创建RTSPClient，实例化RTSPClient时会使用如下代码:

```

RTSPClient::RTSPClient(UsageEnvironment& env, char const* rtspURL,
		       int verbosityLevel, char const* applicationName,
		       portNumBits tunnelOverHTTPPortNum, int socketNumToServer)
  : Medium(env),
    desiredMaxIncomingPacketSize(0), fVerbosityLevel(verbosityLevel), fCSeq(1),
    fAllowBasicAuthentication(True), fServerAddress(0),
    fTunnelOverHTTPPortNum(tunnelOverHTTPPortNum),
    fUserAgentHeaderStr(NULL), fUserAgentHeaderStrLen(0),
    fInputSocketNum(-1), fOutputSocketNum(-1), fBaseURL(NULL), fTCPStreamIdCount(0),
    fLastSessionId(NULL), fSessionTimeoutParameter(0), fSessionCookieCounter(0), fHTTPTunnelingConnectionIsPending(False) {
  setBaseURL(rtspURL);

  fResponseBuffer = new char[responseBufferSize+1];
  resetResponseBuffer();

  if (socketNumToServer >= 0) {
    // This socket number is (assumed to be) already connected to the server.
    // Use it, and arrange to handle responses to requests sent on it:
    fInputSocketNum = fOutputSocketNum = socketNumToServer;
    envir().taskScheduler().setBackgroundHandling(fInputSocketNum, SOCKET_READABLE|SOCKET_EXCEPTION,
						  (TaskScheduler::BackgroundHandlerProc*)&incomingDataHandler, this);
  }

  // Set the "User-Agent:" header to use in each request:
  char const* const libName = "LIVE555 Streaming Media v";
  char const* const libVersionStr = LIVEMEDIA_LIBRARY_VERSION_STRING;
  char const* libPrefix; char const* libSuffix;
  if (applicationName == NULL || applicationName[0] == '\0') {
    applicationName = libPrefix = libSuffix = "";
  } else {
    libPrefix = " (";
    libSuffix = ")";
  }
  unsigned userAgentNameSize
    = strlen(applicationName) + strlen(libPrefix) + strlen(libName) + strlen(libVersionStr) + strlen(libSuffix) + 1;
  char* userAgentName = new char[userAgentNameSize];
  sprintf(userAgentName, "%s%s%s%s%s", applicationName, libPrefix, libName, libVersionStr, libSuffix);
  setUserAgentString(userAgentName);
  delete[] userAgentName;
}

```

> RTSPClient 是在第一次发送命令的时候创建的socket,具体调用是在sendRequest发生，其中创建socket的函数为:openConnection


## 二、RTSP发送命令完成会话

> RTSPClient实例化后紧接着调用continueAfterClientCreation1，这个函数是playcommom中自己实现的实现，其具体代码如下：

```
void continueAfterClientCreation1() {
  setUserAgentString(userAgent);

  if (sendOptionsRequest) {
    // Begin by sending an "OPTIONS" command:
    getOptions(continueAfterOPTIONS);
  } else {
    continueAfterOPTIONS(NULL, 0, NULL);
  }
}
//开始发送option，完成socket创建，同时注册回调函数getSDPDescription

void getOptions(RTSPClient::responseHandler* afterFunc) { 
  ourRTSPClient->sendOptionsCommand(afterFunc, ourAuthenticator);
}

// getSDPDescription的具体实现如下，其完成回调函数continueAfterDESCRIBE注册
void getSDPDescription(RTSPClient::responseHandler* afterFunc) {
  ourRTSPClient->sendDescribeCommand(afterFunc, ourAuthenticator);
}

```

> 在continueAfterDESCRIBE中完成mediasession的实例化，然后调用setupStreams，startPlayingSession发送Play命令并绑定回调函数continueAfterPLAY。

## 三、RTP数据流创建

> 在continueAfterPLAY函数中会初始化MediaSession以及初始化MediaSession，这时候会调用initiate函数完成RTPsocket的建立。

``` 
if (!subsession->initiate(simpleRTPoffsetArg)) 

```

> 建立完成以后continueAfterPLAY完成流的接收与处理

## 总结

> RTSPClient端主要是在相关函数中设置回调函数，回调函数的功能主要是向server端发送RTSP命令:SETUP、OPtion、Describe、play等命令。然后设置发送命令后的回调函数完成进一步处理。其中发送option命令后完成RTSP连接建立，发送Describe后会进一步处理doafterDescribe。具体的还是看代码吧。

> 对于Client研究的不够多，主要是优点感冒头疼。没有理清整个思路。通过RTSPServer以及RTSPClient的参考完成源码阅读，大致理解了Live555源码运行框架以及RTSP命令发送时间以及RTP流建立时间。但是比较细致的如媒体相关的以及RTP打包这些细节还没有完成。紧接着完成对RTP包以及媒体流的理解。

## 参考