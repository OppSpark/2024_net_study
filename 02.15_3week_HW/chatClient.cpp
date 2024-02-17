#include "lib.h"

int main () {
    int clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    char ip[16];
    cout << "Input server IP: "; cin >> ip;

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        return 0;
    }

    while (true) {
        char buf[1024];
        cout << "Input: "; cin >> buf;


        int sendlen = send(clisock, buf, strlen(buf) + 1, 0);
        if (sendlen == SOCKET_ERROR) {
            cout << "send() error" << endl;
            return 0;
        }

        while(true){
            int recvlen = recv(clisock, buf, sizeof(buf), 0);
            if (recvlen == SOCKET_ERROR) {
                cout << "recv() error" << endl;
                return 0;
            }
            else if(recvlen == 0){
                break;
            }
            buf[recvlen] = '\0';
            cout << "Echo: " << buf << endl;
        }
        /*
        int recvlen = recv(clisock, buf, sizeof(buf), 0);
        if (recvlen == SOCKET_ERROR) {
            cout << "recv() error" << endl;
            return 0;
        }
        buf[recvlen] = '\0';

        cout << "Echo: " << buf << endl;
        */
    }

    // 소켓 종료
    close(clisock);
    return 0;
}