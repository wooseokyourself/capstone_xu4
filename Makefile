TARGET = server.out
TARGET_DEBUG_ML = server_debug_ml.out

clean:
	rm -f *.o
	rm -f *.out

all: $(TARGET)

debug_ml: $(TARGET_DEBUG_ML)

delete:
	rm -f /home/html/ws/uploads/*.jpeg

$(TARGET):
	g++ -DDEBUG -std=c++11 -w src/main.cpp src/ml.cpp src/socket.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET)

$(TARGET_DEBUG_ML):
	g++ -DDEBUG_ML -std=c++11 -w debug/test_ml_main.cpp src/ml.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET_DEBUG_ML)
