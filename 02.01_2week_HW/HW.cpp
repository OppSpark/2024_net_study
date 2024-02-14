#include "lib.h"
#include <filesystem>
#include <fstream>


//파일 읽기
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    // Blocking Socket
    // accept (접속한 클라이언트가 있으면)
    // -> connect (클라이언트가 접속에 성공하면)
    // -> send/sendto (클라이언트/서버가 데이터를 송신버퍼에 복사하면)
    // -> recv/recvfrom (클라이언트/서버가 데이터를 수신버퍼에서 복사해옴)
    // Non-blocking Socket
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 0;
    }

    // 논블로킹 소켓으로 만들기
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
            // 블로킹 일때는 바로 문제가 되지만,
            // cout << "accept() error" << endl;
            // return 0;
            // 논블로킹 일때는 한번 더 확인해야 한다
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

        while (true) {
            int recvlen;
            // 논블로킹 소켓은 send()도 루프를 돌면서 될 때까지 계속 시도해야함
            while (true) {
                recvlen = recv(clisock, buf, sizeof(buf), 0);
                if (recvlen == SOCKET_ERROR) {
                    // 논블로킹 소켓은 recv()에서 한번 더 체크해줘야함
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

            if (recvlen == 0) { // 클라이언트가 접속을 끊었으면
                close(clisock); // 소켓을 닫고
                cout << "Client Disconnected" << endl;
                break; // 루프를 빠져나가서 다음 클라이언트를 받는다
            }

            buf[recvlen] = '\0';

            cout << "Recv: " << buf << endl;

            
            //메인 페이지
            if(strstr(buf, "GET / HTTP/1.1") != NULL){
                std::string indexContent = readFile("index.html");
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                response += indexContent;
                strcpy(buf, response.c_str());
            }

        
           //html 파일 읽어서 보내기
            if (strstr(buf, "GET /page HTTP/1.1") != NULL) {
                std::string indexContent = readFile("page.html");
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                response += indexContent;
                strcpy(buf, response.c_str());
            }

            
            //에러 페이지
            if (strstr(buf, "GET /error HTTP/1.1") != NULL) {
                std::string indexContent = readFile("api.json");
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n";
                response += indexContent;
                strcpy(buf, response.c_str());
            }

            
            //사원 전체 조회
            if (strstr(buf, "GET /member HTTP/1.1") != NULL) {  
                if (strstr(buf, "Cookie: id=admin11") != NULL) {
                    std::string indexContent = readFile("member.json");
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n";
                    response += indexContent;
                    strcpy(buf, response.c_str());
                }else{
                    std::string indexContent = readFile("emtpyCookie.json");
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
                string line;
                ofstream file("output.txt");
                file << buf;
                file.close();
                //요청이 오면 응답하기  
                strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n{\"result\":\"success\"}");
            }

            int sendlen;
            
            sendlen = send(clisock, buf, strlen(buf) + 1, 0);
            if (sendlen == SOCKET_ERROR) {
                // 논블로킹 소켓은 send()에서 한번 더 체크해줘야함
                if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
                    continue;
                }else {
                    cout << "send() error" << endl;
                    return 0;
                }
            }else {
                break;
            }

            if (sendlen == 0) { // 클라이언트가 접속을 끊었으면
                close(clisock); // 소켓을 닫고
                cout << "Client Disconnected" << endl;
                break; // 루프를 빠져나가서 다음 클라이언트를 받는다
            }
        }
    }

    // 소켓 종료
    close(servsock);
    return 0;
} // 논블로킹 + 동기 방식은 상당히 많은 루프를 필요로 하기 때문에 비효율적이다
