#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "Command/Command.cpp"
#include "Server/Server.cpp"
#include "Command/CommandProcessor.cpp"
#include <thread>
#include <signal.h>

using namespace std;

Server* server = 0;
pthread_t *thread1 = 0;

void SignalHandler(int signal);
void Callback(Command * command);
void ExecChromeDriver();
void StopChromeDriver();

int main(int argc, char **argv)
{
    ExecChromeDriver();
    signal(SIGINT, SignalHandler);
    try
    {
        server = new Server;
        int status = server->Listen(&Callback);
        delete server;
        return status;
    }
    catch (int x)
    {
        perror("Exiting by unrecoverable error");
        return x;
    }
}

void Callback(Command * command)
{
    std::cout << "Processing command Request" <<  endl;
    CommandProcessor processor;
    processor.Process(command);
}

void SignalHandler(int signal)
{   
    (*server).Stop();
    cout<<"\nChao!\n";
    StopChromeDriver();
}

void StopChromeDriver()
{
    if(pthread_join(*thread1, NULL)) {
        perror("Error joining chromedriver thread\n");
        printf("Consider take a look at ps command\n");
    }
    delete thread1;
}

inline void* CallCD(void *causeitwants){
    system("chromedriver");
}

void ExecChromeDriver()
{
    cout << "===== chromedriver section ====="<<endl;
    thread1 = new pthread_t();
    if(pthread_create(thread1, NULL, &CallCD, NULL)) {
        perror("Error creating thread for chromedriver\n");
        exit(240);
    }
    sleep(1);
}