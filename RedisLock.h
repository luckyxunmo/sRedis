 


#ifndef _REDIS_LOCK_H_
#define _REDIS_LOCK_H_

#ifdef WIN32
#include <Windows.h>
#else
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#endif



class RedisLock {
public:
    RedisLock();
    virtual ~RedisLock();
    void Enter();
    void Leave();

private:
#ifdef WIN32
    CRITICAL_SECTION    mSection;
#else
    pthread_mutex_t     mMutex;
#endif
};

class RedisAutoLock {
public:
    RedisAutoLock(RedisLock& lock);
    virtual ~RedisAutoLock();
private:
    RedisLock& mlock;
};



#define GREDISLOCK(T) RedisAutoLock lock(T)

#endif /* _GREDIS_LOCK_H_ */