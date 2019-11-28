//
// Created by bassam on 02/11/2019.
//

#include <fstream>
#include <iostream>
#include "RequestProcessor.h"
#include "StringUtils.h"

vector<string> RequestProcessor::getRequests() {
    vector<string> rawRequests = getRawRequests();
    vector<string> formattedRequests;

    formattedRequests.reserve(rawRequests.size());
    for (const auto &rawRequest:rawRequests) {
        formattedRequests.emplace_back(formatRequest(rawRequest));
    }

    return formattedRequests;
}

vector<string> RequestProcessor::getRawRequests() {
    vector<string> requestsRaw;

    ifstream inFile("requests.txt", ios::binary);

    string line;
    while (getline(inFile, line)) {
        requestsRaw.emplace_back(line);
    }

    inFile.close();
    return requestsRaw;
}

string RequestProcessor::formatRequest(const string &rawRequest) {
    vector<string> splitRawRequest = StringUtils::split(rawRequest, " ");

    string formattedRequest;

    formattedRequest.append(splitRawRequest[1]);
    formattedRequest.append(" HTTP/1.1\r\n");

    if (splitRawRequest[0] == "client-get") {
        formattedRequest.insert(0, "GET ");
    } else if (splitRawRequest[0] == "client-post") {
        formattedRequest.insert(0, "POST ");
        formattedRequest.append(getFile(splitRawRequest[1]));
    } else {
        cout << "Error parsing request type. " << splitRawRequest[0] << endl;
    }

    formattedRequest.append("\r\n");

    return formattedRequest;
}

string RequestProcessor::getFile(const string &path) {
    string fileContents;

    ifstream inFile(path, ios::binary);
    string line;
    while (inFile) {
        getline(inFile, line);
        fileContents.append(line);
    }

    inFile.close();

    return fileContents;
}
