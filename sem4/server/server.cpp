#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define BACKLOG 16
#define BUFLEN 1024

class Server {
private:
    int Server_fd;
    int port;
    char request[BUFLEN];
public:
    Server(int portnum);
    void Run();
    ~Server();
};

Server::Server(int portnum) {
/*
to create server we need:
    1. open socket - take FD, fd = socket(family, type, protocol)
    2. fill structure: sockaddr_in, which used by bind
    3. bind socket - i.e. give the server an adress, bind(socket FD, struct sockaddr_in, sizeof struct)
    4. put into listening mode
*/
//TODO: Exceptions for socket, bind, listen. Or, at least, appropriate error handling

    port = portnum;

    if ((Server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Cant create socket" << endl;
        exit(1);
    }
    struct sockaddr_in ServAddr;
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_port = htons(port);
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(Server_fd, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0) {
        cerr << "cant bind socket" << endl;
        close(Server_fd);
        exit(1);
    }

    if (listen(Server_fd, BACKLOG) < 0) {
        cerr << "cant listen" << endl;
        exit(1);
    }
}

Server::~Server() {
    close(Server_fd);
}

char* Send(const char* file, string header) {
    int fd = open(file, O_RDONLY);
    string str = header;
    str += "\nAllow: GET\nServer: MyServer/0.1\nConnection: keep-alive\nContet-length: ";

    char c;
    int len = 0;
    while(read(fd, &c, 1)) len++;
    lseek(fd, 0, 0);

    str += to_string(len);
    str += "\n\n";

    int n = str.length();
    char* res = (char*)malloc(sizeof(char) * (n+1));
    strcpy(res, str.c_str());
    return res;
}

void Server::Run() {
    while (1) {
        struct sockaddr_in ClientAddr;
        size_t ClAddrLen = sizeof(ClientAddr);
        int Client_fd = accept(Server_fd, (struct sockaddr*)&ClientAddr, (socklen_t*)&ClAddrLen);
        if (Client_fd < 0) {
            cerr << "Client error" << endl;
            close(Server_fd);
            exit(1);
        }
        int req = recv(Client_fd, request, BUFLEN, 0);
        if (req < 0) {
            cerr << "Server error" << endl;
            close(Client_fd);
            close(Server_fd);
            exit(1);
        }
//        if(strncmp(request, "GET", 3)) {
        {
            char* buf = Send("src/501.html", "HTTP/1.0 501 NotImplemented");
            send(Client_fd, buf, strlen(buf), 0);
            free(buf);
            cerr << "Error: BadRequest" << endl;
        }
        shutdown(Client_fd, SHUT_RDWR);
        close(Client_fd);
    }
}

int main(int argc, char**argv) {
    int portnum = 8080;
    if (argc == 2)
        portnum = atoi(argv[1]);

    Server server(portnum);
    server.Run();
    return 0;
}
