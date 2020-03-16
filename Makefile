TARGET = server.out
TARGET_DEBUG = server_debug.out
TARGET_DEBUG_ML = server_debug_ml.out
TARGET_DEBUG_WEB = server_debug_web.out

update:
	git add .
	git commit -m "update"
	git push -u mac master

clean:
	rm -f *.o
	rm -f *.out

all: $(TARGET)

debug: $(TARGET_DEBUG)

debug_ml: $(TARGET_DEBUG_ML)

debug_web: $(TARGET_DEBUG_WEB)

delete:
	rm -f /home/html/ws/tests/*.jpeg
	rm -f /home/html/ws/cam/*.jpeg

$(TARGET):
	g++ -std=c++11 -w src/main.cpp src/ml.cpp src/socket.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET)

$(TARGET_DEBUG):
	g++ -DDEBUG -std=c++11 -w src/main.cpp src/ml.cpp src/socket.cpp `pkg-config --cflags --libs opencv4` -I/usr/include/mysql -o $(TARGET_DEBUG)

$(TARGET_DEBUG_ML):
	g++ -DDEBUG_ML -std=c++11 -w debug/test_ml_main.cpp src/ml.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET_DEBUG_ML)

$(TARGET_DEBUG_WEB):
	g++ -DDEBUG_WEB -std=c++11 -w debug/test_web_main.cpp src/ml.cpp src/web.cpp `pkg-config --cflags --libs opencv4` -I/usr/include/mysql -o $(TARGET_DEBUG_WEB)
