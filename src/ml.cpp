#include "ml.hpp"

#ifdef DEBUG_ML
string TEST_IMAGE_PATH = "1.jpeg";
#endif

static string MODEL_PATH = "../model/yolov3.weights";
static string CONFIG_PATH = "../model/yolov3.cfg";
static string CLASSES_PATH = "../model/coco.names";
static string INPUT_IMAGE_PATH = "/home/html/ws/uploads/";
static string OUTPUT_IMAGE_PATH = "/home/html/ws/uploads/";

static float confThreshold = 0.4;
static float nmsThreshold = 0.5;

static vector<string> classes;

/*
	struct protocol --> struct decoded 변환
*/
struct decoded*
decoding (struct protocol* dataPtr) {
#ifdef DEBUG
	printf ("decoding() called..\n");
	printf ("ml.cpp/decoding() before decoding..  \n");
	printf (" >> data.prevBufSize = %d\n", dataPtr->prevBuf.size());
	printf (" >> data.currBufSize = %d\n", dataPtr->currBuf.size());
	printf (" >> data.diffValSize = %d\n", sizeof(dataPtr->diffValue));
	printf (" >> data.diffVal = %d\n", dataPtr->diffValue);
#endif

    struct decoded* decImgPtr = (struct decoded*) malloc (sizeof(struct decoded));

#ifdef DEBUG
	printf ("sizeof(struct decoded) = %d\n", sizeof(struct decoded));
	printf ("sizeof(decImgPtr) in decoding() = %d\n", sizeof(decImgPtr));
#endif

    /*
        Mat cv::imdecode	(	InputArray 	buf,
                                int 	flags 
                            )
        Parameters
            buf	    Input array or vector of bytes.
            flags	The same flags as in cv::imread, see cv::ImreadModes.
    */
	decImgPtr->prev = imdecode (dataPtr->prevBuf, 1);
    decImgPtr->curr = imdecode (dataPtr->currBuf, 1);
    decImgPtr->diffValue = dataPtr->diffValue;
#ifdef DEBUG
	time_t t = time(NULL);
	struct  tm tm = *localtime(&t);
	printf ("sizeof(Mat prevImg) = %d\n", sizeof(decImgPtr->prev));
	printf ("sizeof(Mat currImg) = %d\n", sizeof(decImgPtr->curr));
	string name;
	name += "debug/";
	name += to_string (tm.tm_year+1900);
	name += "-";
	name += to_string (tm.tm_mon+1);
	name += "-";
	name += to_string (tm.tm_mday);
	name += "_";
	name += to_string (tm.tm_hour);
	name += ":";
	name += to_string (tm.tm_min);
	name += ":";
	name += to_string (tm.tm_sec);
	string prevName = name + "_prev.jpeg";
	string currName = name + "_curr.jpeg";
	imwrite (prevName, decImgPtr->prev);
	imwrite (currName, decImgPtr->curr);
#endif
	free (dataPtr);
    return decImgPtr;
}


bool
setNet (Net& net) {
    net = readNet (MODEL_PATH, CONFIG_PATH);

    /*
        선호하는 백엔드를 지정. 목록은 다음과 같다.
        enum  	cv::dnn::Backend {
                    cv::dnn::DNN_BACKEND_DEFAULT = 0,
                    cv::dnn::DNN_BACKEND_HALIDE,
                    cv::dnn::DNN_BACKEND_INFERENCE_ENGINE,
                    cv::dnn::DNN_BACKEND_OPENCV,
                    cv::dnn::DNN_BACKEND_VKCOM,
                    cv::dnn::DNN_BACKEND_CUDA
        }
    */
    net.setPreferableBackend(DNN_BACKEND_OPENCV);

    /*
        선호하는 타겟 디바이스를 지정. 목록은 다음과 같다.
        enum  	cv::dnn::Target {
                    cv::dnn::DNN_TARGET_CPU = 0,
                    cv::dnn::DNN_TARGET_OPENCL,
                    cv::dnn::DNN_TARGET_OPENCL_FP16,
                    cv::dnn::DNN_TARGET_MYRIAD,
                    cv::dnn::DNN_TARGET_VULKAN,
                    cv::dnn::DNN_TARGET_FPGA,
                    cv::dnn::DNN_TARGET_CUDA,
                    cv::dnn::DNN_TARGET_CUDA_FP16
    }
    */
    net.setPreferableTarget(DNN_TARGET_CPU);
}

inline void
preprocess (const Mat& frame, Net& net, Size inpSize, float scale,
            const Scalar& mean, bool swapRB) {
    static Mat blob;
    // Create a 4D blob from a frame.
    if (inpSize.width <= 0)
        inpSize.width = frame.cols;
    if (inpSize.height <= 0)
        inpSize.height = frame.rows;

	/*
	Mat
	blobFromImage  (
			ImputArray	image,			입력영상. 1 or 3 or 4 채널
			double		scalarfactor=1.0,	입력영상 픽셀에 곱할 값
			const size&	size = Size(),		출력영상 크기
			const Scalar&	mean = Scalar(),	입력영상의 채널에서 뺄 평균 값.
								만약 image가 BGR 순서이고 swapRB가 true이면
								(R평균,G평균,B평균) 순서로 값 지정.
								인자 없으면 Scalar().
			bool		swapRB = false,		첫 번째 채널과 세 번째 채널을 바꿀 것인지
								결정하는 플래그.
								true이면 BGR --> RGB
			bool		crop = false		입력영상의 크기를 변경 후, 크롭(crop)을
								수행할 것인지 결정하는 플래그.
			int		ddepth			출력blob의 깊이. CV_32F 또는 CV_8U.
			)
	*/
    blob = blobFromImage(frame, 1.0, inpSize, Scalar(), swapRB, false, CV_8U);

    // Run a model.
    net.setInput(blob, "", scale, mean);
    if (net.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
    {
        resize(frame, frame, inpSize);
        Mat imInfo = (Mat_<float>(1, 3) << inpSize.height, inpSize.width, 1.6f);
        net.setInput(imInfo, "im_info");
    }
}

void
postprocess (Mat& frame, const vector<Mat>& outs, Net& net) {
    static vector<int> outLayers = net.getUnconnectedOutLayers();
    static string outLayerType = net.getLayer(outLayers[0])->type;

    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    if (outLayerType == "DetectionOutput")
    {
        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]
        CV_Assert(outs.size() > 0);
        for (size_t k = 0; k < outs.size(); k++)
        {
            float* data = (float*)outs[k].data;
            for (size_t i = 0; i < outs[k].total(); i += 7)
            {
                float confidence = data[i + 2];
                if (confidence > confThreshold)
                {
                    int left   = (int)data[i + 3];
                    int top    = (int)data[i + 4];
                    int right  = (int)data[i + 5];
                    int bottom = (int)data[i + 6];
                    int width  = right - left + 1;
                    int height = bottom - top + 1;
                    if (width <= 2 || height <= 2)
                    {
                        left   = (int)(data[i + 3] * frame.cols);
                        top    = (int)(data[i + 4] * frame.rows);
                        right  = (int)(data[i + 5] * frame.cols);
                        bottom = (int)(data[i + 6] * frame.rows);
                        width  = right - left + 1;
                        height = bottom - top + 1;
                    }
                    classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
                    boxes.push_back(Rect(left, top, width, height));
                    confidences.push_back(confidence);
                }
            }
        }
    }
    else if (outLayerType == "Region")
    {
        for (size_t i = 0; i < outs.size(); ++i)
        {
            // Network produces output blob with a shape NxC where N is a number of
            // detected objects and C is a number of classes + 4 where the first 4
            // numbers are [center_x, center_y, width, height]
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
            {
                Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                Point classIdPoint;
                double confidence;
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > confThreshold)
                {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(Rect(left, top, width, height));
                }
            }
        }
    }
    else
        CV_Error(Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);

    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}

/* 결과이미지에 박스 그리기 */
void
drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame) {
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0));

    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ": " + label;
    }

    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - labelSize.height),
              Point(left + labelSize.width, top + baseLine), Scalar::all(255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar());
}

/* 머신러닝을 이용하여 입력받은 파일을 분석하고 결과값 리턴 */
void 
MachineLearning (struct protocol* data) {
    time_t timeObj = time(NULL);
	struct  tm tm = *localtime(&timeObj);
    string currTime, temp;
	currTime += to_string (tm.tm_year+1900);
    temp = to_string (tm.tm_mon+1);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
    temp = to_string (tm.tm_mday);
    if (temp.length() == 1) temp = "0" + temp;
    currTime += temp;
	temp = to_string (tm.tm_hour);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
	temp = to_string (tm.tm_min);
	if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
	temp = to_string (tm.tm_sec);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;

    INPUT_IMAGE_PATH += currTime + ".jpeg";
    OUTPUT_IMAGE_PATH += currTime + "_out.jpeg";
	Mat img;

#ifdef DEBUG_ML
/*
	test_ml_main.cpp 와 함께 컴파일되었다면,
	Mat을 struct protocol 이 아닌 .jpeg 이미지파일로부터 생성함.
*/
	img = imread (TEST_IMAGE_PATH, IMREAD_COLOR);
    imwrite (INPUT_IMAGE_PATH, img);
    img.release();
    img = imread (INPUT_IMAGE_PATH, IMREAD_COLOR); // BGR channel

#else
    struct decoded* decImgPtr = decoding (data);
	/* 이후 decImgPtr.curr 을 딥러닝의 input으로 넣고, 나머지 멤버는 웹출력에서 활용 */

	img = decImgPtr->curr.clone(); // 이건 아마 BGR channel.
    imwrite (INPUT_IMAGE_PATH, img);
#endif

    /*
        원본 예제 실행 예 (frome image or video file)
        example_dnn_object_detection
        --config=[PATH-TO-DARKNET]/cfg/yolo.cfg
        --model=[PATH-TO-DARKNET]/yolo.weights
        --classes=object_detection_classes_pascal_voc.txt
        --width=416
        --height=416
        --scale=0.00392
        --input=[PATH-TO-IMAGE-OR-VIDEO-FILE]
        --rgb
    */
   	
	/* 이는 원본예제의 경우 파서에서 값이 결정되는데 실행예에서는 아예 컴파일옵션에 넣지않음. */
	Scalar mean = Scalar();

    float scale = 0.00392;
    bool swapRB = true;
    int inpWidth = 416;
    int inpHeight = 416;



    // Open file with classes names.
    string file = CLASSES_PATH;
    ifstream ifs(file.c_str());
    if (!ifs.is_open())
        CV_Error(Error::StsError, "File " + file + " not found");
    string line;
    while (getline(ifs, line)) {
        classes.push_back(line);
    }

	// 모델 로드
    Net net;
    setNet (net);
    vector<String> outNames = net.getUnconnectedOutLayersNames();

	/*
		!! 여기까지는 한번만 실행되면 이후 프로그램이 종료될 때까지 소멸되면 안되는 변수들
		!! 이 이하는 매 사진마다 실행되어야 함
	*/

    /* Image Process */
    Mat blob;
	preprocess(img, net, Size(inpWidth, inpHeight), scale, mean, swapRB);

	vector<Mat> outs;
	net.forward(outs, outNames);

	postprocess(img, outs, net);


	/* 박스와 추론시간 기입 */
	vector<double> layersTimes;
	double freq = getTickFrequency() / 1000;
	double t = net.getPerfProfile(layersTimes) / freq;
	string label_inferTime = format ("Inference time: %.2f ms", t);
    string label_confThreshold = format ("confThreshold : %.1f", confThreshold);
	putText (img, label_inferTime, Point(0, 15), FONT_HERSHEY_SIMPLEX, 3.5, Scalar(0, 255, 0));
    putText (img, label_confThreshold, Point(0, 30), FONT_HERSHEY_SIMPLEX, 3.5, Scalar(0, 255, 0));

	imwrite (OUTPUT_IMAGE_PATH, img);
}
