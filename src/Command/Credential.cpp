#ifndef Credential_cpp
#define Credential_cpp

#include <iostream>
#include <string>

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


using namespace std;

class Credential {

 private:

  string pass;
  string user;
  int fuxyNumber;
  char *encodeTable;

  int GetFuzyNumber(){
    return 1;
  }

 public:

  Credential(string user, string pass){
     this->user = user;
     this->pass = pass;
  }

  string GetKey() {
    return this->pass;
  }

  string GetUser(){
     return user;
  }

  ~Credential(){
  }
};

#endif
