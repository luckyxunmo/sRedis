/* 
 * File:   asyncRedis.h
 * Author: peifeng.li
 *
 * Created on 2017年6月29日, 上午9:41
 */

#ifndef ASYNCREDIS_H
#define ASYNCREDIS_H

#include "sessionRedis.h"
#include<string.h>
#include <sstream>
#include <pthread.h>

bool checkReply(const redisReply* const reply)
{
    if(NULL == reply)
    {
        return false;
    }
    switch(reply->type)
    {
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_ARRAY:
        case REDIS_REPLY_INTEGER:
              return true;
        case REDIS_REPLY_STATUS:
              return (0 == strcasecmp(reply->str,"OK"));
        case  REDIS_REPLY_NIL:
        case  REDIS_REPLY_ERROR: 
        default:
              return false;       
    }
}
//设置命令执行后的回调函数
void getCallback(redisAsyncContext *c, void *r, void *privdata) {
	redisReply *reply = static_cast<redisReply*>(r);
	if (reply == NULL) return;
	vector<string> result;
	if(checkReply(reply))
    {
         for (size_t j = 0; j < reply->elements; j++) 
	      {         
                if(reply->element[j]->str)
                {
                     result.push_back(string(reply->element[j]->str));
                }
	                        
                else if(reply->element[j]->integer)
                {
		    std::ostringstream oss;
		    oss << reply->element[j]->integer ;
                    result.push_back(oss.str());
                }
                    
          }     
        
    }
	
vector<string>::iterator iter = result.begin();
for(; result.end() != iter; iter++)
	cout << *iter <<endl;
	/* Disconnect after receiving the reply to GET */
	//redisAsyncDisconnect(c);
	//sleep(1);
}

//设置连接回调函数
void connectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	printf("Connected...\n");
}

//设置断开连接回调函数
void disconnectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	printf("Disconnected...\n");
}


void* fun(void* argv)
{
	cout<<"it is child thread"<<endl;

   
	struct event_base *base = event_base_new();//新建一个libevent事件处理

	redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 8888);//新建异步连接
	if (c->err) {
	    printf("Error: %s\n", c->errstr);	
	}
	redisLibeventAttach(c,base);//将连接添加到libevent事件处理
	redisAsyncSetConnectCallback(c,connectCallback);//设置连接回调
	redisAsyncSetDisconnectCallback(c,disconnectCallback);//设置断开连接回调
        redisAsyncCommand(c, getCallback, NULL, "SUBSCRIBE __keyevent@0__:expired",NULL,NULL);//发送set命令
	event_base_dispatch(base);//开始libevent循环。注意在这一步之前redis是不会进行连接的，前边调用的命令发送函数也没有真正发送命令
}
void subscribe()
{
   pthread_t id;
  int ret = pthread_create(&id,NULL,fun,NULL);
  cout<<"This is the main process"<<endl;
  pthread_join(id,NULL);
}

#endif /* ASYNCREDIS_H */

