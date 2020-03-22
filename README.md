# Introduction
This server program receives an image through C's socket API, detects a person through deep learning, and outputs the inference result and the number of people to a web page.   
You can also see client code here: https://github.com/wooseokyourself/capstone_pi.

# Description
This https://github.com/wooseokyourself/capstone_xu4 repo contains inference code for YOLOv3 in OpenCV DNN. The code works on Linux. Training is done on the COCO dataset by default: https://cocodataset.org/#home. Credit to Joseph Redmon for YOLO: https://pjreddie.com/darknet/yolo/.

# Test Environment and Requirements
** *This program is designed to run only CPU in Odroid XU4 (armv7l). Using GPU has not been tested.* **
* Ubuntu 16.04.6 LTS (Mate)
* gcc/g++ 4.9.4
* Makefile
* OpenCV 4.1.2 with contrib (actually contrib is unnecessary)
* Apache 2.4.18
* PHP 7.0.33   

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
   
   
# Web Structures
    $(WEB_ROOT)/
> Print out the most recent inference image and person count values. (index.php)

    $(WEB_ROOT)/inputs
> Directory to save input images.

    $(WEB_ROOT)/outputs
> Directory to save inference images and 'results.txt'.
   
   
# Application Protocol
    std::vector<unsigned char>
> Encoded form of cv::Mat. There is a process of decoding back to cv::Mat in this program.
   
   
   
# Install

### 1. Get YOLOv3 pre-trained model
    cd model
    ./getModels.sh
> You need to remove all classes but 'person' in 'coco.names' to inference only people.

   
### 2. Set root path of your web into $(WEB_ROOT) in Makefile
> The path **MUST be ABSOLUTE path**.
> Also there must be **'/'** in the end of your path.   
   ex. WEB_ROOT = /home/html/ws/www/
   
   
### 3. Compile

#### 3.1. Compile Release ver.   
    make all

#### 3.2. Compile Debugging ver.   
    make debug
> Only **"Input local images --> Inference --> Print into web page"** is performed **WITHOUT SOCKET COMMUNICATION**.
   
   
   
   
# Usage

### 1. Init root dir of your Web   
    make init
   
### 2. Run

#### 2.1. Run Release ver.   
    make run   
> + then the program stuck in listen() to wait connection request.   
> + also see client code here: https://github.com/wooseokyourself/capstone_pi.
   
#### 2.2. Run Debugging ver.   
    make debug_run
> + Program reads **.jpeg** files from '*debug/images/*' sequentially from 1.   
>     (ex. debug/images/*1.jpeg*, debug/images/*2.jpeg*, ... debug/images/**$(last).jpeg**)
> + 'make debug_run' will ask you the value of $(last)

---------------------------------------------------------------
   
   
# Others

### Removes *.out
    make clean


--------------------------------------------------------------
   
   
# Example Result of Web (index.php)

### from Raspi camera (Release ver.)
![시연2](https://user-images.githubusercontent.com/49421142/76927049-53dc3880-6921-11ea-9b9f-eae576d58d72.JPG)

### from test image (Debugging ver.)
![캡처](https://user-images.githubusercontent.com/49421142/76976201-fecb1180-6976-11ea-81db-f5a9195ffcca.JPG)

