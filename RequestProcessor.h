//
// Created by bassam on 02/11/2019.
//

#ifndef SOCKET_PROGRAMMING_CLIENT_REQUESTPROCESSOR_H
#define SOCKET_PROGRAMMING_CLIENT_REQUESTPROCESSOR_H

#include <vector>
#include <string>

using namespace std;

class RequestProcessor {

public:
    static vector <string> getRequests();

    static vector<string> getRawRequests();

    static string formatRequest(const string& rawRequest);

    static string getFile(const string& path);
};


#endif //SOCKET_PROGRAMMING_CLIENT_REQUESTPROCESSOR_H
