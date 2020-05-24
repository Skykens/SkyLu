//
// Created by jimlu on 2020/5/23.
//

#include "httprequest.h"
#include <algorithm>
#include <functional>
namespace skylu{
    HttpRequest::HttpRequest(Buffer * input)
        :m_input_buffer(input)
        ,m_version(UNKNOW)
        ,m_method(INVALID)
        ,m_isWorking(false)
        ,m_state(RequestLine){

    }
    bool HttpRequest::parseRequest() {
        bool ret = true;
        bool more = true;
        while(more){
            if(m_state == RequestLine){
                const char * crlf = m_input_buffer->findCRLF();
                if(crlf){
                    ret = parseRequestLine(m_input_buffer->curRead(),crlf);
                    if(ret){
                        m_input_buffer->updatePosUntil(crlf + 2);
                        m_state = Headers;
                    }else{
                        more = false;  //解析终止
                    }
                } else{
                    return false;
                }
            }else if(m_state == Headers){
                const char *crlf = m_input_buffer->findCRLF();
                if(crlf){
                    const char * fildname = std::find(m_input_buffer->curRead(),crlf,':');
                    //找到字段名
                    if(fildname != crlf){
                        addHeader(m_input_buffer->curRead(),fildname,crlf);
                    }else{
                        m_state = Finish;
                        more = false;
                    }
                    m_input_buffer->updatePosUntil(crlf+2);
                }else{
                    more = false;
                }
            }else if(m_state == Body){

                // 处理报文体还没写


            }
        }
        return ret;
    }

    bool HttpRequest::parseRequestLine(const char *begin, const char *end) {
        bool ret = false;
        const char *start = begin;
        const char *space = std::find(start,end,' ');
        if(space != end && setMethod(start,space)){
            start = space + 1;
            space = std::find(start,end,' ');
            if(space != end){
                const char * question = std::find(start,space ,'?');
                if(question != space){
                    setPath(start,question);
                    setArg(question,space);
                }else{
                    setPath(start,space);
                }

                start = space + 1;
                ret = end - start == 8 && std::equal(start,end - 1,"HTTP/1.");
                if(ret){
                    if(*(end - 1) == '1'){
                        setVersion(HTTP11);
                    } else if(*(end - 1) == '0'){
                        setVersion(HTTP10);

                    }else
                        ret = false;
                }
            }
        }
        return ret;
    }

    void HttpRequest::addHeader(const char *start, const char *namelast, const char *end) {
        std::string fild(start,namelast);
        ++namelast;  //跳过冒号
        while(namelast < end && *namelast ==' ') {
            namelast++;  //去掉前面的空格
        }
        std::string value(namelast,end);
        while(value.size()&& value[value.size()-1] == ' '){
            value.pop_back(); //去除后面的空格
        }
        m_header[fild] = value; //可优化为move
    }

    std::string HttpRequest::getMethod() const {
        std::string ret;
        switch(m_method){
#define XX(name) \
        case Method::name: \
            return #name;  \
            break;

            XX(GET);
            XX(POST);
            XX(HEAD);
            XX(PUT);
            XX(DELETE);

#undef XX
            default:
                return "UNKNOW";
        }
    }
    std::string HttpRequest::getHeader(const std::string &fild) const {
        std::string ret;
        auto it = m_header.find(fild);
        if(it != m_header.end()){
            ret = it->second;
        }
        return ret;
    }


    bool HttpRequest::isKeepAlive() const {
        std::string  fild = getHeader("Connection");
        return fild == "Keep-Alive" || (m_version == HTTP11&&fild != "close");
    }


    void HttpRequest::resetParse() {
        m_state = RequestLine;
        m_method = INVALID;
        m_version = UNKNOW;
        m_path="";
        m_arg="";
        m_header.clear();
    }

    void HttpRequest::setPath(const char *begin, const char *end) {
        std::string subpath;
        subpath.assign(begin,end);
        if(subpath == "/"){
            subpath = "/index.html";
        }
        m_path = WWW_ROOR + subpath;
    }

    bool HttpRequest::setMethod(const char *begin, const char *end) {
        std::string m(begin, end);
#define XX(name) \
        if(m == #name) \
            m_method = name;
        XX(GET);
        XX(POST);
        XX(HEAD);
        XX(PUT);
        XX(DELETE);

#undef XX

        return m_method !=INVALID;



    }
}

