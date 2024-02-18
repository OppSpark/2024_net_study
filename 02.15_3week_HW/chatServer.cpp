#include "lib.h"

#define DEFAULT_BUFLEN 1024
#define MAX_EVENTS 100

struct Session {
    int sock = INVALID_SOCKET;
    char buf[DEFAULT_BUFLEN];
    int recvbytes = 0;
    int sendbytes = 0;
};

int main() {
    int servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (servsock == INVALID_SOCKET) {
        cout << "socket() error" << endl;
        return 1;
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
        return 1;
    }

    if (listen(servsock, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() error" << endl;
        return 1;
    }

    vector<Session*> sessions;
    sessions.reserve(100);

    sessions.push_back(new Session{servsock});

    int epollfd = epoll_create1(0);
    if (epollfd == SOCKET_ERROR) { 
        cout << "epoll_create1() error" << endl;
        return 1;
    }

    epoll_event epEvents[MAX_EVENTS], epEvent; 
    epEvent.events = EPOLLIN;
    epEvent.data.fd = servsock;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock, &epEvent) == SOCKET_ERROR) {
        cout << "epoll_ctl() error" << endl;
        return 1;
    }

    while (true) {
        int epfds = epoll_wait(epollfd, epEvents, MAX_EVENTS, INFINITE);
        if (epfds == SOCKET_ERROR) { 
            cout << "epoll_wait() error" << endl;
            break;
        }

        for (int i = 0; i < epfds; i++) {
            if (epEvents[i].data.fd == servsock) {
                sockaddr_in cliaddr;
                socklen_t addrlen = sizeof(cliaddr);

                int clisock = accept(servsock, (sockaddr*)&cliaddr, &addrlen); 
                if (clisock == INVALID_SOCKET) { 
                    cout << "accept() error" << endl;
                    break;
                }

                int flags = fcntl(clisock, F_GETFL, 0);
                fcntl(clisock, F_SETFL, flags | O_NONBLOCK);

                Session* newSession = new Session{clisock};

                epEvent.events = EPOLLIN | EPOLLOUT;
                epEvent.data.ptr = newSession;

                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clisock, &epEvent) == SOCKET_ERROR) {
                    cout << "epoll_ctl() error" << endl;
                    return 1;
                }

                sessions.push_back(newSession);
                cout << "====================================" << endl;
                cout << "Client Connected" << endl;
                cout << "클라이언트 번호 : " << clisock << endl;
                cout << "현제 연결된 세션 크기 : " << sessions.size() << endl;

                continue;
            }

            Session* session = (Session*)epEvents[i].data.ptr;

            if (epEvents[i].events & EPOLLIN) {
                int recvbytes = recv(session->sock, session->buf, DEFAULT_BUFLEN, 0); // 데이터 수신
                if (recvbytes == SOCKET_ERROR) { 
                    if (errno == EWOULDBLOCK) continue; 
                    cout << "recv() error" << endl;
                    break;
                }

                if (recvbytes == 0) {
                    close(session->sock);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, session->sock, NULL);
                    sessions.erase(remove(sessions.begin(), sessions.end(), session), sessions.end());
                    delete session;
                    cout << "====================================" << endl;
                    cout << "Client Disconnected" << endl;
                    cout <<  session->sock << "번 클라이언트가 종료함" << endl;
                    cout << "현제 연결된 세션 크기 : " << sessions.size() << endl;
                    continue;
                }
                cout << "====================================" << endl;
                cout << "클라이언트 번호 : " << session->sock  << endl;
                cout << "내용 : " << session->buf << endl;

                for(int j = 0; j < sessions.size(); j++) {
                    if (sessions[j]->sock == servsock) continue;
                    send(sessions[j]->sock, session->buf, recvbytes, 0);
                
                }
            }
        }
    }

    close(servsock);
    close(epollfd);
    return 0;
}