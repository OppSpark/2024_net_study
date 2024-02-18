#include "lib.h"
#include <thread>

void sendt(int clisock) {
    while (true) {
        char buf[1024];
        cout << "내용을 입력하세요: ";
        cout << "====================================" << endl;

        ssize_t sendlen = send(clisock, buf, strlen(buf) + 1, 0);
        if (sendlen == SOCKET_ERROR) {
            cout << "send() error" << endl;
            break;
        }
    }
}

void recvt(int clisock) {
    while (true) {
        char buf[1024];
        ssize_t recvlen = recv(clisock, buf, sizeof(buf), 0);
        if (recvlen == SOCKET_ERROR) {
            cout << "recv() error" << endl;
            break;
        }
        buf[recvlen] = '\0';
        cout << "답장: " << buf << endl;
        cout << "====================================" << endl;
    }
}

int main() {
    int clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    char ip[16];
    cout << "IP를 입력하세요: ";
    cin >> ip;
    cout << "====================================" << endl;
    cin.ignore(); // 버퍼를 비워줌

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(12345);

    if (connect(clisock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "connect() error" << endl;
        return 0;
    }

    std::thread send(sendt, clisock);
    std::thread recv(recvt, clisock);

    send.join();
    recv.join();

    close(clisock);
    return 0;
}
