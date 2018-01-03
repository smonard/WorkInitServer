#ifndef Command_cpp
#define Command_cpp

#include "Credential.cpp"
#include <sstream>

class Command
{
private:
    Credential * credentials;
    char sing;

public:
    static Command* Parse(string request){
        string user, pass;
        stringstream ss(request);
        std::getline(ss, user, '|');
        std::getline(ss, pass, '|');
        Credential * credential = new Credential(user, pass);
        return new Command(credential, 'U');
    }

    Command(Credential * credential, char nsing) : credentials(credential), sing(nsing)
    {}

    Credential * GetCredential() const
    {
        return this->credentials;
    }

    ~Command()
    {
        delete credentials;
    }
};


#endif