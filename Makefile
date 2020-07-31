BIN = bin
CPPSRC = cpp
WEBAPP = src/webapp # 수정필요
RSRC = resources
CFG = config

#-----------------------------------------------#

# Target
CPP_TARGET = $(BIN)/a.out
CPP_MULTI_TARGET = $(BIN)/a.out
CPP_SINGLE_TARGET = $(BIN)/a.out

# compiler, flags, libs
CXX = g++
CXXFLAGS = -std=c++11 -pthread
OPENCV = `pkg-config --cflags --libs opencv4`
JSONCPP = `pkg-config --cflags --libs jsoncpp`
FFI = `pkg-config --cflags --libs libffi`
RESTCLIENT = -I /usr/local/restclient-cpp/include -l restclient-cpp

# Sources
RELEASE_SRC = $(CPPSRC)/common.cpp $(CPPSRC)/admin.cpp $(CPPSRC)/ml.cpp $(CPPSRC)/socket.cpp $(CPPSRC)/uploader.cpp $(CPPSRC)/main.cpp
MULTI_VIDEO_SRC =  $(CPPSRC)/common.cpp $(CPPSRC)/admin.cpp $(CPPSRC)/ml.cpp $(CPPSRC)/from_multi_video.cpp $(CPPSRC)/uploader.cpp $(CPPSRC)/multi_video_main.cpp

#-----------------------------------------------#


# Compile: release version

all:
	$(CXX) $(CXXFLAGS) $(RELEASE_SRC) $(OPENCV) $(JSONCPP) $(FFI) $(RESTCLIENT) -o $(CPP_TARGET)
	@echo "Compile is done! Run with 'make run'"


#-----------------------------------------------#


# Compile: release version

debug:
	$(CXX) $(CXXFLAGS) $(MULTI_VIDEO_SRC) $(OPENCV) $(JSONCPP) $(FFI) $(RESTCLIENT) -o $(CPP_MULTI_TARGET)
	@echo "Compile is done! Run with 'make run'"
	

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

init: # 수정필요
	rm -rf $(RSRC)
	mkdir $(RSRC)
	mkdir $(RSRC)/images
	touch $(RSRC)/camera_ip.txt
	touch $(RSRC)/people.txt
	rm -rf $(CFG)
	mkdir $(CFG)
	mkdir $(CFG)/images
	touch $(CFG)/ROI.txt
	touch $(CFG)/admin_input.txt
	touch $(CFG)/mode.txt

#-----------------------------------------------#

# for myself
update:
	git add .
	git commit -m "debugging"
	git push -u mac master

#-----------------------------------------------#
