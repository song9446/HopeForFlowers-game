#include <functional>
#ifdef __EMSCRIPTEN__
#include <SDL_net.h>
#include <vector>

class WebsocketWrapper {
private:
    typedef const std::function<void (char*, int)> Collable;
    TCPsocket socket;
    std::vector<char> buf;
public:
    enum class CODE: int {
        SUCCESS=0, FAIL_OPEN_PORT=1, FAIL_CONNECT=2, 
    };
    WebsocketWrapper();
    CODE connect(const char* addr, int port);
    int send(void* msg, int size);
    int recv(Collable& collable);
};

#else
#include "easywsclient.hpp"
class WebsocketWrapper {
private:
    typedef const std::function<void (char*, int)> Collable;
    easywsclient::WebSocket::pointer ws;
public:
    enum class CODE: int {
        SUCCESS=0, FAIL_OPEN_PORT=1, FAIL_CONNECT=2, 
    };
    WebsocketWrapper();
    CODE connect(const char* addr, int port);
    int send(void* msg, int size);
    int recv(Collable& collable);
};
#endif
