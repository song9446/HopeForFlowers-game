CPP_OPENSSL_OSX = -L/usr/local/opt/openssl/lib -L/usr/local/lib -I/usr/local/opt/openssl/include -I/usr/local/include

OUT_ROOT = build
OUT = $(OUT_ROOT)/server
#SOURCES = server.cpp game_state.hpp
SOURCES = src/server.cpp ../protocol/src/packet.cpp
#all: build/server

$(OUT): $(SOURCES) src/tool.cpp
	mkdir -p $(dir $(OUT))
	$(CXX) $(CPP_OPENSSL_OSX) -std=c++14 -O3 $(SOURCES) -Isrc -I../protocol/include -o $(OUT) -L. -Lsrc -lpthread -luWS -lssl -lcrypto -lz -luv -lleveldb -llmdb -lcrypto

clean:
	rm -rf $(OUT_ROOT)
