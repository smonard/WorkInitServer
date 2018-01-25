#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include "HttpRequestBuilder.cpp"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

class ChromeDriverClient{

private:
  BIO * biocd = 0;

  string host;
  int hostPort;
  
  HttpRequestBuilder * httpRequestBuilder;

  BIO* OpenSocket()
  {
    string uri = string(this->host + ":" + std::to_string(this->hostPort));
    biocd = BIO_new_connect(uri.c_str());
    if(BIO_do_connect(biocd) <= 0)
    {
        fprintf(stderr, "Error attempting to connect\n");
        ERR_print_errors_fp(stderr);
        throw 212;
    }
    return biocd;
  }

  string MakeRequest(string request) 
  {
    int bytesNumber;
    char response[512];

    BIO_write(biocd, request.c_str(), request.size());

    bytesNumber = BIO_read(biocd,response, 512);
    BIO_reset(biocd);
    return string(response);
  }

  string FindSessionId(string response)
  {
    size_t pos = response.find("\r\n\r\n");

    char session[33];

    size_t indexOfsession= pos + 18;
    
    for (char x = 0 ; x < 33; x++  ){
      session[x] = response.at(indexOfsession);
      indexOfsession++;
    }
    session[32] = '\0';
    return string(session);
  }

  string FindElementId(string response)
  {
    size_t pos = response.find("ELEMENT");

    size_t indexOfsession= pos + 10;
    size_t pos2 = response.find("}")  - 1;
    
    size_t s = pos2 - indexOfsession;

    char * element = new char[s + 1];

    for (int i=0; i < s; i++){
      element[i] = response.at(indexOfsession + i);
    }
    element[s] = '\0';
    
    string s2  = string(element);
    delete [] element;
    return s2;
  }

  unsigned char FindStatusCode(string driverResponse)
  {
    return 0;
  }
  
public:

  ChromeDriverClient(string n_host, int n_port) : host(n_host) 
  {
    this->hostPort = n_port;
    httpRequestBuilder = new HttpRequestBuilder(n_host, n_port);
  }

  string StartWebBrowserSession()
  {
      OpenSocket();
      string response = MakeRequest( httpRequestBuilder->PostForCreateSession());
      return FindSessionId(response);
  }

  unsigned char OpenUrl(string session, string url)
  {
     string driverResponse = MakeRequest( httpRequestBuilder->PostForOpenUrl(session, url));
     return FindStatusCode(driverResponse);
  }

  unsigned char PutTextAtElement(string session, string elementId, string user)
  {
     string driverResponseFE = MakeRequest( httpRequestBuilder->PostForFindById(session, elementId));
     unsigned char status1 = FindStatusCode(driverResponseFE);
     string rq = httpRequestBuilder->PostForSetText(session, FindElementId(driverResponseFE), user);
     string driverResponseSV = MakeRequest(rq);
     return (status1 + FindStatusCode(driverResponseSV));
  }

  unsigned char PutTextAtElementByName(string session, string elementName, string user)
  {
     string driverResponseFE = MakeRequest( httpRequestBuilder->PostForFindByName(session, elementName));
     unsigned char status1 = FindStatusCode(driverResponseFE);
     string rq = httpRequestBuilder->PostForSetText(session, FindElementId(driverResponseFE), user);
     string driverResponseSV = MakeRequest(rq);
     return (status1 + FindStatusCode(driverResponseSV));
  }

  unsigned char CreateTab(string session, string url)
  {
     string tabName = "tab" + to_string(rand() % 100);
     string driverResponseFE = MakeRequest( httpRequestBuilder->PostForCreateTab(session, url, tabName));
     unsigned char status1 = FindStatusCode(driverResponseFE);
     string driverResponseSV = MakeRequest( httpRequestBuilder->PostForSwitchTab(session, tabName));
     return (status1 + FindStatusCode(driverResponseSV));
  }

  unsigned char SubmitElement(string session, string elementId)
  {
     string driverResponseFE = MakeRequest( httpRequestBuilder->PostForFindById(session, elementId));
     unsigned char status1 = FindStatusCode(driverResponseFE);
     string driverResponseSV = MakeRequest( httpRequestBuilder->PostForSubmit(session, FindElementId(driverResponseFE)));
     return (status1 + FindStatusCode(driverResponseSV));
  }

  ~ChromeDriverClient()
  {
    if(biocd) BIO_free_all(biocd);
    delete httpRequestBuilder;
  }

};