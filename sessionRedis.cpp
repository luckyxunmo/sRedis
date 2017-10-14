#include "sessionRedis.h"
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "redisReply.h"


using namespace std;
namespace sRedis
{

sessionRedis::sessionRedis()
{

}
sessionRedis::sessionRedis(const char* ip , int port, int millisecond )
{
    strcpy(m_ip,ip);
    m_port = port;   
    m_millisecond = millisecond;
    int tv_sec = m_millisecond / 1000;  
    int millisec = m_millisecond % 1000;  
    long tv_usec = millisec * 1000;  
    timeout.tv_sec = tv_sec;  
    timeout.tv_usec = tv_usec;  
    
    memset(m_passWord,0,sizeof(m_passWord)/sizeof(char));
    m_auth = 0;//不需要密码
    db_number = 0;//0号数据库
    
}
/*
sessionRedis::sessionRedis(const RedisConf& redisConfig)
{
    if(redisConfig.addr.empty())
    {    
        memset(m_ip,0,sizeof(m_ip)/sizeof(char));
        m_port = 0;
    }
    else
    {
        size_t pos = redisConfig.addr.find(":",0);
        string ip = redisConfig.addr.substr(0,pos);
        strcpy(m_ip,const_cast<char*>(ip.c_str()));

        string port = redisConfig.addr.substr(pos+1,redisConfig.addr.length()); 
	m_port = atoi(port.c_str());
    }
    m_millisecond = redisConfig.conn_timeout *1000;
    int tv_sec = m_millisecond / 1000;  
    int millisec = m_millisecond % 1000;  
    long tv_usec = millisec * 1000;  
     
    timeout.tv_sec = tv_sec;  
    timeout.tv_usec = tv_usec; 
 } */
sessionRedis::~sessionRedis()
{
    ClosePool();
}


bool sessionRedis::ping()
{
    vector<string> vCmdData;
    vCmdData.push_back("PING");

   RedisReply reply =  commandArgv(vCmdData);
   return reply.checkReply(); 
}
   
bool sessionRedis::hmset(const char* key, const std::map<string,string>& attrs)
{
    if(attrs.empty())
	return false;
   
    vector<string> vCmdData;
    vCmdData.push_back("HMSET");
    vCmdData.push_back(key);
    
    map<string,string>::const_iterator mapIter = attrs.begin();
    while(mapIter != attrs.end())
    {
        vCmdData.push_back(mapIter->first);
        vCmdData.push_back(mapIter->second);
        mapIter++;
    }
   RedisReply reply =  commandArgv(vCmdData);
   
   return reply.checkReply();        	
}

RedisReply sessionRedis::commandArgv(const vector<string>& vCmdData)
{
    vector<const char*> argv(vCmdData.size());
    vector<size_t> argvlen(vCmdData.size());
    unsigned int j = 0;
    for(vector<string>::const_iterator i = vCmdData.begin(); i !=vCmdData.end();j++,i++)
    {
        argv[j] = i->c_str();
        argvlen[j] = i->size();
    }   
    redisContext * connect= saftGrab();
    if(connect == NULL) return false;
    
    redisReply* reply = static_cast<redisReply*>(redisCommandArgv(connect,argv.size(),&(argv[0]),&(argvlen[0]))); 
    FreeConn(connect);
    
    if(reply == NULL) 
    {   freeReply(reply);
        return RedisReply();
    }
    RedisReply reply2(reply);
    freeReply(reply);
    return reply2;
}


bool sessionRedis::hmset(const string key, const std::map<string,string>& attrs)
{
  
    if(attrs.empty())
	return false;
    else
        return hmset(key.c_str(),attrs);
}

bool sessionRedis::hmget(const char* key, const std::vector<string>& names, std::vector<string>* result)
{	
    if(names.empty() || key == NULL)
    {
        return false;
    }
    bool bRet;
    vector<string> vCmdData;
    vCmdData.push_back("HMGET");
    vCmdData.push_back(key);
    
    vector<string>::const_iterator iter = names.begin();
    while(iter!= names.end())
    {
        cout <<"names "<< *iter <<endl;
        vCmdData.push_back(*iter);
        iter++;
    }
   RedisReply reply =  commandArgv(vCmdData);
   	
    if(reply.checkReply())
    {
        *result = reply.element;    
        bRet =  true; 
    }
    else
    {
        bRet = false;
    }
    return bRet;
}

bool sessionRedis::hmget(const string key, const std::vector<string>& names, std::vector<string>* result)
{
	
    if(names.empty() || key.empty())
    {
        return false;
    }
    else
        return hmget(key.c_str(),names,result);
}

int sessionRedis::hset(const char* key, const char* name, const char* value)
{
	
    if(NULL == name || NULL == value)
    {
        return -1;
    }
    int count = -1;
    vector<string> vCmdData;
    vCmdData.push_back("HSET");
    vCmdData.push_back(key);
    vCmdData.push_back(name);
    vCmdData.push_back(value);
    
    RedisReply reply = commandArgv(vCmdData);
    if(reply.checkReply())
    {
        count = reply.integer;    
    }
    return count;  
}

int sessionRedis::hset(const string key, const string name, const string value)
{
	
    if( name.empty() || key.empty())
    {
        return -1;
    }
    else
        return hset(key.c_str(),name.c_str(),value.c_str());
}

 bool sessionRedis::hget(const char* key, const char* name, string& result)
 {
	
    if(NULL == name || NULL == key)
    {
        return false;
    }
    
    vector<string> vCmdData;
    vCmdData.push_back("HGET");
    vCmdData.push_back(key);
    vCmdData.push_back(name);
    
    RedisReply reply = commandArgv(vCmdData);
    if(reply.checkReply())
    {
        result = reply.str;   
        return true;
    }    
    return false;
 }

 bool sessionRedis::hget(const string key, const string name, string& result)
 {	 
    if( name.empty() || key.empty())
    {
        return false;
    }
    else
        return hget(key.c_str(),name.c_str(),result);
 }

int sessionRedis::del_oneKey(const char* key)
{
    if(key == NULL)
        return -1;
    int cout = -1;
     vector<string> vCmdData;
    vCmdData.push_back("DEL");
    vCmdData.push_back(key);;
    
    RedisReply reply = commandArgv(vCmdData);
    if(reply.checkReply())
    {
        return reply.integer;   
    }    
    return cout; 	 
}

int sessionRedis::del_oneKey(const string key)
{	
    if( key.empty())
        return -1;
     else
        return del_oneKey(key.c_str());

}

int sessionRedis::del_keys(const vector<string>& keys)
{	
    if(keys.empty())
        return -1;
    int cout = -1;
    
    vector<string> vCmdData;
    vCmdData.push_back("DEL");
    
    vector<string>::const_iterator iter = keys.begin();
    while(iter!= keys.end())
    {
        vCmdData.push_back(*iter);
        iter++;
    }
    
    RedisReply reply =  commandArgv(vCmdData);
    if(reply.checkReply())
    {
        return reply.integer;   
    }    
    return cout; 
}

bool sessionRedis::delAllKeys()
{  
    vector<string> vCmdData;
    vCmdData.push_back("FLUSHDB");
    RedisReply reply =  commandArgv(vCmdData);
    if(reply.checkReply())
    {
        return true;   
    }    
    return false;  
}

bool sessionRedis:: exists(const string key)
{
    if(key.empty())
        return false;
     vector<string> vCmdData;
    vCmdData.push_back("EXISTS");
    vCmdData.push_back(key);;
    
    RedisReply reply = commandArgv(vCmdData);
    if(reply.checkReply())
    {
        return true;   
    } 
    return false;
}

bool sessionRedis::expire(const string key, const long second)
{
    if(key.empty())
        return false;
    else
    {
        vector<string> vCmdData;
        vCmdData.push_back("EXPIRE");
        vCmdData.push_back(key);
        std::ostringstream oss;
	oss << second ;
         vCmdData.push_back(oss.str());
         
        RedisReply reply = commandArgv(vCmdData);
       if(reply.checkReply())
       {
           return true;   
       }  
       return false;
    }  
}

int sessionRedis::ttl(const string key)
{
    if(key.empty())
        return -3;
    vector<string> vCmdData;
    vCmdData.push_back("TTL");
    vCmdData.push_back(key);;
    
    RedisReply reply = commandArgv(vCmdData);
    if(reply.checkReply())
    {
        return reply.integer;   
    } 
    return -1;
}


void sessionRedis::close(redisContext*& connect)
{
    if(NULL != connect)
    {
        redisFree(connect);
        connect = NULL;
    }
}

 redisContext* sessionRedis::saftGrab()
 {
     redisContext* connect = NULL;
     int maxTimesReConnect = 5;
     while(NULL == (connect = getConnect()) || !isConnect(connect))
     {
         close(connect);
        // CSys::Sleep(1,0);
         if(0 >= (maxTimesReConnect--))
         {
             throw ("数据库redis连接已经断开，多次尝试无法连接成功");
         }
     }
     return connect;
     
 }
bool sessionRedis::isConnect(redisContext*& connect)
{
    if(connect->err)  return false;
    redisReply *reply =static_cast<redisReply*>(redisCommand(connect,"ping"));  
    if(checkReply(reply))
    { 
        freeReply(reply);
        return true;
    }
    else{
        freeReply(reply);
        return false;
    }
}

bool sessionRedis::selectDb(redisContext *& connect)
{
    std::ostringstream oss;
    oss << "SELECT " << db_number ;
    cout<< "【数据库执行脚本】"<< oss.str() <<endl;
    redisReply *reply =static_cast<redisReply*>(redisCommand(connect,oss.str().c_str()));
    if(checkReply(reply))
    {
        freeReply(reply); 
        return true;
    }
    else{
        freeReply(reply); 
        return false;
    }
      
}
redisContext * sessionRedis::getConnect()
{	
    redisContext * connect = NULL;
    GREDISLOCK(mRedisLock);
    if(mRedisConnPool.empty())
    {
	    connect = redisConnectWithTimeout(m_ip,m_port,timeout);
	    if(connect != NULL && 0== connect->err)
        {
            if(m_auth)//需要验证密码
            {
                string command = "auth " + string(m_passWord);
                cout<< "【数据库执行脚本】"<< command <<endl;
                redisReply *reply =static_cast<redisReply*>(redisCommand(connect,command.c_str()));
                if(!checkReply(reply))
                {
                    cout << "authenticate failed" << endl;
                    freeReply(reply);
                    close(connect);
                }
                else{    
                    freeReply(reply);
                    selectDb(connect);
                    return connect;
                }
            }
            else
            {
                selectDb(connect);
                return connect;
            }
        }else{
            if(connect)
            {
                cout<<"error is "<<connect->err<<endl;
            close(connect);
            }
        }
    }
    else
    {
	    connect = mRedisConnPool.front();
	    mRedisConnPool.pop_front();
    } 
    return connect;
}

void sessionRedis::FreeConn(redisContext* redisConn)
{
    GREDISLOCK(mRedisLock);
    if (NULL != redisConn) 
    {
    	mRedisConnPool.push_back(redisConn);
    }
}

void sessionRedis::ClosePool()
{
    GREDISLOCK(mRedisLock);
    for (std::list<redisContext*>::iterator pIter = mRedisConnPool.begin();
			mRedisConnPool.end() != pIter;++pIter)
    {
	redisContext*& pRedisConn = *pIter;
	if (NULL != pRedisConn)
	{
	    redisFree(pRedisConn);
	    pRedisConn = NULL;
	}
    }			
}

bool sessionRedis::checkReply(const redisReply* const reply)
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
              return (0 == strcasecmp(reply->str,"OK") || 0 == strcasecmp(reply->str,"PONG"));
        case  REDIS_REPLY_NIL:
        case  REDIS_REPLY_ERROR: 
        default:
            cout << reply->str<<endl;
              return false;       
    }
}

void sessionRedis::freeReply(redisReply*& reply)
{
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
}

}


    