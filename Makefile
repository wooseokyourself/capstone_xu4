BIN = bin
CPPSRC = src/cpp
WEBAPP = src/webapp
RSRC = resources

#-----------------------------------------------#

# Target
CPP_TARGET = $(BIN)/a.out

# compiler, flags, libs
CXX = g++
CXXFLAGS = -std=c++11 -pthread
OPENCV = `pkg-config --cflags --libs opencv4`

# Sources
RELEASE_SRC = $(CPPSRC)/common.cpp $(CPPSRC)/admin.cpp $(CPPSRC)/ml.cpp $(CPPSRC)/socket.cpp $(CPPSRC)/uploader.cpp $(CPPSRC)/main.cpp
DEBUG_SRC = -DDEBUG 

# Task options
OPT = #-DDIVIDE
#-----------------------------------------------#

# Compile and Run: release version

all: $(CPP_TARGET)
	@echo "Compile is done! Run with 'make run'"

$(CPP_TARGET):
	$(CXX) $(CXXFLAGS) $(OPT) $(RELEASE_SRC) $(OPENCV) -o $(CPP_TARGET)

run:
	node $(WEBAPP)/server.js

#-----------------------------------------------#
## NOT USED!
# Compile and Run: debug version

debug: $(TARGET_DEBUG)
	@echo "Compile is done! Run with 'make debug_run'"

$(TARGET_DEBUG):
	$(CXX) $(CXXFLAGS) $(OPT) $(DEBUG_SRC) $(OPENCV) -o $(TARGET_DEBUG)

debug_run:
	@read -p "Enter the last number of debug/images/*.jpeg: " LAST; \
	./.server_debug.out $(WEB_ROOT) $$LAST

#-----------------------------------------------#

# Make init and Remove target file

clean:
	rm -f $(BIN)/*.out

#-----------------------------------------------#

# Remove all subdirs and files in $(WEB_ROOT)

init:
	rm -rf $(RSRC)
	mkdir $(RSRC)
	mkdir $(RSRC)/images
	touch $(RSRC)/camera_ip.txt
	touch $(RSRC)/people.txt

#-----------------------------------------------#

# for myself
update:
	git add .
	git commit -m "debugging"
	git push -u mac nodejs

#-----------------------------------------------#
