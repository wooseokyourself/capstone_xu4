TARGET = server.out
TARGET_DEBUG = server_debug.out

update:
	git add .
	git commit -m "update"
	git push -u mac master

clean:
	rm -f *.o
	rm -f *.out

all: $(TARGET)

debug: $(TARGET_DEBUG)

$(TARGET):
	g++ -std=c++11 -w src/common.cpp src/ml.cpp src/socket.cpp src/main.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET)

$(TARGET_DEBUG):
	g++ -DDEBUG -std=c++11 -w src/common.cpp src/ml.cpp src/web.cpp src/debug_main.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET_DEBUG)
