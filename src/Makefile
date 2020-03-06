TARGET = server.out
DLMODULE = dnn.out

all: $(TARGET)

dnn: $(DLMODULE)

clean:
	rm -f *.o
	rm -f $(TARGET) $(DLMODULE)

$(TARGET):
	g++ -DDEBUG -std=c++11 -w main.cpp ml.cpp socket.cpp web.cpp `pkg-config --cflags --libs opencv4` -o $(TARGET)

$(DLMODULE):
	g++ -DDEBUG_ML -std=c++11 -w test_ml_main.cpp ml.cpp `pkg-config --cflags --libs opencv4` -o $(DLMODULE)
