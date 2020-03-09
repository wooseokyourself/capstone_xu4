#include "ml.hpp"

OpenCV_DNN::OpenCV_DNN () {
    /* 외부 경로 설정 */
    this->MODEL_PATH = "model/yolov3.weights";
    this->CONFIG_PATH = "model/yolov3.cfg";
    this->CLASSES_PATH = "model/coco.names";

#ifdef DEBUG_ML
    this->INPUT_IMAGE_PATH = "/home/html/ws/tests/";
    this->OUTPUT_IMAGE_PATH = "/home/html/ws/tests/";
#else
    this->INPUT_IMAGE_PATH = "/home/html/ws/cam/";
    this->OUTPUT_IMAGE_PATH = "/home/html/ws/cam/";
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
   	
    people = 0;

	/* DNN 설정 */
	this->mean = Scalar();

    this->scale = 0.00392;
    this->swapRB = true;
    this->inpWidth = 416;
    this->inpHeight = 416;

    this->confThreshold = 0.4;
    this->nmsThreshold = 0.5;

    // Open file with classes names.
    string file = CLASSES_PATH;
    ifstream ifs(file.c_str());
    if (!ifs.is_open())
        CV_Error(Error::StsError, "File " + file + " not found");
    string line;
    while (getline(ifs, line)) {
        this->classes.push_back(line);
    }

	// 모델 로드
    this->net = readNet (MODEL_PATH, CONFIG_PATH);

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
    this->net.setPreferableBackend(DNN_BACKEND_OPENCV);

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
    this->net.setPreferableTarget(DNN_TARGET_CPU);
    this->outNames = net.getUnconnectedOutLayersNames();
}

#ifdef DEBUG_ML
void
OpenCV_DNN::MachineLearning (string TEST_IMAGE_FILE) {
    /*
	    test_ml_main.cpp 와 함께 컴파일되었다면,
	    Mat을 struct protocol 이 아닌 .jpeg 이미지파일로부터 생성함.
    */
    people = 0;
    Mat img;
    string SAVING_FILE = TEST_IMAGE_FILE;
    if (SAVING_FILE.length() == 6) {
        SAVING_FILE = "0" + TEST_IMAGE_FILE;
    }
    TEST_IMAGE_FILE = "debug/test_images/" + TEST_IMAGE_FILE;
    img = imread (TEST_IMAGE_FILE, IMREAD_COLOR);

    string output_file = OUTPUT_IMAGE_PATH + SAVING_FILE;


    /*  여기까지만 기존의 MachineLearning과 다르다. */

    /* Image Process */
    Mat blob;
	preprocess(img);

	vector<Mat> outs;
	net.forward(outs, outNames);

	postprocess(img, outs);


	/* 박스와 추론시간 기입 */
	vector<double> layersTimes;
	double freq = getTickFrequency() / 1000;
	double t = net.getPerfProfile(layersTimes) / freq;
	string label_inferTime = format ("Inference time: %.2f ms", t);
    string label_confThreshold = format ("confThreshold: %.1f", confThreshold);
    string label_resolution = format ("Resolution: %d X %d", img.cols, img.rows);
    string label_people = format ("People: %d", this->people);
	putText (img, label_inferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (img, label_confThreshold, Point(0, 70), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (img, label_resolution, Point(0, 105), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (img, label_people, Point(0, 140), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);

	imwrite (output_file, img);
}
#endif

void 
OpenCV_DNN::MachineLearning (std::vector<unsigned char> vec) {
    people = 0;
    Mat img = imdecode (vec, 1);
    vec.clear();
    
    string currTime = getCurrTime();
    string input_file = INPUT_IMAGE_PATH + currTime + ".jpeg";
    string output_file = OUTPUT_IMAGE_PATH + currTime + "_out.jpeg";

    imwrite (input_file, img);

    /* Image Process */
    Mat blob;
	preprocess(img);

	vector<Mat> outs;
	net.forward(outs, outNames);

	postprocess(img, outs);


	/* 박스와 추론시간 기입 */
	vector<double> layersTimes;
	double freq = getTickFrequency() / 1000;
	double t = net.getPerfProfile(layersTimes) / freq;
	string label_inferTime = format ("Inference time: %.2f ms", t);
    string label_confThreshold = format ("confThreshold: %.1f", confThreshold);
    string label_resolution = format ("Resolution: %d X %d", img.cols, img.rows);
    string label_people = format ("People: %d", this->people);
	putText (img, label_inferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (img, label_confThreshold, Point(0, 70), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (img, label_resolution, Point(0, 105), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (img, label_people, Point(0, 140), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);

	imwrite (output_file, img);
}


inline void
OpenCV_DNN::preprocess (const Mat& frame) {
    static Mat blob;
    Size inpSize = Size(this->inpWidth, this->inpHeight);
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
    blob = blobFromImage(frame, 1.0, inpSize, Scalar(), this->swapRB, false, CV_8U);

    // Run a model.
    this->net.setInput(blob, "", this->scale, this->mean);
    if (net.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
    {
        resize(frame, frame, inpSize);
        Mat imInfo = (Mat_<float>(1, 3) << inpSize.height, inpSize.width, 1.6f);
        this->net.setInput(imInfo, "im_info");
    }
}

void
OpenCV_DNN::postprocess (Mat& frame, const vector<Mat>& outs) {
    static vector<int> outLayers = this->net.getUnconnectedOutLayers();
    static string outLayerType = this->net.getLayer(outLayers[0])->type;

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
    NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {   
            int idx = indices[i];
        if (classIds[idx] == 0) { /* 사람인 경우에만 박스 그리기 */
            people++;
            Rect box = boxes[idx];
            drawPred(classIds[idx], confidences[idx], box.x, box.y,
                    box.x + box.width, box.y + box.height, frame);
        }
    }
}

/* 결과이미지에 박스 그리기 */
void
OpenCV_DNN::drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame) {
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0));

    string label = format("%.2f", conf);
    if (!this->classes.empty())
    {
        CV_Assert(classId < (int)this->classes.size());
        label = this->classes[classId] + ": " + label;
    }

    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - labelSize.height),
              Point(left + labelSize.width, top + baseLine), Scalar::all(255), FILLED);
    // putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar());
}

inline string
OpenCV_DNN::getCurrTime () {
    time_t timeObj = time(NULL);
	struct  tm* tm = localtime(&timeObj);
    string currTime, temp;
	currTime += to_string (tm->tm_year+1900);
    temp = to_string (tm->tm_mon+1);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
    temp = to_string (tm->tm_mday);
    if (temp.length() == 1) temp = "0" + temp;
    currTime += temp;
	temp = to_string (tm->tm_hour);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
	temp = to_string (tm->tm_min);
	if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
	temp = to_string (tm->tm_sec);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
    return currTime;
}