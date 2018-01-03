#include <string.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

class HttpRequest {
private:
    string requestBody;
    string headersSection;

public:

    HttpRequest(const string &request)
    {
        string buf;
        stringstream ss(request);

        while (std::getline(ss, buf, '\n'));

        this->requestBody = buf;

        if(this->requestBody.empty())
        {
            perror("Could not find a request body");
            throw 150;
        }
    }

    string GetRequestBody() const
    {
        return requestBody;
    }

};