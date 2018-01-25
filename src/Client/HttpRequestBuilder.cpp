#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

class HttpRequestBuilder {

private:
    string host;
    int port;

  string BuildRequest(const char * request, const char* resource){
    string requestBody(request);
    string http = "POST " + string(resource) + string(" HTTP/1.1\r\n");
    string httpHost = "Host: " + this->host + "\r\n";
    string httpContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    string httpContentLeght = string("Content-Length: ") + to_string(requestBody.size()) + "\r\n";
    string httpConnectionClose = "Connection: Close\r\n";
    string httpAccept = "Accept: */*\r\n";
    string httpEndLine = "\r\n";

    stringstream requestStream;
    requestStream << http << httpHost << httpContentType << httpContentLeght << httpConnectionClose << httpAccept << httpEndLine << requestBody;

    return requestStream.str();
  }

public:

    HttpRequestBuilder(string _host, int _port) : host(_host), port(_port){
    }

    string PostForCreateSession(){
        const char* requestBody = "{\"desiredCapabilities\": {\"browserName\": \"firefox\",\"version\": \"\",\"platform\": \"ANY\",\"javascriptEnabled\": true,\"cssSelectorsEnabled\": true,\"takesScreenshot\": false,\"nativeEvents\": false,\"rotatable\": false},\"capabilities\": {\"firstMatch\": [{\"browserName\": \"firefox\"}]}}\r\n";
        const char* resource = "/session";
        return BuildRequest(requestBody, resource);
    }

    string PostForOpenUrl(string sessionId, string url){
        string ss1 = string("{\"url\":\"" + url + "\"}");
        const char * requestBody = ss1.c_str();
        string ss = string("/session/" + sessionId + "/url");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

    string PostForFindById(string sessionId,string id){
        string ss1 = string("{\"using\":\"id\",\"value\":\"" + id +  "\"}");
        const char * requestBody = ss1.c_str();
        string ss = string("/session/" + sessionId + "/element");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

    string PostForFindByCSS(string sessionId,string css){
        string ss1 = string("{\"using\":\"css\",\"value\":\"" + css +  "\"}");
        const char * requestBody = ss1.c_str();
        string ss = string("/session/" + sessionId + "/element");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

     string PostForFindByName(string sessionId,string name){
        string ss1 = string("{\"using\":\"name\",\"value\":\"" + name +  "\"}");
        const char * requestBody = ss1.c_str();
        string ss = string("/session/" + sessionId + "/element");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

    string PostForSetText(string sessionId, string elementId, string text){
        string ss1 = string("{\"value\":[\"" + text +  "\"]}");
        const char * requestBody = ss1.c_str();
        string ss2 = string("/session/" + sessionId + "/element/" + elementId + "/value");
        const char * resource = ss2.c_str();
        return BuildRequest(requestBody, resource);
    }

    string PostForSubmit(string sessionId, string elementId){
        const char * requestBody = "{}";
        string ss = string("/session/" + sessionId + "/element/" + elementId + "/submit");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

    string PostForCreateTab(string sessionId, string newUrl, string tabName){
        string ss1 = "{\"script\":\"window.open('" + newUrl +"', '" + tabName + "')\",\"args\":[]}";
        const char * requestBody = ss1.c_str();
        string ss = string("/session/" + sessionId + "/execute");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

    string PostForSwitchTab(string sessionId, string tabName){
        string ss1 = "{\"name\":\"" + tabName + "\"}";
        const char * requestBody = ss1.c_str();
        string ss = string("/session/" + sessionId + "/window");
        const char * resource = ss.c_str();
        return BuildRequest(requestBody, resource);
    }

    string GetForShutdonw(){
        const char * requestBody = "{}";
        const char * resource = "/shutdown";
        return BuildRequest(requestBody, resource);
    }

};