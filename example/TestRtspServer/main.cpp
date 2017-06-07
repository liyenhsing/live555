#include "TestRtspServer.h"

int main(int argc, char const *argv[])
{
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
	
	//存储用户名和密码的数据结构
	UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
	// Repeat the above with each <username>, <password> that you wish to allow
	// access to the server.
#endif

  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
	RTSPServer* rtspServer;
	//server的端口号
	portNumBits rtspServerPortNum = 554;
	//创建RTSP的服务器（RTSPServer::createNew）
	rtspServer = TestRtspServer::createNew(*env, rtspServerPortNum, authDB);
	if (rtspServer == NULL) {
		//如果上面没有成功创建server,那么后面继续换一个端口，重新创建一次
		rtspServerPortNum = 8554;
		rtspServer = TestRtspServer::createNew(*env, rtspServerPortNum, authDB);
	}
	if (rtspServer == NULL) {
		//server创建失败，报错，并退出
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	*env << "LIVE555 Media Server\n";
	//拿到IP地址  如：rstp://192.168.2.183
	//bind操作也在这个阶段
	char* urlPrefix = rtspServer->rtspURLPrefix(); 
	*env << "Play streams from this server using the URL\n\t"
		<< urlPrefix << "<filename>\nwhere <filename> is a file present in the current directory.\n";
	*env << "Each file's type is inferred from its name suffix:\n";
	*env << "\t\".264\" => a H.264 Video Elementary Stream file\n";
	*env << "\t\".265\" => a H.265 Video Elementary Stream file\n";
	*env << "\t\".aac\" => an AAC Audio (ADTS format) file\n";
	*env << "\t\".ac3\" => an AC-3 Audio file\n";
	*env << "\t\".amr\" => an AMR Audio file\n";
	*env << "\t\".dv\" => a DV Video file\n";
	*env << "\t\".m4e\" => a MPEG-4 Video Elementary Stream file\n";
	*env << "\t\".mkv\" => a Matroska audio+video+(optional)subtitles file\n";
	*env << "\t\".mp3\" => a MPEG-1 or 2 Audio file\n";
	*env << "\t\".mpg\" => a MPEG-1 or 2 Program Stream (audio+video) file\n";
	*env << "\t\".ogg\" or \".ogv\" or \".opus\" => an Ogg audio and/or video file\n";
	*env << "\t\".ts\" => a MPEG Transport Stream file\n";
	*env << "\t\t(a \".tsx\" index file - if present - provides server 'trick play' support)\n";
	*env << "\t\".vob\" => a VOB (MPEG-2 video with AC-3 audio) file\n";
	*env << "\t\".wav\" => a WAV Audio file\n";
	*env << "\t\".webm\" => a WebM audio(Vorbis)+video(VP8) file\n";
	*env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";

	// Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
	// Try first with the default HTTP port (80), and then with the alternative HTTP
	// port numbers (8000 and 8080).
	//listen的操作也在这个阶段
	if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
		//打印这个server的http的端口
		*env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
	}
	else {
		*env << "(RTSP-over-HTTP tunneling is not available.)\n";
	}
	//死循环，处理可读的socket和时间
	env->taskScheduler().doEventLoop(); //does not return
	return 0; // only to prevent compiler warning
}