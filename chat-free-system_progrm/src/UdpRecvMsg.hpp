/*
 *  Josn序列胡反序列化使用 来 进行Udp数据交互传输。
 */

#include<iostream>
#include<string>
#include<jsoncpp/json/json.h>
#include<stdint.h>

#include"LogSer.hpp"

//json封装的数据
class UdpRecvMsg{
    public:
        void Deserialize(std::string& Message){

            //使用json反序列化解析数据 - 将数据保存到val中。
            Json::Reader reader;
            Json::Value val;
            reader.parse(Message, val, false);

            //使用json提供的接口获取数据

            NickName_ = val["NickName_"].asString();
            School_ = val["School_"].asString();
            Msg_ = val["Msg_"].asString();
            UserId_ = val["UserId_"].asUInt64();

        }                                                                                                                                                                                                                                                                                                                                                                               

        uint32_t GetUserId(){
            return UserId_;
        }

        const std::string&  GetNickName(){
            return NickName_;
        }

        const std::string&  GetSchool(){
            return School_;
        }

        const std::string&  GetMsg(){
            return Msg_;
        }

    private:
        std::string NickName_;
        std::string School_;
        std::string Msg_;
        uint32_t UserId_;
};
