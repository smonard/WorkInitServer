#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <signal.h>
#include "Server/HttpRequest.cpp"
#include "Command/Command.cpp"

using namespace std;

class Server{

private :

 SSL_CTX *ctx = 0;
 int sock = 0;
 bool listening = 0;


int CreateSocket(int port)
{
    int s = 0;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
	    perror("Unable to create socket");
	    throw 200;
    }

    if (::bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
	    perror("Unable to bind");
	    throw 201;
    }

    if (listen(s, 1) < 0) {
	    perror("Unable to listen");
	    throw 202;
    }

    return s;
}

SSL_CTX *CreateContext()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
      perror("Unable to create SSL context");
      ERR_print_errors_fp(stderr);
      throw 203;
    }

    return ctx;
}

void ConfigureContext(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);
    char* certr = (std::getenv("CERT_PR"));
    if (SSL_CTX_use_certificate_file(ctx, certr, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        throw 204;
    }
    char* keyr = (std::getenv("KEY_PR"));
    if (SSL_CTX_use_PrivateKey_file(ctx, keyr, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        throw 205;
    }
}

Command * GetCommandInstance(char * p)
{
    HttpRequest http((string(p)));
    string rqb = http.GetRequestBody();
    
    Command *command = Command::Parse(rqb);
    return command;
}

public:

  Server(){
    int port = 9443;
    cout << "===== Starting UBX ====="<<endl;
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ERR_load_BIO_strings();
    ctx = CreateContext();
    ConfigureContext(ctx);
    sock = CreateSocket(port);
    cout << "Server ready at "<< port<<endl;
  }

  int Listen(void (*Callback)(Command*))
  {
    listening = true;
    while(this->listening)
    {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;
        char response[] = "HTTP/1.1 200 OK\r\nConnection: close\r\nServer: UBX\r\n\r\n";

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) continue;
        std::cout  << "\nS>" << ">>> Incoming request\n" << endl;
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        if (SSL_accept(ssl) <= 0)
        {
            ERR_print_errors_fp(stderr);
        }
        else
        {
            char *buffer = new char[256];
            size_t read = SSL_read(ssl,buffer, 256);
            if(read)
            {
                std::cout << "S> " << read << " bytes read" << endl;
                char *p = (char*)realloc(buffer, read);
                Command * command = 0;
                try
                {   
                    command = GetCommandInstance(p);
                    delete [] (p); 
                    (*Callback)(command);
                    delete command;
                }
                catch(int x)
                { 
                    if(command) delete (command); 
                    if(p) delete [] (p); 
                }
            }
            if(SSL_write(ssl, response, strlen(response)) <= 0)
            {
                perror("Unable to send response");
            }
        }
        std::cout << "S> " << "Finalizing request processing" << endl;
        SSL_free(ssl);
        close(client);
    }
  }

  void Stop()
  {
    cout<<"\nTrying to stop Server gracefully";
    listening = false;
    close(sock);
  }

  ~Server()
  {
    if(sock) close(sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
  }

};