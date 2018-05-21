CPP_OPENSSL_WIN32 = -march=x86-64 -Wall -LC:/MinGW/lib -I/usr/local/opt/openssl/include -I/usr/local/include -I../server/src/game_state -Ithirparty/easywsclient -IC:/MinGW/x86_64-w64-mingw32/include/SDL2 -LC:/MinGW/x86_64-w64-mingw32/lib 

WEB_OUT_ROOT = $(OUT_ROOT)/web
LINUX_OUT_ROOT = $(OUT_ROOT)/linux
WIN32_OUT_ROOT = $(OUT_ROOT)/win32/
#SOURCES = server.cpp game_state.hpp

SDIRS = src thirdparty/Chipmunk2D/src ../gamelogic ../protocol/src
VPATH = $(subst $(eval) $(eval),:,$(SDIRS))
BDIR = bin
ODIR = build
BIN = client
IDIRS = include thirdparty/Chipmunk2D/include/chipmunk ../gamelogic ../protocol/include
SRCS = $(foreach SDIR, $(SDIRS), $(wildcard $(SDIR)/*.cpp) $(wildcard $(SDIR)/*.c))
OBJS = $(foreach OBJ, $(notdir $(SRCS:.cpp=.o)), $(ODIR)/$(OBJ:.c=.o))
DEPS = $(foreach IDIR, $(IDIRS), $(wildcard $(IDIR)/*.hpp) $(wildcard $(IDIR)/*.h))

all: linux

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

compile: $(OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $^ -o $(BDIR)/$(TARGET)/$(BIN)$(EXT) $(LDFLAGS)

#$(ODIR)/%.o: $(THIRDPARTY_SDIR)/%.c
#	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
#OBJS := $(OBJS) $(patsubst $(THIRDPARTY_SDIR)/%.c, $(ODIR)/%.o, $(THIRDPARTY_SDIR)/%.c)

set-linux: 
	$(eval TARGET := linux)
	$(eval CXX := /usr/bin/clang++)
	$(eval CC := /usr/bin/clang)
	$(eval CFLAGS := -I/usr/local/opt/openssl/include -I/usr/local/include -I/usr/include/SDL2 $(foreach IDIR, $(IDIRS), -I$(IDIR)))
	$(eval CPPFLAGS := -g -O3 -DHAS_TR1 -march=x86-64 -Wall)
	$(eval CXXFLAGS := -std=c++14)
	$(eval LDFLAGS := -L/usr/local/opt/openssl/lib -L/usr/local/lib -lpthread -lGL -lSDL2 -lSDL2_net -lSDL2_image -lSDL2_net -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
	$(eval EXT := )
	mkdir -p $(BDIR)/$(TARGET)
	cp -rf asset $(BDIR)/$(TARGET)/

set-web: 
	$(eval TARGET := web)
	$(eval CXX := em++)
	$(eval CC := emcc)
	$(eval CFLAGS := -I/usr/local/opt/openssl/include -I/usr/local/include -I/usr/include/SDL2 $(foreach IDIR, $(IDIRS), -I$(IDIR)))
	$(eval CPPFLAGS := -g -O3 -DHAS_TR1 -Wall -s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_NET=2 -s USE_SDL_IMAGE=2 -s DISABLE_EXCEPTION_CATCHING=0 -s ASSERTIONS=1 --preload-file asset -s EXPORTED_FUNCTIONS='["_main_", "_main"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall"]' -s SDL2_IMAGE_FORMATS='["png"]')
	$(eval CXXFLAGS := -std=c++14)
	$(eval LDFLAGS := -lm)
	$(eval EXT := .js)
	mkdir -p $(BDIR)/$(TARGET)
	cp -rf src/shell/* $(BDIR)/$(TARGET)/

set-win64: $(SOURCES)
	$(eval TARGET := win64)
	$(eval CXX := g++)
	$(eval CC := gcc)
	$(eval CFLAGS := -I/usr/local/opt/openssl/include -I/usr/local/include -I/usr/include/SDL2 -I$(IDIR))
	$(eval CPPFLAGS := -g -O3 -DHAS_TR1 -march=x86-64 -Wall)
	$(eval CXXFLAGS := -std=c++14)
	$(eval LDFLAGS := -L/usr/local/opt/openssl/lib -L/usr/local/lib -lpthread -lGL -lSDL2 -lSDL2_net -lSDL2_image -lSDL2_net -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lwsock32 -lws2_32)
	$(eval EXT := .exe)
	mkdir -p $(BDIR)/$(TARGET)
	cp -rf src/win32_runtime_library/* $(BDIR)/$(TARGET)/
	cp -rf asset $(BDIR)/$(TARGET)/

linux: set-linux compile
web: set-web compile
win64: set-win64 compile

clean:
	rm -rf $(BDIR)
	rm -f $(ODIR)/*.o
