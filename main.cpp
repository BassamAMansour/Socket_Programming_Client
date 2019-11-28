#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include "RequestProcessor.h"

#define RECV_BUFFER_SIZE 1000000

using namespace std;

void setupAddressInfo(char **argv, addrinfo **serverInfo);

int connectToServer(addrinfo **serverInfo);

int sendAllInBuffer(int socketFd, const string &bufferString);

void sendRequests(int socketFd);

void waitForResponse(int socketFd);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: client hostname and port\n");
        exit(1);
    }

    struct addrinfo *serverInfo;

    setupAddressInfo(argv, &serverInfo);

    int socketFd = connectToServer(&serverInfo);

    sendRequests(socketFd);

    close(socketFd);

    return 0;
}

void sendRequests(int socketFd) {
    vector<string> requests = RequestProcessor::getRequests();

    for (const auto &request:requests) {
        sendAllInBuffer(socketFd, request);
        waitForResponse(socketFd);
    }
}

void waitForResponse(int socketFd) {
    int receivedBytes;
    char responseBuffer[RECV_BUFFER_SIZE];

    if ((receivedBytes = recv(socketFd, responseBuffer, sizeof responseBuffer, 0)) <= 0) {
        // got error or connection closed by server
        if (receivedBytes == 0) {
            // connection closed
            printf("server on socket %d hung up\n", socketFd);
        } else {
            perror("recv");
        }

        close(socketFd); // bye!
    } else {
        cout << string(responseBuffer) << endl;
    }
}

int sendAllInBuffer(int socketFd, const string &bufferString) {
    int bufferLength = bufferString.size();
    char buffer[bufferLength + 1];
    bufferString.copy(buffer, bufferLength + 1);

    int total = 0;
    int bytesLeft = bufferLength;
    int sentBytes = -1;

    while (total < bufferLength) {
        sentBytes = send(socketFd, buffer + total, bytesLeft, 0);
        if (sentBytes == -1) { break; }
        total += sentBytes;
        bytesLeft -= sentBytes;
    }

    bufferLength = total; // return number actually sent here

    if (sentBytes == -1) {
        perror("sendall");
        printf("We only sent %d bytes because of the error!\n", bufferLength);
    }

    return sentBytes == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int connectToServer(addrinfo **serverInfo) {
    int socketFd = -1;
    struct addrinfo *addressIterator;

    for (addressIterator = *serverInfo; addressIterator != nullptr; addressIterator = addressIterator->ai_next) {

        if ((socketFd = socket(addressIterator->ai_family, addressIterator->ai_socktype,
                               addressIterator->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        int yes = 1;
        if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (connect(socketFd, addressIterator->ai_addr, addressIterator->ai_addrlen) == -1) {
            perror("client: connect");
            close(socketFd);
            continue;
        }

        break;
    }

    if (addressIterator == nullptr || socketFd == -1) {
        fprintf(stderr, "client: failed to connect\n");
        exit(2);
    }

    char s[INET6_ADDRSTRLEN];
    inet_ntop(addressIterator->ai_family, get_in_addr((struct sockaddr *) addressIterator->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s on socket %d\n", s, socketFd);

    freeaddrinfo(*serverInfo);

    return socketFd;
}

void setupAddressInfo(char **argv, addrinfo **serverInfo) {
    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(argv[1], argv[2], &hints, serverInfo);
    if ((status) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
}
