#include "lib.h"
// 실시간 채팅 서버 구현
// 소켓 통신으로 접속한 사용자들끼리 실시간 채팅을 할 수 있는 서버를 구현한다.

// 1. 서버는 사용자의 접속을 받아들이고, 사용자가 입력한 메시지를 다른 사용자에게 전달한다.
// 2. 사용자는 서버에 접속하여 메시지를 입력하고, 서버로부터 메시지를 받는다.
// 3. 서버는 사용자가 입력한 메시지를 다른 사용자에게 전달한다.
// 4. 사용자는 서버로부터 메시지를 받는다.

#define DEFAULT_BUFLEN 1024 // 버퍼의 기본 길이 정의
#define MAX_EVENTS 100 // epoll의 한 번에 처리할 이벤트 수


struct Session {
    int sock = INVALID_SOCKET; // 클라이언트 소켓
    char buf[DEFAULT_BUFLEN]; // 데이터를 담을 버퍼
    int recvbytes = 0; // 받은 바이트 수
    int sendbytes = 0; // 보낸 바이트 수
};


int main() {
    int servsock = socket(AF_INET, SOCK_STREAM, 0); // TCP 소켓 생성
    if (servsock == INVALID_SOCKET) { // 소켓 생성 실패 시 오류 처리
        cout << "socket() error" << endl;
        return 1;
    }

    int flags = fcntl(servsock, F_GETFL, 0);
    fcntl(servsock, F_SETFL, flags | O_NONBLOCK); // 소켓을 비동기(non-blocking) 모드로 설정

    sockaddr_in servaddr; // 서버 주소 정보
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    if (bind(servsock, (sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) { // 주소 바인딩
        cout << "bind() error" << endl;
        return 1;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) { // 연결 대기 상태로 변경
        cout << "listen() error" << endl;
        return 1;
    }

    vector<Session*> sessions; // 클라이언트 세션을 저장할 벡터
    sessions.reserve(100); // 벡터의 사이즈 미리 예약

    // 포인터를 사용한 이유는 epoll_wait() 함수에서 효율적으로 처리하기 위함
    sessions.push_back(new Session{servsock});

    int epollfd = epoll_create1(0); // epoll 인스턴스 생성
    if (epollfd == SOCKET_ERROR) { // epoll 인스턴스 생성 실패 시 오류 처리
        cout << "epoll_create1() error" << endl;
        return 1;
    }

    epoll_event epEvents[MAX_EVENTS], epEvent; // epoll 이벤트 리스트와 이벤트 구조체

    // 서버 소켓은 EPOLLIN 이벤트를 감지하도록 설정
    epEvent.events = EPOLLIN;
    epEvent.data.fd = servsock;
    // 서버 소켓 정보가 들어간 epoll_event 구조체를 epollfd에 추가
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock, &epEvent) == SOCKET_ERROR) {
        cout << "epoll_ctl() error" << endl;
        return 1;
    }

    while (true) {
        // epoll_wait() 함수를 사용하여 모든 이벤트를 감지하고 변경된 소켓만 남김
        int epfds = epoll_wait(epollfd, epEvents, MAX_EVENTS, INFINITE);
        if (epfds == SOCKET_ERROR) { // epoll_wait() 오류 처리
            cout << "epoll_wait() error" << endl;
            break;
        }

        // 살아남은 소켓 = 상태가 변경된 소켓이므로 이벤트들을 처리
        for (int i = 0; i < epfds; i++) {
            // 상태가 변경된 소켓이 서버 소켓이면 무조건 EPOLLIN이므로 accept() 호출
            if (epEvents[i].data.fd == servsock) { // 연결 요청 확인
                sockaddr_in cliaddr;
                socklen_t addrlen = sizeof(cliaddr);

                int clisock = accept(servsock, (sockaddr*)&cliaddr, &addrlen); // 클라이언트 연결 수락
                if (clisock == INVALID_SOCKET) { // 수락 실패 시 오류 처리
                    cout << "accept() error" << endl;
                    break;
                }

                int flags = fcntl(clisock, F_GETFL, 0);
                fcntl(clisock, F_SETFL, flags | O_NONBLOCK);

                // 포인터를 사용한 이유는 epoll_wait() 함수에서 효율적으로 처리하기 위함
                Session* newSession = new Session{clisock};

                // 클라이언트 소켓은 EPOLLIN과 EPOLLOUT 이벤트를 감지하도록 설정
                epEvent.events = EPOLLIN | EPOLLOUT;
                epEvent.data.ptr = newSession;
                // 클라이언트 소켓 정보가 들어간 epoll_event 구조체를 epollfd에 추가
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clisock, &epEvent) == SOCKET_ERROR) {
                    cout << "epoll_ctl() error" << endl;
                    return 1;
                }

                sessions.push_back(newSession);
                
                cout << "Client Connected" << endl;
                cout << "클라이언트  번호: " << clisock << endl;
                cout << "세선 크기: " << sessions.size() << endl;

                continue;
            }

            // 포인터를 사용해서 불 필요한 이중 반복문 대신 이벤트 주소를 직접 참조
            Session* session = (Session*)epEvents[i].data.ptr;

            // 상태가 변경된 소켓이 클라이언트 소켓이면
            // EPOLLIN 또는 EPOLLOUT 이벤트를 감지하므로 recv() 또는 send() 호출
            if (epEvents[i].events & EPOLLIN) { // 데이터 수신 확인
                int recvbytes = recv(session->sock, session->buf, DEFAULT_BUFLEN, 0); // 데이터 수신
                if (recvbytes == SOCKET_ERROR) { // 수신 실패 시 오류 처리
                    if (errno == EWOULDBLOCK) continue; // 블록되는 경우 무시
                    cout << "recv() error" << endl;
                    break;
                }

                if (recvbytes == 0) { // 연결 종료 시 세션 제거
                    close(session->sock);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, session->sock, NULL);
                    sessions.erase(remove(sessions.begin(), sessions.end(), session), sessions.end());
                    delete session;
                    cout << "Client Disconnected" << endl;
                    continue;
                }
                cout << "Received: " << session->buf << endl;
                
                // 모든 클라이언트에게 메시지 전달
                for(int j = 0; j < sessions.size(); j++) {
                    if (sessions[j]->sock == servsock) continue;
                    send(sessions[j]->sock, session->buf, recvbytes, 0);
                
                }



                // for (auto& s : sessions) { // 모든 세션에게 메시지 전달
                //     //서버에게는 보내지 않음
                //     if (s->sock == servsock) continue;

                //     send(s->sock, session->buf, recvbytes, 0); // 데이터 전송
                // }

            }
        }
    }

    close(servsock);
    close(epollfd);
    return 0;
}
