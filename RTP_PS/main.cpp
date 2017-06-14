#include "PSPacket.h"
#include "streamdef.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

#define INBUF_SIZE (1024*1024)
#define LOG

#define PESONLY

long GetFileLength(FILE* fp)
{
	fseek(fp, 0, SEEK_END);   //指向以文件尾为基准，偏移0个字节的位置
	int file_length = ftell(fp);  //用于得到文件位置指针当前位置相对于文件首的偏移字节数，即得到一个文件的长度
	fseek(fp , 0 , SEEK_SET);  //指向以文件头为基准
	return file_length;
}


int main(int argc, char const *argv[])
{
	FILE* file,*outFile;
	const char* filename = "c:\\out.ps";
	const char* outfilename = "out.mp4";
	file = fopen(filename,"rb");
	outFile = fopen(outfilename,"wb");

	if(file == NULL)
	{
		cout<<"can not open such file"<<endl;
		return -1;
	}

	if(outfilename == NULL)
	{
		cout<<"can not open such file"<<endl;
		return -1;
	}


	unsigned char* inbuf = new unsigned char[INBUF_SIZE];
	long fileLength = GetFileLength(file);

	int preRead = 0; //前一次读取文件的长度
	int offSet = 0;
	long alreadyRead = 0;
	int size = 0;

	while(1)
	{

		if(alreadyRead < fileLength)
		{
			if(offSet == 0)
			{
				preRead = fread(inbuf, 1, INBUF_SIZE, file);
			}
			else
			{
				cout<<"the size is "<<size<<endl;
				cout << "the offset is " << offSet << endl;
				memmove( inbuf ,inbuf + offSet , size - offSet );
				preRead = fread(inbuf+ size - offSet,1,(INBUF_SIZE-size),file);
				offSet = 0;
	
			}
			alreadyRead += preRead;
			size += preRead;
		}

		
	
		if(!is_pes_header((pes_header*)(inbuf+offSet)))
		{
			cout<<"current is not pes header"<<endl;
		}
		while(!is_pes_header((pes_header*)(inbuf+offSet)))
		{
			//pay attention to overflow
			//inbuf++;
			offSet++;
			if (offSet >= INBUF_SIZE)
				return 0;
		}
		cout<<"the begin offset is "<<offSet<<endl;
	
		pes_header_t* getPesHeader = (pes_header_t*)(inbuf + offSet);

		#ifdef LOG
		printf("the stream id is %x\n", getPesHeader->stream_id);
		printf("the length is %x\n", getPesHeader->PES_packet_length);
		#endif
		unsigned char* GetPacketLength = (unsigned char*)(&getPesHeader->PES_packet_length);
		unsigned char changetype[2];
		changetype[1] = GetPacketLength[0];
		changetype[0] = GetPacketLength[1];
		cout<<"testing"<<endl;
		unsigned short* result = (unsigned short*)changetype;
		printf("the result is %d\n",*result);

		offSet += sizeof(pes_header_t) ;
		//inbuf += sizeof(pes_header_t);
		cout << "the offset is " << offSet << endl;

		fwrite(offSet + inbuf,1,*result,outFile);

		offSet += *result;

		cout<<"the file Length is "<<fileLength<<" the Read Length "<<alreadyRead<<endl;

		

	}

	


	
	fclose(file);
	fclose(outFile);
	if(inbuf != NULL)
		delete inbuf;

	return 0;
}