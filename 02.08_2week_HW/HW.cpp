#include "lib.h"
#include <filesystem>
#include <fstream>


std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {

    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    int flags = fcntl(servsock, F_GETFL, 0);
    fcntl(servsock, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    if (bind(servsock, (sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        cout << "bind() error" << endl;
        return 0;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 0;
    }

    while (true) {
        sockaddr_in cliaddr;
        uint addrlen = sizeof(cliaddr);
        int clisock = accept(servsock, (sockaddr*)&cliaddr, &addrlen);
        if (clisock == INVALID_SOCKET) {

            if (errno == EWOULDBLOCK) {
                continue;
            }
            else {
                cout << "accept() error: " << endl;
                return 0;
            }
        }

        cout << "Client Connected" << endl;

        char buf[1024] = "";

        int recvlen;

        while (true) {
            recvlen = recv(clisock, buf, sizeof(buf), 0);
            if (recvlen == SOCKET_ERROR) {
                if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
                    continue;
                }
                else {
                    cout << "recv() error" << endl;
                    return 0;
                }
            }
            else {
                break;
            }
        }

        if (recvlen == 0) {
            close(clisock);
            cout << "Client Disconnected" << endl;
            break;
        }

        buf[recvlen] = '\0';

        cout << "Recv: " << buf << endl;

        //메인 페이지
        if(strstr(buf, "GET / HTTP/1.1") != NULL){
            std::string indexContent = readFile("./html/index.html");
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            response += indexContent;
            strcpy(buf, response.c_str());
        }

        //page.html 파일 읽고 출력
        if (strstr(buf, "GET /page HTTP/1.1") != NULL) {
            std::string indexContent = readFile("./html/page.html");
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            response += indexContent;
            strcpy(buf, response.c_str());
        }

        //에러 페이지
        if (strstr(buf, "GET /error HTTP/1.1") != NULL) {
            std::string indexContent = readFile("./json/api.json");
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n";
            response += indexContent;
            strcpy(buf, response.c_str());
        }

        //사원 전체 조회
        if (strstr(buf, "GET /member HTTP/1.1") != NULL) {  
            if (strstr(buf, "Cookie: id=admin11") != NULL) {
                std::string indexContent = readFile("./json/member.json");
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n";
                response += indexContent;
                strcpy(buf, response.c_str());
            }else{
                std::string indexContent = readFile("./json/emtpyCookie.json");
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n";
                response += indexContent;
                strcpy(buf, response.c_str());
            }
        }

        //로그인
        if (strstr(buf, "GET /cookie HTTP/1.1") != NULL) {  
            strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nSet-Cookie: id=admin11\r\n\r\n<p>Login Success</p>");
        }

        //로그아웃
        if (strstr(buf, "GET /logout HTTP/1.1") != NULL) {  
            strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nSet-Cookie: id=\r\n\r\n<p>Logout Success</p>");
        }

        //포스트
        if (strstr(buf, "POST /post HTTP/1.1") != NULL) {  
            //요청 받은 값을 output.txt에 저장
            string line;
            ofstream file("output.txt");
            file << buf;
            file.close();
            strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n{\"result\":\"success\"}");
        }

        while(true){
            int sendlen;
            sendlen = send(clisock, buf, strlen(buf) + 1, 0);
            if (sendlen == SOCKET_ERROR) {
                if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
                    continue;
                }else {
                    cout << "send() error" << endl;
                    return 0;
                }
            }else {
                break;
            }
        }

        close(clisock);
        cout << "Client Disconnected" << endl;
        continue;
    }

    close(servsock);
    return 0;
}
