#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include "RedisLock.h"
#include <hiredis/adapters/libevent.h>
#include "redisReply.h"

using namespace std;

namespace sRedis
{

#define REDIS_CONNPOOL_SIZE 128

class sessionRedis
{
  public:
 
  sessionRedis();
  sessionRedis(const char* ip , int port,int millisecond );
  //sessionRedis(const RedisConf&);
  ~sessionRedis();
 
   // 关闭mRedisConnPool中的所有连接
   void ClosePool();
   
   bool ping();
   
   bool setString(const string& key,const string& data,const unsigned int expireSeconds  = 0);
   bool getString(const string& key, string& data);
 
   /**
	 * 将多个"域-值"对添加至 KEY 对应的哈希表中
	 * HMSET: set the key's multiple fileds in redis-server
	 * @param key {const char*} 哈希表 key 值
	 *  the hash key for Hash class
	 * @param attrs {const std::map<acl::string, ...>&} the fileds in map
	 * @return {bool} 添加是否成功
	 *  if successful for HMSET command
	 */
   bool hmset(const char* key, const std::map<string,string>& attrs);
   bool hmset(const string key, const std::map<string,string>& attrs);

   /**
	 * 设置 key 对象中某个域字段的值
	 * set one field's value in the hash stored at key.
	 * @param key {const char*} key 键值
	 *  the hash key
	 * @param name {const char*} key 对象的域名称
	 *  the filed name of the hash key
	 * @param value {const char*} key 对象的域值
	 *  the filed value of the hash key
	 * @return {int} 返回值含义：
	 *  1 -- 表示新添加的域字段添加成功
	 *  0 -- 表示更新已经存在的域字段成功
	 * -1 -- 表示出错或该 key 对象非哈希对象或从结点禁止修改
	 *  return int value as below:
	 *  1 -- this is a new filed and set ok
	 *  0 -- thie is a old filed and set ok
	 * -1 -- error happend or the key is not a Hash type
	 */
   int hset(const char* key, const char* name, const char* value);
   int hset(const string key, const string name, const string value);

   /**
	 * 根据 KEY 值将多个"域-值"对从哈希表中取出
	 * get the values associated with the specified fields
	 * in the hash stored at key
	 * @param key {const char*} 哈希表 key 值
	 *  the hash key
	 * @param names 对应 key 的域值对
	 *  the given hash fileds
	 * @param result {std::vector<acl::string>*} 当该对象指针非空时存储查询结果；
     */
   bool hmget(const char* key, const std::vector<string>& names, std::vector<string>* result=NULL);
   bool hmget(const string key, const std::vector<string>& names, std::vector<string>* result=NULL);
   /**
	 * 从 redis 哈希表中获取某个 key 对象的某个域的值
	 * get the value assosiated with field in the hash stored at key
	 * @param key {const char*} key 键值
	 *  the hash key
	 * @param name {const char*} key 对象的域字段名称
	 *  the field's name
	 * @param result {acl::string&} 存储查询结果值(内部对该 string 进行内容追加)
	 *  store the value result of the given field
	 * @return {bool} 返回值含义：
	 *  true -- 成功获得对象的域字段值
	 *          get the value associated with field
	 *  false -- 域字段不存在或操作失败或该 key 对象非哈希对象
	 *           the field not exists, or error happened,
	 *           or the key isn't a hash key
	 */
   bool hget(const char* key, const char* name, string& result);
   bool hget(const string key, const string name, string& result);
   
   	/**
	 * 将字符串值 value 关联到 key
	 * set the string value of a key
	 * @param key {const char*} 字符串对象的 key
	 *  the key of a string
	 * @param value {const char*} 字符串对象的 value
	 *  the value of a string
	 * @return {bool} 操作是否成功，返回 false 表示出错或该 key 对象非字符串对象
	 *  true if SET was executed correctly, false if error happened or
	 *  the key's object isn't a string.
   *note: 对于某个原本带有生存时间（TTL）的键来说， 当 SET 命令成功在这个键上执行时， 
   *      这个键原有的 TTL 将被清除。
	 */
   bool set(const string& key, const string& value);

  /**
	 * 返回 key 所关联的字符串值
	 * get the value of a key 
	 * @param key {const char*} 字符串对象的 key
	 *  the key of a string
	 * @param buf {string&} 操作成功后存储字符串对象的值
	 *  store the value of a key after GET executed correctly
	 * @return {bool} 操作是否成功，返回 false 表示出错或 key 非字符串对象
	 *  if the GET was executed correctly, false if error happened or
	 *  is is not a string of the key
	 */
   bool get(const string& key, string& value);

  /**
	 * 删除一个或一组 KEY，
	 * delete one or some keys from redis, 
	 * @return {int} 返回所删除的 KEY 的个数，如下：
	 *  0: 未删除任何 KEY
	 *  -1: 出错
	 *  >0: 真正删除的 KEY 的个数，该值有可能少于输入的 KEY 的个数
	 *  return the number of keys been deleted, return value as below:
	 *  0: none key be deleted
	 * -1: error happened
	 *  >0: the number of keys been deleted
	 *
	 */
   int del_oneKey(const char* key);
   int del_oneKey(const string key);
   int del_keys(const vector<string>& keys);

   /*清空当前数据库中的所有的key,此命令要慎用，以免造成误操作*/
   bool delAllKeys();
   /*
   * 判断 KEY 是否存在
   * @return{bool} 返回true表示存在，否則表示出錯或者不存在
   */
   bool exists(const string key);

   /*
    *设置key的生存周期，单位是（秒）
    *@return{bool}返回TRUE表示成功，否则表示出错或key不存在
   */
   bool expire(const string key,const long second);
   
  /* 获得 key的剩余生存周期，单位（秒）
   * @return {int} 返回对应键值的生存周期
   * >0 该 key 剩余的生存周期（秒）
   * -2：key 不存在
   * -1：出錯
   * -3：当 key 存在但没有设置剩余时间
  */
   int ttl(const string key);
   bool subscribe(const string channel, std::vector<string>* result=NULL);
   RedisReply  commandArgv( const vector<string>& vCmdData);
   
private:
   std::list<redisContext*> mRedisConnPool;
   
   char m_ip[15];
   int m_port;
   char m_passWord[30];
   int m_auth;
   int db_number;
   struct timeval timeout;  
   int m_millisecond;
   
private:
   sessionRedis(sessionRedis&);
   sessionRedis& operator = (const sessionRedis&);
   
   redisContext* getConnect(); // 从pool中获取可用conn
   void FreeConn(redisContext * redisConn); //释放conn到pool中

   bool selectDb(redisContext *& connect);//选择数据库db区
   redisContext* saftGrab();
   bool isConnect(redisContext*& connect);
   void close(redisContext*& connect);

   bool checkReply(const redisReply* const reply);
   void freeReply(redisReply*& reply);
   RedisLock   mRedisLock;
};

}

