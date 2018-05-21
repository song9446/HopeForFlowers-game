//#include <game_state/game_state.hpp>
#include <iostream>
#include <uWS/uWS.h>
#include <thread>
#include <server_config.hpp>
#include <string>
//#include "leveldb/db.h"
#include "db_wrapper.hpp"
#include "packet.hpp"
#include "tool.hpp"


class UserManager {
public:
    enum class RESULT: int {
        SUCCESS=0, ID_ALREADY_EXIST, WRONG_ID_OR_SECRET,
        NONEXIST_ID, 
    };
    class Userdata {
        public:
        //char mail[128];
        int worm_id;
    };
private:
    LMDBWrapper db;
public:
    UserManager(const char* path): db(path){ 

    }
    ~UserManager(){
    }
    RESULT join(MDB_val id, MDB_val secret, Userdata userdata){
        std::string key = std::string(reinterpret_cast<char*>(id.mv_data), id.mv_size) + std::string(reinterpret_cast<char*>(secret.mv_data), secret.mv_size);
        char hashed_key[65];
        tool::sha256(key.c_str(), key.size(), hashed_key);
        printf("%s, %s, %s\n", std::string(reinterpret_cast<char*>(id.mv_data), id.mv_size), std::string(reinterpret_cast<char*>(secret.mv_data), secret.mv_size), hashed_key);
        if(!db.exist(MDB_val{64, hashed_key})){
            db.put(MDB_val{64, hashed_key}, MDB_val{sizeof(userdata), &userdata});
            return RESULT::SUCCESS;
        }else 
            return RESULT::ID_ALREADY_EXIST;
    }
    RESULT login(MDB_val id, MDB_val secret, Userdata* ud){
        printf("0\n");
        std::string key = std::string(reinterpret_cast<char*>(id.mv_data), id.mv_size) + std::string(reinterpret_cast<char*>(secret.mv_data), secret.mv_size);
        char hashed_key[65] = {'\0',};
        printf("1\n");
        tool::sha256(key.c_str(), key.size(), hashed_key);
        printf("2\n");
        printf("%s, %s, %s\n", std::string(reinterpret_cast<char*>(id.mv_data), id.mv_size), std::string(reinterpret_cast<char*>(secret.mv_data), secret.mv_size), hashed_key);
        if(!db.get(MDB_val{64, hashed_key}, ud))
            return RESULT::WRONG_ID_OR_SECRET;
        else
            return RESULT::SUCCESS;
    }
};


int main() {
    uWS::Group<uWS::SERVER> *login_group = NULL;
    //logout_group->addAsync();
    //login_group->addAsync();
    std::thread login_thread([&login_group]{
        uWS::Hub h;
        login_group = &h.getDefaultGroup<uWS::SERVER>();
        login_group->onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
            Packet req(message, length);
            switch(req.getOP()){
            case Packet::OP::ECHO:
                ws->send(req.bytes(), req.size(), opCode);
                break;
            }
        });
        h.getDefaultGroup<uWS::SERVER>().listen(uWS::TRANSFERS);
        h.run();
    });

    UserManager user_manager("test");
    uWS::Hub h;
    uWS::Group<uWS::SERVER> *logout_group = h.createGroup<uWS::SERVER>();
    logout_group->onMessage([&login_group, &user_manager](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
        uint8_t one = 1, zero = 0;
        Packet req(message, length);
        std::cout << (int)req.getOP() << std::endl;
        switch(req.getOP()){
        case Packet::OP::ECHO:
            ws->send(req.bytes(), req.size(), opCode);
            break;
        case Packet::OP::JOIN:
            char id[32], pw[32]; 
            tool::randStr(id, 32); tool::randStr(pw, 32);
            if(user_manager.join(MDB_val{32, id}, MDB_val{32, pw}, UserManager::Userdata{1}) == UserManager::RESULT::SUCCESS){
                Packet res(Packet::OP::JOIN, size_t(32), id, size_t(32), pw);
                ws->send(res.bytes(), res.size(), opCode);
            }
            else{
                Packet res(Packet::OP::JOIN, uint8_t(0));
                ws->send(res.bytes(), res.size(), opCode);
            }
            break;
        case Packet::OP::LOGIN:
            UserManager::Userdata userdata;
            MDB_val d = req.nextMDBVal(), w = req.nextMDBVal();
            if(user_manager.login(d, w, &userdata) == UserManager::RESULT::SUCCESS){
                Packet res(Packet::OP::LOGIN, uint8_t(1), userdata);
                ws->send(res.bytes(), res.size(), opCode);
                ws->transfer(login_group);
            }
            else{
                Packet res(Packet::OP::LOGIN, uint8_t(0));
                ws->send(res.bytes(), res.size(), opCode);
            }
            break;
        }
    });
    if (h.listen(server_config::SERVER_PORT, nullptr, uS::REUSE_PORT, logout_group)) {
        std::cout << "server is running on " << server_config::SERVER_PORT << std::endl;

    } else {
        std::cout << "FAILURE: Cannot listen to same port twice!" << std::endl;
        exit(-1);
    }
    h.run();
    login_thread.join();
    std::cout << "server end!" << std::endl; 
}
