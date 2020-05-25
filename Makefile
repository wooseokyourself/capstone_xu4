BIN = bin
CPPSRC = src/cpp
WEBAPP = src/webapp
RSRC = resources

#-----------------------------------------------#

# Target
CPP_TARGET = $(BIN)/a.out
CPP_MULTI_TARGET = $(BIN)/a.out
CPP_SINGLE_TARGET = $(BIN)/a.out

# compiler, flags, libs
CXX = g++
CXXFLAGS = -std=c++11 -pthread
OPENCV = `pkg-config --cflags --libs opencv4`

# Sources
RELEASE_SRC = $(CPPSRC)/common.cpp $(CPPSRC)/admin.cpp $(CPPSRC)/ml.cpp $(CPPSRC)/socket.cpp $(CPPSRC)/uploader.cpp $(CPPSRC)/main.cpp
MULTI_VIDEO_SRC =  $(CPPSRC)/common.cpp $(CPPSRC)/admin.cpp $(CPPSRC)/ml.cpp $(CPPSRC)/from_multi_video.cpp $(CPPSRC)/uploader.cpp $(CPPSRC)/multi_video_main.cpp

# Task options
OPT = #-DDIVIDE
#-----------------------------------------------#

# Compile: release version

all: $(CPP_TARGET)
	@echo "Compile is done! Run with 'make run'"

$(CPP_TARGET):
	$(CXX) $(CXXFLAGS) $(OPT) $(RELEASE_SRC) $(OPENCV) -o $(CPP_TARGET)

#-----------------------------------------------#

# Compile: debug version

multi: $(CPP_MULTI_TARGET)
	@echo "Multi Video's Compile is done!"

$(CPP_MULTI_TARGET):
	$(CXX) $(CXXFLAGS) $(OPT) $(MULTI_VIDEO_SRC) $(OPENCV) -o $(CPP_MULTI_TARGET)
	

#-----------------------------------------------#

# Run

run:
	node $(WEBAPP)/server.js

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
	git push -u mac master

#-----------------------------------------------#
