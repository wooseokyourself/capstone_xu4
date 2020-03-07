TARGET = server.out
DLMODULE = dnn.out

all: $(TARGET)

dnn: $(DLMODULE)

delete:
	rm -f /home/html/ws/uploads/*.jpeg

clean:
	rm -f *.o
	rm -f $(TARGET) $(DLMODULE)

$(TARGET):
	g++ -DDEBUG -std=c++11 -w src/main.cpp src/ml.cpp src/socket.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET)

$(DLMODULE):
	g++ -DDEBUG_ML -std=c++11 -w debug/test_ml_main.cpp src/ml.cpp `pkg-config --cflags --libs opencv4` -o $(DLMODULE)
