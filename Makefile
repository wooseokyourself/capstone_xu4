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

delete:
	rm -f /home/html/ws/tests/*.jpeg
	rm -f /home/html/ws/cam/*.jpeg

$(TARGET):
	g++ -std=c++11 -w src/main.cpp src/ml.cpp src/socket.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET)

$(TARGET_DEBUG):
	g++ -DDEBUG -std=c++11 -w debug/debug_main.cpp src/ml.cpp src/web.cpp `pkg-config --cflags --libs opencv4` -I/usr/include/mysql -o $(TARGET_DEBUG)