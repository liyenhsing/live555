#pragma once

/**
 * Author: ArvinPeng
 * Date:2017-6-7
 * Description :this project use to test the understanding of live555, 
 * make the framewrok of live555 clearly.
 */

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "RTSPClient.hh"
#include "RTSPServer.hh"
#include <iostream>
using namespace std;

class TestRtspServer:public RTSPServer
{
public:
	static TestRtspServer* createNew(UsageEnvironment& env, Port ourPort = 554,
					   UserAuthenticationDatabase* authDatabase = NULL,
					   unsigned reclamationTestSeconds = 65);
	
	class TestRTSPClientConnection : public RTSPClientConnection
	{
	public:
		friend class RTSPServer;
	
		TestRTSPClientConnection(TestRtspServer* ourServer, int clientSocket, struct sockaddr_in clientAddr);
		virtual ~TestRTSPClientConnection();
	protected:
		virtual void handleCmd_DESCRIBE(char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);

	private:
		TestRtspServer&	m_ourServer;

	};

	class TestRTSPClientSession : public RTSPClientSession
	{
	public:
		TestRTSPClientSession(RTSPServer& ourServer, u_int32_t sessionId);
		virtual ~TestRTSPClientSession();
	protected:
		virtual void handleCmd_SETUP(RTSPClientConnection* ourClientConnection,
						 char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);
		virtual void handleCmd_TEARDOWN(RTSPClientConnection* ourClientConnection,
				    		ServerMediaSubsession* subsession);
		virtual void handleCmd_PLAY(RTSPClientConnection* ourClientConnection,
					ServerMediaSubsession* subsession, char const* fullRequestStr);
		virtual void handleCmd_PAUSE(RTSPClientConnection* ourClientConnection,
					 ServerMediaSubsession* subsession);

			    		
	};
protected:
	TestRtspServer(UsageEnvironment &env, int ourSocket, Port ourPort,
                      UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds);
	~TestRtspServer();
	virtual ServerMediaSession *lookupServerMediaSession(char const *streamName);
	//virtual RTSPClientConnection* createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr);
	virtual RTSPClientSession* createNewClientSession(u_int32_t sessionId);
private:

};