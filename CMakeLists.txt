cmake_minimum_required(VERSION 2.6)
PROJECT(test)

ADD_DEFINITIONS(-D_GLIBCXX_USE_NANOSLEEP )
set(CMAKE_CXX_FLAGS "-std=c++0x  -pthread")
INCLUDE_DIRECTORIES( 
	../../team/quantdo/public/include/hiredis/
        ../../team/quantdo/public/include/
       ../../team/quantdo/src/
 )
 
	
	LINK_DIRECTORIES( 
    ../../team/quantdo/public/lib/
	/usr/local/lib 
     )
	
	

add_executable(test test.cc sessionRedis.cpp RedisLock.cpp)
TARGET_LINK_LIBRARIES(test hiredis event ) 

