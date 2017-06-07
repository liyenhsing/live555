#include <BasicUsageEnvironment.hh>
#include "liveMedia.hh"
static ServerMediaSession* createNewSMS(UsageEnvironment& env,char const* fileName);
int main()
{
	TaskScheduler* scheduler;
	UsageEnvironment* env ;
	RTSPServer* rtspServer;
	ServerMediaSession* sms;
	//创建任务调用器
	scheduler = BasicTaskScheduler::createNew();
	//创建交互环境
	env = BasicUsageEnvironment::createNew(*scheduler);
	//创建RTSP服务器
	rtspServer = RTSPServer::createNew(*env,554); //使用554端口
	if(rtspServer == NULL)
	{
		rtspServer = RTSPServer::createNew(*env,8554); //554端口被占用，就使用8554端口
	}
	//打印服务器地址
	*env << "Play streams from this server using the URL\n\t"
	<< rtspServer->rtspURLPrefix() << "<filename>.\n";
	//创建session
	sms = createNewSMS(*env, "test_video.264");
	rtspServer->addServerMediaSession(sms);
	//添加其它文件对应的session...
	//进行事件循环
	env->taskScheduler().doEventLoop(); // does not return
	return 0;
}
static ServerMediaSession* createNewSMS(UsageEnvironment& env,char const* fileName)
{
	char const* extension = strrchr(fileName,'.');
	ServerMediaSession* sms = NULL;
	Boolean reuseSource = False; //不重用source
	sms = ServerMediaSession::createNew(env, fileName); //创建一个session
	//
	//在session中添加subssesion
	//
	if(strcmp(extension, ".aac") == 0)
	{
		sms->addSubsession(ADTSAudioFileServerMediaSubsession::createNew(env, fileName, reuseSource));
	}
	else if(strcmp(extension, ".264") == 0)
	{
		sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
	}
	else if(strcmp(extension, ".mpg") == 0)
	{
		MPEG1or2FileServerDemux* demux
		= MPEG1or2FileServerDemux::createNew(env, fileName, reuseSource);
		sms->addSubsession(demux->newVideoServerMediaSubsession());
		sms->addSubsession(demux->newAudioServerMediaSubsession());
	}
	//添加其它媒体格式支持...
	return sms;
}   
