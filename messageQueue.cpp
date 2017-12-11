//============================================================================
// Name        :
// Author      : Anand
// Version     :
// Copyright   : 
// Description : Written in C++, Ansi-style

//compile as : g++ messageQueue.cpp -lpthread -lrt -std=c++11 -o msgQue
//============================================================================

#include <iostream>
#include <pthread.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <cerrno>           // for printing error no
#include <cstring>			// for strerror
#include <unistd.h>
#include <stdio.h>


#define SENDER_MSG_Q "/sender-queue"

using namespace std;

pthread_t tid_checker;
pthread_t tid_issuer;

pthread_attr_t tidAttr;

mqd_t mqd;

typedef struct Baggage
{
    bool authenticity;
    char name[10];
}BAG;

static int bagNum;
/***************************************************/
//Issuer routine

void* issuer(void*vp)
{

	BAG bg{false,"bag-1"};
	printf(" sizeof(bg): %d\n",sizeof(bg));
	sprintf(bg.name,"bag-%d",++bagNum);
	if(-1==mq_send(mqd,"Hello",6,0)) //(const char*)&bg
	{
		cout<<"Issuer:"<<strerror(errno)<<endl;
	}
	sleep(1);
    pthread_exit(0);
}

//Checker routine

void checker()
{
	BAG pkg;
	ssize_t size=0;
	printf(" sizeof(pkg): %d\n",sizeof(pkg));
	char recBuff[50]={'\0'};
	if((size = mq_receive(mqd,recBuff,50,0) )!= sizeof(pkg))
	{
		cout<<"Checker :"<<strerror(errno)<<"| Size = "<<size<<endl;
	}
	printf(" Rx : %s\n",recBuff);
    pthread_exit(0);
}

//Signal handler for ctrl+z

void sigstop(int p)
{
  // yet to define
}

/**************************************************/
int main()
{
	mq_attr mqattr;
	mqattr.mq_flags=0;
	mqattr.mq_maxmsg=10;
	mqattr.mq_msgsize=25;
	mqattr.mq_curmsgs=0;

	//initialize thread attributes
	pthread_attr_init(&tidAttr);
	if((mqd=mq_open(SENDER_MSG_Q,O_CREAT | O_RDWR | O_NONBLOCK,\
			0666,&mqattr))==-1)
	{
		cout<<strerror(errno)<<endl;
		return -1;
	}
		cout << "Mq is open" << endl;
    // typecasting is needed as (void *(*)(void *)) to compile for C++
    pthread_create(&tid_issuer,NULL,(void *(*)(void *))&issuer,NULL);
    pthread_create(&tid_checker,NULL,(void *(*)(void *))&checker,NULL);

    pthread_join(tid_issuer,NULL);
    pthread_join(tid_checker,NULL);

	return 0;
}