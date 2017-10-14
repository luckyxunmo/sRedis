#include "sessionRedis.h"
#include <vector>
#include <map>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include<string.h>
#include <sstream>
#include <pthread.h>

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::cin;
using namespace sRedis;

#define HOST "127.0.0.1"
#define PORT 8888

bool AuthenticateToken( sessionRedis& sredis,  const string& token)
{
	if(sredis.exists(token))
	  return true;
	else
	{
	   cout << "token doesn't exist"<<endl;
	   return false;
	}
}
bool AuthenticateToken( sessionRedis& sredis, const string& userId, const string& token)
{
	if(userId.empty() || token.empty())
	  return false;

	string realToken;  
	sredis.hget(userId,"Token",realToken);
	if(0==realToken.compare(token))
	  return true;
	else
	{
	    cout << "token Authenticate failed"<<endl;
		return false;
	}    
}


void TestHmset(sessionRedis& sredis)
{
   
    string key   ;
    cout << "input key"<<endl;
    cin >> key;
    
    string userID = "key 6" ; 
    string time = "12 34" ;
 
    
    std::map<string,string> attrs;
    attrs["user ID"] = userID;
    attrs["time Last"] = time;

     if(!sredis.hmset(key,attrs))
    	cout << " hmset failed" <<endl; 
}

void TestHset(sessionRedis& sredis)
{
    string hsetKey = "uu";
    cout << "key is "<<endl;

    string hsetName = "qq";
    string hsetvalue ="80";
    //cout << "name is "<< endl;
  //  cin >> hsetName; 
   // string hsetvalue = "";
 //   cout << "value is" << endl;
  //  cin >> hsetvalue;
    cout << sredis.hset(hsetKey,hsetName,hsetvalue);
        //cout<< "hset failed"<<endl;
    
}

void TestHmget(sessionRedis& sredis)
{
    string hmgetKey("uu");
   // cout << "hmgetkey is "<<endl;
   // cin >> hmgetKey;
    
    std::vector<string> names;
    names.push_back("time Last");
     names.push_back("user ID");
    /*cout << "input filed" << endl;
    string filed;
    while(cin>>filed)
    {
        if(filed == "end")
            break;
        names.push_back(filed);
    }*/
    std::vector<string> result;
    if(!sredis.hmget(hmgetKey,names,&result))
	cout << " hmget failed" <<endl;
	
    cout << "result is " <<endl;
    vector<string>::iterator iter = result.begin();
    for(;iter != result.end();iter++)
    {
	cout << *iter <<endl;
    } 
}

void TestHget(sessionRedis& sredis)
{  
    string key2;
    cout << "input key2" <<endl;
    cin >> key2;

    string name2;
    cout << "input name" <<endl;
    cin >> name2;

    string getValue;  
    if(!sredis.hget(key2,name2,getValue))
        cout << "getVaule failed" <<endl;
     else
        cout << "getVaule " << getValue <<endl;
     
}

void TestDelOneKey(sessionRedis& sredis)
{
    cout<< "input key to del"<<endl;
    string key;
    cin >> key;   
    long long  count = sredis.del_oneKey(key);      
    cout << count <<endl;
}

void TestDelkeys(sessionRedis& sredis)
{
     cout<< "input key to del"<<endl;
     string key;
     vector<string> keys;
     while(cin >> key)
    {
       if(key == "end")
         break;     
     keys.push_back(key);
    }
   cout << sredis.del_keys(keys)<<endl;
}

void TestPing(sessionRedis& sredis)
{
    if(sredis.ping())
        cout << "ping success" << endl;
    else
        cout << "ping failed" << endl;
}

void TestToken(sessionRedis& sredis)
{
    string token1("lala");
    if(AuthenticateToken(sredis,token1))
       cout<<"key"<<token1<<" exists"<<endl;
    string userId("test");
    string token("lala");
    if(AuthenticateToken(sredis,userId,token))
        cout << "success"<<endl;
}
int main(int argc, char **argv)
{
   sessionRedis sredis("127.0.0.1",8888,10000);
   bool flag =  sredis.ping();
   cout << flag <<endl;
  // TestHset(sredis);
   //TestPing(sredis);
   //TestHmget(sredis);
   TestHget(sredis);
   
    return 0;
    
    /*
    redisContext* pRedisConn = redisConnect(HOST, PORT);
    if(pRedisConn->err)
    {
        printf("connect redis server fail!\n");
        return -1;
    }
    printf("connect redis server success!\n");

    std::string key = "12345"; 
    std::string test_key = "test_key";
    std::string value = "teat value";
    redisReply* pReply = NULL;
    pReply = reinterpret_cast<redisReply *>(redisCommand(pRedisConn, "HMSET %s %s %s", key.c_str() ,test_key.c_str(), value.c_str()));
      freeReplyObject(pReply);
      redisFree(pRedisConn);
   */
return 0;

}


