#pragma once

#include <iostream>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <vector>
#include <atomic>
#include <thread>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

using namespace std;

#define OUT

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1