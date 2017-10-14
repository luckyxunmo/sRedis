/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   redisReply.h
 * Author: peifeng.li
 *
 * Created on 2017年9月21日, 下午3:04
 */

#ifndef REDISREPLY_H
#define REDISREPLY_H

#include <hiredis/hiredis.h>
#include <vector>
#include <string>

using std::string;

class RedisReply{
public:
    RedisReply(){}
    RedisReply(redisReply* reply)
    {
        type = reply->type;
        integer = reply->integer;
        len = reply->len;
        if(reply->str != NULL)
        {
          str = reply->str;
        }
        elements = reply->elements;
        if(type == REDIS_REPLY_ARRAY)
        {
            for (size_t j = 0; j < reply->elements; j++) 
           {         
              if(reply->element[j]->str)
             {
                element.push_back(string(reply->element[j]->str));
             }                 
             else
             {
                element.push_back(string());
             }         
           }      
        }
    }

    ~RedisReply(){}
    
    bool checkReply()
    {
        switch(type)
    {
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_ARRAY:
        case REDIS_REPLY_INTEGER:
              return true;
        case REDIS_REPLY_STATUS:
            return (0==str.compare("OK") || 0==str.compare("PONG"));
              
        case  REDIS_REPLY_NIL:
            return false;
        case  REDIS_REPLY_ERROR: 
        default:
              return false;       
    }
    }
public:
    int type; /* REDIS_REPLY_* */
    long long integer; /* The integer when type is REDIS_REPLY_INTEGER */
    int len; /* Length of string */
    string str; /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
    size_t elements; /* number of elements, for REDIS_REPLY_ARRAY */
    std::vector<std::string> element; /* elements vector for REDIS_REPLY_ARRAY */
    string error;
    
};


#endif /* REDISREPLY_H */

