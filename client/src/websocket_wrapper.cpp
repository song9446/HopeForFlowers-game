#include "websocket_wrapper.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

    const int INITIAL_BUF_SIZE = 1024;
    WebsocketWrapper::WebsocketWrapper(){ }
    WebsocketWrapper::CODE WebsocketWrapper::connect(const char* addr, int port){
        IPaddress ipAddress;
        if(-1 == SDLNet_ResolveHost(&ipAddress, addr, port)){
            printf("Failed to open port : %d\n", port);
            return CODE::FAIL_OPEN_PORT;
        }
        socket = SDLNet_TCP_Open(&ipAddress);
        if(socket == nullptr){
            printf("Failed to connect to port : %d\n", port);
            printf("Error: %s\n", SDLNet_GetError());
            return CODE::FAIL_CONNECT;
        }
        buf.resize(INITIAL_BUF_SIZE);
        return CODE::SUCCESS;
    }
    int WebsocketWrapper::send(void* msg, int size){
        return SDLNet_TCP_Send(socket, msg, size);
    }
    int WebsocketWrapper::recv(Collable& collable){
        int len = 0, t=0;
        while((t = SDLNet_TCP_Recv(socket, &buf[len], buf.size()-len))>0){
            if(t == buf.size()-len)
                buf.resize(2*buf.size());
            len += t;
        }
        if(len) collable(&buf[0], len);
        return len;
    }

#else
    WebsocketWrapper::WebsocketWrapper(){ }
    WebsocketWrapper::CODE WebsocketWrapper::connect(const char* addr, int port){
        std::string addr_str(addr);
        addr_str += ":" + std::to_string(port);
        addr_str = "ws://" + addr_str;
        ws = easywsclient::WebSocket::from_url(addr_str);
        if(ws != NULL)
            return CODE::SUCCESS;
        else return CODE::FAIL_CONNECT;
    }
    int WebsocketWrapper::send(void* msg, int size){
        ws->sendBinary(std::string(reinterpret_cast<char*>(msg), size));
        ws->poll();
        return size;
    }
    int WebsocketWrapper::recv(Collable& collable){
        ws->poll();
        int len = 0;
        ws->dispatchBinary([&collable, &len](std::vector<uint8_t> message){
            collable(reinterpret_cast<char*>(&message[0]), message.size());
            len = message.size();
        });
        return len;
    }
#endif
