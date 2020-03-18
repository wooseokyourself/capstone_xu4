# apache2 service's root
# It MUST be ABSOLUTE PATH and has '/' at the end of itself.
WEB_ROOT = /home/html/ws/www/

#-----------------------------------------------#

# Target
TARGET = .server.out
TARGET_DEBUG = .server_debug.out

# compiler, flags, libs
CXX = g++
CXXFLAGS = -std=c++11
OPENCV = `pkg-config --cflags --libs opencv4`

# Sources
RELEASE_SRC = src/common.cpp src/ml.cpp src/web.cpp src/socket.cpp src/main.cpp
DEBUG_SRC = -DDEBUG src/common.cpp src/ml.cpp src/web.cpp src/debug_main.cpp

#-----------------------------------------------#

# Compile and Run: release version

all: $(TARGET)
	@echo "Compile is done! Run with 'make run'"

$(TARGET):
	$(CXX) $(CXXFLAGS) $(RELEASE_SRC) $(OPENCV) -o $(TARGET)

run:
	./.server.out $(WEB_ROOT)

#-----------------------------------------------#

# Compile and Run: debug version

debug: $(TARGET_DEBUG)
	@echo "Compile is done! Run with 'make debug_run'"

$(TARGET_DEBUG):
	$(CXX) $(CXXFLAGS) $(DEBUG_SRC) $(OPENCV) -o $(TARGET_DEBUG)

debug_run:
	@read -p "Enter the last number of debug/images/*.jpeg: " LAST; \
	./.server_debug.out $(WEB_ROOT) $$LAST

#-----------------------------------------------#

# Make init and Remove target file

clean:
	rm -f *.o
	rm -f *.out

#-----------------------------------------------#

# Remove all subdirs and files in $(WEB_ROOT)

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
