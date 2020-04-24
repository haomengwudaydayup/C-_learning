/*      
    日志信息 - 标记程序发生的错误 - 错误等级、时间、错误内容！
 */

#pragma once
#include<iostream>
#include<sys/time.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include<stdlib.h>

const char* Level[] = {
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
    "DEBUG"
};

enum LogLevel{
    INFO = 0,
    WARNINGS,
    ERROR,
    FATAL,
    DEBUG
};

class LogTime{
    public:
        static int GetTimeStamp(){
            //第一个参数是一个结构体， struct timeval (秒、毫秒)
            //第二个参数是一个时区 - STC
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec;
        }

        static void GetTimeStamp(std::string& timestamp){
            //调用这个函数就可以获得时间戳
            //返回 年月日 时分秒

            time_t SysTime;     //time这个函数可以获取秒级的单位
            time(&SysTime);

            //这个localtime函数可以将time函数出餐的秒数转化为年月日-时分秒等
            struct tm* ST = localtime(&SysTime);
            //格式化字符串 [yyyy-mm-ddd HH-mm-ss];
            char TimeNow[23] = {0};
            snprintf(TimeNow, sizeof(TimeNow) - 1, "%04d-%2d-%2d %02d:%02d:%02d", ST->tm_year + 1900,
                    ST->tm_mon + 1, ST->tm_mday, ST->tm_hour, ST->tm_min, ST->tm_sec);

            timestamp.assign(TimeNow, strlen(TimeNow));
        }
};

//日记服务 - 写成inline - 为了调用该代码在原处展开，并且合适的调用__FILE__ & __LINE__
inline std::ostream& Log(LogLevel lev, const char* file, int line, const char* logmsg){

    std::string level_info = Level[lev];
    std::string timer_stamp;

    LogTime::GetTimeStamp(timer_stamp);

    //日记信息格式： 
    //[时间 info/warning/error/fatal/debug 文件 行号] 具体的错误信息
    std::cout << "[" << timer_stamp << " " << level_info << " " <<  file << ":" << line << "]"
        << logmsg << std::endl;

    return std::cout;
}

#define Log(lev, logmsg) Log(lev, __FILE__, __LINE__, logmsg)
