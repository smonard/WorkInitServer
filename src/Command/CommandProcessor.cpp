#include "../Client/ChromeDriverClient.cpp"
#include <unistd.h>


class CommandProcessor
{
private:

    ChromeDriverClient * webBrowserClient;
public:

    CommandProcessor()
    {
        webBrowserClient = new ChromeDriverClient("localhost", 9515);
    }

    ~CommandProcessor()
    {
        delete webBrowserClient;
    }

    unsigned char Process(Command * command)
    {
        return ProcessUpAllCommand(command->GetCredential());
    }

private:

    unsigned char ProcessUpAllCommand(Credential * credential)
    {
        string sessionId = webBrowserClient->StartWebBrowserSession();
        unsigned char status = webBrowserClient->OpenUrl(sessionId, std::getenv("MAIN_URL"));
        status &= OpenLogsSession(sessionId, credential);
        status &= OpenBoardSession(sessionId, credential);
        return status;
    }

    unsigned char OpenBoardSession(string sessionId, Credential* credential)
    {
        cout<<"Initializing board"<<endl;
        char* boardUrl = std::getenv("BOARD_URLS");

        string buf;
        stringstream ss(boardUrl);
        bool login = true;
        unsigned char status = 0;
        while (std::getline(ss, buf, ';')){
            status += OpenBoardTab(sessionId, credential, buf, login);
            login = false;
        }

        if(status)
        {
            perror("Something went wrong when trying to open a Board session");
            return 1;
        }
        cout<<"Done"<<endl;
        return status;
    }

    unsigned char OpenBoardTab(string sessionId, Credential* credential, string url, bool login)
    {
        unsigned char status = webBrowserClient->CreateTab(sessionId, url);
        if(login) {
            status += webBrowserClient->PutTextAtElement(sessionId, "login-form-username", (*credential).GetUser());
            status += webBrowserClient->PutTextAtElement(sessionId, "login-form-password", (*credential).GetKey());
            status += webBrowserClient->SubmitElement(sessionId, "login-form-submit");
            sleep(1);
        }
        return status;
    }

    unsigned char OpenLogsSession(string sessionId, Credential* credential)
    {
        cout<<"Initializing Logs"<<endl;
        char* logsUrl = std::getenv("LOGS_URLS");

        string buf;
        stringstream ss(logsUrl);
        bool login = true;
        unsigned char status = 0;
        while (std::getline(ss, buf, ';')){
            status += OpenLogTab(sessionId, credential, buf, login);
            login = false;
        }
        if(status)
        {
            perror("Something went wrong when trying to open a Logs session");
            return 1;
        }
        cout<<"Done"<<endl;

        return status;
    }

    unsigned char OpenLogTab(string sessionId, Credential* credential, string url, bool login)
    {
        unsigned char status = webBrowserClient->CreateTab(sessionId, url);
        if(login) {
            status += webBrowserClient->PutTextAtElement(sessionId, "username", (*credential).GetUser());
            status += webBrowserClient->PutTextAtElement(sessionId, "password", (*credential).GetKey() + "\n");
            sleep(1);
        }
        return status;
    }
};