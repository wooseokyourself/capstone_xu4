# apache2 service's root
# it must be absolute path and has '/' at the end of itself
WEB_ROOT = /home/html/ws/www/

#-----------------------------------------------#

# target
TARGET = server.out
TARGET_DEBUG = server_debug.out

# compiler, flags, libs
CXX = g++
CXXFLAGS = -std=c++11 -W
OPENCV = `pkg-config --cflags --libs opencv4`

# sources
RELEASE_SRC = src/common.cpp src/ml.cpp src/web.cpp src/socket.cpp src/main.cpp
DEBUG_SRC = -DDEBUG src/common.cpp src/ml.cpp src/web.cpp src/debug_main.cpp

#-----------------------------------------------#

# compile and run: release version

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(OPENCV) $(RELEASE_SRC) -o $(TARGET)

run:
	./server.out $(WEB_ROOT)

#-----------------------------------------------#

# compile and run: debug version

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG):
	$(CXX) $(CXXFLAGS) $(OPENCV) $(DEBUG_SRC) -o $(TARGET_DEBUG)

run_debug:
	@read -p "Enter the last number of debug/images/*.jpeg: " LAST; \
	./server_debug.out $(WEB_ROOT) $$LAST

#-----------------------------------------------#

# make init and remove target file
clean:
	make init
	rm -f *.o
	rm -f *.out

#-----------------------------------------------#

# Remove all subdirs and files in $(WEB_ROOT).
init:
	rm -r $(WEB_ROOT)*
	cp -r -f src/web/* $(WEB_ROOT)
	mkdir $(WEB_ROOT)inputs
	mkdir $(WEB_ROOT)outputs
	touch $(WEB_ROOT)outputs/results.txt

#-----------------------------------------------#


#-----------------------------------------------#

# for myself
update:
	git add .
	git commit -m "update"
	git push -u mac master

#-----------------------------------------------#
