# READ.md is being written now...

# Introduction
This directory contains server-side of OpenCV DNN YOLOv3 Project developed by __21500740@handong.edu__, __21500463@handong.edu__, and __21700551@handong.edu__, as part of 20-1 Capstone Design at Handong Global University's Department of Computer Science and Electrical Engineering, and is **freely available under the [Beerware license](https://github.com/wooseokyourself/capstone_xu4/blob/master/LICENSE).**

# Description
This https://github.com/wooseokyourself/capstone_xu4 repo contains code for receiveing images through C's socket API, inference for YOLOv3 in OpenCV DNN, and printing inference results in web server. The code works on Linux.   
Training is done on the COCO dataset by default: https://cocodataset.org/.   
**Credit to Joseph Redmon for YOLO:** https://pjreddie.com/darknet/yolo/.   
You can also see [Client code here](https://github.com/wooseokyourself/capstone_pi).

# Test Environment and Requirements
#### *This program is designed to run only CPU in Odroid XU4 (armv7l). Using GPU has not been tested.*
#### *All requirements below have not been tested in other versions.*
* Ubuntu 16.04.6 LTS (Mate)
* gcc/g++ 4.9.4
* GNU Make 4.1
* CMake 3.5.1
* OpenCV 4.1.2
* Node.js 12.16.3
* npm 6.14.4

#### *OpenCV Build Options in Test Environment*

~~~
General configuration for OpenCV 4.1.2 =====================================
  Version control:               unknown

  Extra modules:
    Location (extra):            /home/odroid/opencv_package/opencv_contrib/modules
    Version control (extra):     unknown

  Platform:
    Timestamp:                   2020-02-10T16:13:16Z
    Host:                        Linux 3.10.106-153 armv7l
    CMake:                       3.5.1
    CMake generator:             Unix Makefiles
    CMake build tool:            /usr/bin/make
    Configuration:               RELEASE

  CPU/HW features:
    Baseline:                    VFPV3 NEON FP16
      requested:                 DETECT
      required:                  VFPV3 NEON

  C/C++:
    Built as dynamic libs?:      YES
    C++ Compiler:                /usr/bin/c++  (ver 4.9.4)
    C++ flags (Release):         -DTBB_USE_GCC_BUILTINS=1   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wundef -Winit-self -Wpointer-arith -Wsign-promo -Wuninitialized -Winit-self -Wno-delete-non-virtual-dtor -Wno-comment -Wno-missing-field-initializers -fdiagnostics-show-option -pthread -fomit-frame-pointer -ffunction-sections -fdata-sections  -mfpu=neon-vfpv4 -fvisibility=hidden -fvisibility-inlines-hidden -O3 -DNDEBUG  -DNDEBUG
    C++ flags (Debug):           -DTBB_USE_GCC_BUILTINS=1   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wundef -Winit-self -Wpointer-arith -Wsign-promo -Wuninitialized -Winit-self -Wno-delete-non-virtual-dtor -Wno-comment -Wno-missing-field-initializers -fdiagnostics-show-option -pthread -fomit-frame-pointer -ffunction-sections -fdata-sections  -mfpu=neon-vfpv4 -fvisibility=hidden -fvisibility-inlines-hidden -g  -O0 -DDEBUG -D_DEBUG
    C Compiler:                  /usr/bin/cc
    C flags (Release):           -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wuninitialized -Winit-self -Wno-comment -Wno-missing-field-initializers -fdiagnostics-show-option -pthread -fomit-frame-pointer -ffunction-sections -fdata-sections  -mfpu=neon-vfpv4 -fvisibility=hidden -O3 -DNDEBUG  -DNDEBUG
    C flags (Debug):             -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wuninitialized -Winit-self -Wno-comment -Wno-missing-field-initializers -fdiagnostics-show-option -pthread -fomit-frame-pointer -ffunction-sections -fdata-sections  -mfpu=neon-vfpv4 -fvisibility=hidden -g  -O0 -DDEBUG -D_DEBUG
    Linker flags (Release):      -Wl,--gc-sections  
    Linker flags (Debug):        -Wl,--gc-sections  
    ccache:                      NO
    Precompiled headers:         NO
    Extra dependencies:          dl m pthread rt
    3rdparty dependencies:

  OpenCV modules:
    To be built:                 aruco bgsegm bioinspired calib3d ccalib core datasets dnn dnn_objdetect dnn_superres dpm face features2d flann freetype fuzzy gapi hfs highgui img_hash imgcodecs imgproc line_descriptor ml objdetect optflow phase_unwrapping photo plot python2 python3 quality reg rgbd saliency shape stereo stitching structured_light superres surface_matching text tracking ts video videoio videostab xfeatures2d ximgproc xobjdetect xphoto
    Disabled:                    world
    Disabled by dependency:      -
    Unavailable:                 cnn_3dobj cudaarithm cudabgsegm cudacodec cudafeatures2d cudafilters cudaimgproc cudalegacy cudaobjdetect cudaoptflow cudastereo cudawarping cudev cvv hdf java js matlab ovis sfm viz
    Applications:                perf_tests apps
    Documentation:               NO
    Non-free algorithms:         NO

  GUI: 
    GTK+:                        YES (ver 3.18.9)
      GThread :                  YES (ver 2.48.2)
      GtkGlExt:                  NO
    VTK support:                 NO

  Media I/O: 
    ZLib:                        /usr/lib/arm-linux-gnueabihf/libz.so (ver 1.2.8)
    JPEG:                        /usr/lib/arm-linux-gnueabihf/libjpeg.so (ver 80)
    WEBP:                        build (ver encoder: 0x020e)
    PNG:                         /usr/lib/arm-linux-gnueabihf/libpng.so (ver 1.2.54)
    TIFF:                        /usr/lib/arm-linux-gnueabihf/libtiff.so (ver 42 / 4.0.6)
    OpenEXR:                     build (ver 2.3.0)
    HDR:                         YES
    SUNRASTER:                   YES
    PXM:                         YES
    PFM:                         YES

  Video I/O:
    DC1394:                      NO
    FFMPEG:                      YES
      avcodec:                   YES (56.60.100)
      avformat:                  YES (56.40.101)
      avutil:                    YES (54.31.100)
      swscale:                   YES (3.1.101)
      avresample:                NO
    GStreamer:                   YES (1.8.3)
    v4l/v4l2:                    YES (linux/videodev2.h)

  Parallel framework:            pthreads

  Trace:                         YES (with Intel ITT)

  Other third-party libraries:
    Lapack:                      NO
    Eigen:                       YES (ver 3.2.92)
    Custom HAL:                  YES (carotene (ver 0.0.1))
    Protobuf:                    build (3.5.1)

  OpenCL:                        YES (no extra features)
    Include path:                /home/odroid/opencv_package/opencv/3rdparty/include/opencl/1.2
    Link libraries:              Dynamic load

  Python 2:
    Interpreter:                 /usr/bin/python2.7 (ver 2.7.12)
    Libraries:                   /usr/lib/arm-linux-gnueabihf/libpython2.7.so (ver 2.7.12)
    numpy:                       /usr/lib/python2.7/dist-packages/numpy/core/include (ver 1.11.0)
    install path:                lib/python2.7/dist-packages/cv2/python-2.7

  Python 3:
    Interpreter:                 /usr/bin/python3 (ver 3.5.2)
    Libraries:                   /usr/lib/arm-linux-gnueabihf/libpython3.5m.so (ver 3.5.2)
    numpy:                       /usr/lib/python3/dist-packages/numpy/core/include (ver 1.11.0)
    install path:                lib/python3.5/dist-packages/cv2/python-3.5

  Python (for build):            /usr/bin/python2.7

  Java:                          
    ant:                         NO
    JNI:                         NO
    Java wrappers:               NO
    Java tests:                  NO

  Install to:                    /usr/local
-----------------------------------------------------------------
~~~
   
   
# Application Protocol
    std::vector<unsigned char>
* Encoded form of cv::Mat. There is a process of decoding back to cv::Mat in this program.
   
   
   
# Install

### 1. Get YOLOv3 pre-trained model
    cd bin/model
    ./getModels.sh
* This program infers only 'person' class.
* Nevertheless, you need to remove all classes but 'person' in 'bin/model/coco.names' to shorten the elapsed time of inference.

   
### 2. Compile

#### 2.1. Compile Release ver.   
    make all

#### 2.2. Compile Debugging ver.   
    make debug
* Only **"Input local videos(mp4 format) --> Inference --> Print into web page"** is performed **WITHOUT SOCKET COMMUNICATION**.
* The local videos are stored in bin/testvideos and its name must be ascending order from 1.
* For example, if there are two videos, then the naming is required:    
bin/testvideos/1.mp4    
bin/testvideos/2.mp4
   
   
# Run

### 1. Init all configuration data and resources   
    make init
* Notice. It **REMOVES ALL CONFIGURATION DATA AND PREVIOUS OUTPUTS**.   
* You can omit this step.

### 2. Create directories to store the result images.
    mkdir resources/images/$(cameraID)
* This directories should exist in ascending order from 1, depending on the number of cameras.
* For example, if there are two cameras, then this directories are required:    
resources/images/1    
resources/images/2    

### 3. Run

    make run   
* Then you can now access the webpage with either public IP or private IP.
* Default port number of web is 10051 and this variable is stored in @express_port in  src/webapp/server.js
* If you compiled Debugging ver., then the program will read **.mp4** files from '*bin/testvideos/*' sequentially from 1.   
    (ex. bin/testvideos/*1.mp4*, bin/testvideos/*2.mp4*, ...)
* Also see [Client code here](https://github.com/wooseokyourself/capstone_pi).
  
   
# Others

+ #### Cancel compilation
~~~
    make clean
~~~
   
# Example Results

## from Raspi camera (Release ver.)
![2020_04_09_19_01_22](https://user-images.githubusercontent.com/49421142/78919234-992cf980-7acc-11ea-9450-b354f0d26eb4.jpeg)
![2020_04_09_18_54_45](https://user-images.githubusercontent.com/49421142/78919433-dc876800-7acc-11ea-98d3-3cec899e4a62.jpeg)


## from test images (Debugging ver.)
![result 4](https://user-images.githubusercontent.com/49421142/77255817-96996a00-6cad-11ea-9a56-aacc81d4889f.JPG)
![result 5](https://user-images.githubusercontent.com/49421142/77255821-98632d80-6cad-11ea-85e8-13ec533ee69e.JPG)
![result 6](https://user-images.githubusercontent.com/49421142/77255822-9ac58780-6cad-11ea-9b11-a4bf53ba78d5.JPG)
![result 7](https://user-images.githubusercontent.com/49421142/77255824-9bf6b480-6cad-11ea-9a36-721638835bf9.JPG)
