# Live555Server运行框架

> 接触Live555不久后为了进一步了解其使用方法，测试了其自带的Server例子以及在网上收集了相关例程用于测试。

## 测试方法

> 在一个PC上运行RTSpServer,然后使用VLC串流模式播放RTSPServer上的文件。测试环境Win7

---

## Server运行框架

> 本文主要参考Live555学习笔记以及Live555源码后完成的相关文档。

### 一、Server端初始化到建立连接完成的工作

> 1.创建运行环境以及调度后，在创建Server时会调用CreateNew(),此时，会调用setUpOurSocket函数，该函数的具体源码如下：

```
//setUpOurSocket 在GenericMediaServer中定义
int GenericMediaServer::setUpOurSocket(UsageEnvironment& env, Port& ourPort) {
  int ourSocket = -1;
  
  do {
    // The following statement is enabled by default.
    // Don't disable it (by defining ALLOW_SERVER_PORT_REUSE) unless you know what you're doing.
#if !defined(ALLOW_SERVER_PORT_REUSE) && !defined(ALLOW_RTSP_SERVER_PORT_REUSE)
    // ALLOW_RTSP_SERVER_PORT_REUSE is for backwards-compatibility #####
    NoReuse dummy(env); // Don't use this socket if there's already a local server using it
#endif
    
    ourSocket = setupStreamSocket(env, ourPort);
    if (ourSocket < 0) break;
    
    // Make sure we have a big send buffer:
    if (!increaseSendBufferTo(env, ourSocket, 50*1024)) break;
    
    // Allow multiple simultaneous connections:
    if (listen(ourSocket, LISTEN_BACKLOG_SIZE) < 0) {
      env.setResultErrMsg("listen() failed: ");
      break;
    }
    
    if (ourPort.num() == 0) {
      // bind() will have chosen a port for us; return it also:
      if (!getSourcePort(env, ourSocket, ourPort)) break;
    }
    
    return ourSocket;
  } while (0);
  
  if (ourSocket != -1) ::closeSocket(ourSocket);
  return -1;
}

```
> 其主要功能是创建socket并监听。在createNew时会创建一个RTPServer，创建RTPserver时会调用到基类的构造函数：

```
GenericMediaServer
::GenericMediaServer(UsageEnvironment& env, int ourSocket, Port ourPort,
		     unsigned reclamationSeconds)
  : Medium(env),
    fServerSocket(ourSocket), fServerPort(ourPort), fReclamationSeconds(reclamationSeconds),
    fServerMediaSessions(HashTable::create(STRING_HASH_KEYS)),
    fClientConnections(HashTable::create(ONE_WORD_HASH_KEYS)),
    fClientSessions(HashTable::create(STRING_HASH_KEYS)) {
  ignoreSigPipeOnSocket(fServerSocket); // so that clients on the same host that are killed don't also kill us
  
  // Arrange to handle connections from others:
  env.taskScheduler().turnOnBackgroundReadHandling(fServerSocket, incomingConnectionHandler, this);
}

```

> 此时，会将incomingConnectionHandler与Server的socket号绑定，放入handlerSet中在LOOP中调用,相关内容可以参考Live555框架。incomingConnectionHandler具体代码如下


```
// incomingConnectionHandler具体在GenericMediaServer中实现。
// incomingConnectionHandler内部又调用了incomingConnectionHandlerOnSocket
// incomingConnectionHandlerOnSocket的源码如下：

void GenericMediaServer::incomingConnectionHandlerOnSocket(int serverSocket) {
  struct sockaddr_in clientAddr;
  SOCKLEN_T clientAddrLen = sizeof clientAddr;
  int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
  if (clientSocket < 0) {
    int err = envir().getErrno();
    if (err != EWOULDBLOCK) {
      envir().setResultErrMsg("accept() failed: ");
    }
    return;
  }
  ignoreSigPipeOnSocket(clientSocket); // so that clients on the same host that are killed don't also kill us
  makeSocketNonBlocking(clientSocket);
  increaseSendBufferTo(envir(), clientSocket, 50*1024);
  
#ifdef DEBUG
  envir() << "accept()ed connection from " << AddressString(clientAddr).val() << "\n";
#endif
  
  // Create a new object for handling this connection:
  (void)createNewClientConnection(clientSocket, clientAddr);
}

//createNewClientConnection会实例化一个RTSPClientConnection
GenericMediaServer::ClientConnection*
RTSPServer::createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr) {
  return new RTSPClientConnection(*this, clientSocket, clientAddr);
}

//这个RTSPClientConnection最终会使用基类，而基类中创建了一个handling用于连接时候处理
GenericMediaServer::ClientConnection
::ClientConnection(GenericMediaServer& ourServer, int clientSocket, struct sockaddr_in clientAddr)
  : fOurServer(ourServer), fOurSocket(clientSocket), fClientAddr(clientAddr) {
  // Add ourself to our 'client connections' table:
  fOurServer.fClientConnections->Add((char const*)this, this);
  
  // Arrange to handle incoming requests:
  resetRequestBuffer();
  envir().taskScheduler()
    .setBackgroundHandling(fOurSocket, SOCKET_READABLE|SOCKET_EXCEPTION, incomingRequestHandler, this);
}
```

> 由代码可知创建Server以后，经过层层封装最终实现socket创建以及监听端口以及完成创建连接的回调函数注册。创建的回调函数又实现了连接建立后收到数据时回调函数注册。

### 二、建立连接后RTP数据发送

> 建立连接后Server等待客户端发起Request,收到客户端发送的数据后Server使用incomingRequestHandler去处理。incomingRequestHandler的源码如下：

```
void GenericMediaServer::ClientConnection::incomingRequestHandler() {
  struct sockaddr_in dummy; // 'from' address, meaningless in this case
  
  int bytesRead = readSocket(envir(), fOurSocket, &fRequestBuffer[fRequestBytesAlreadySeen], fRequestBufferBytesLeft, dummy);
  handleRequestBytes(bytesRead);
}

//在handleRequestBytes中会处理Setup命令

 if (authenticationOK("SETUP", urlTotalSuffix, (char const*)fRequestBuffer)) {
	    clientSession
	      = (RTSPServer::RTSPClientSession*)fOurRTSPServer.createNewClientSessionWithId();
 } 
//此时创建RTPSession
```

### 三、发送数据

> 连接建立好后，在收到Play命令的时候会发送数据发送数据的代码如下：

```
void RTSPServer::RTSPClientSession
::handleCmd_PLAY(RTSPServer::RTSPClientConnection* ourClientConnection,
		 ServerMediaSubsession* subsession, char const* fullRequestStr)
```

> 由于代码太长因此具体的请看RTSPClientSession::handleCmd_PLAY的实现情况。

> 整个Server的连接建立数据发送以及会话结束都已经通过上述文段进行了一个粗略的描述。还有一些细节如：一些析构的具体细节以及SDP生成过程还需要自己仔细查阅代码。

> 补充RTP发送数据的细节，因为RTP可以通过TCP和UDP发送数据

---

## 总结

> live555 server主要实在GenericMediaServer中实现，GenericMediaServer中比较重要的两个内部类是：RTSPClientConnecttion以及RTSPClientSession这两个类分别用来处理RTSP客户端连接以及客户端RTP会话。RTSP协议中发送的DESCRIBE命令实在RTSPClientConnection中实现，因为创建RTSPClientSession是在收到SETUP命令发生时。因此在之前处理的命令都放在RTPClientConnect中完成，而RTSPClientConnection中的handleCmd_DESCRIBE虚函数用来处理收到DESCRIBE命令后的处理。会话后的RTSP控制命令在RTPClientSession中完成处理，主要包括: handleCmd_SETUP handleCmd_TEARDOWN handleCmd_play handleCmd_pause.每个RTSPClientConnection以及每个RTSPClientSession内部都有一个owner类在创建的时候会把owner设置到其中。尤其是在Describe命令收到时会调用owner的lookupServerMediaSession用来查询媒体。

> 因此如果要完成对RTSP命令的处理就需要重载这些虚函数。

