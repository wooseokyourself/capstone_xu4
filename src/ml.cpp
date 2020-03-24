#include "ml.hpp"

OpenCV_DNN::OpenCV_DNN () {
    // Set path.
    this->MODEL_PATH = "model/yolov3.weights";
    this->CONFIG_PATH = "model/yolov3.cfg";
    this->CLASSES_PATH = "model/coco.names";
   	
    people = 0;

	// Set DNN.
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
    ASSERT (ifs.is_open() == true);
    string line;
    while (getline(ifs, line)) {
        this->classes.push_back(line);
    }

	// Load model.
    this->net = readNet (MODEL_PATH, CONFIG_PATH);

    /*
    Set Backend. References here:
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
    Set Target device. References here:
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

/* original
void 
OpenCV_DNN::MachineLearning (Mat inputImg) {
    this->outputImg = inputImg.clone();
    this->people = 0;
    
    // Image processig.
	preprocess(outputImg);

	vector<Mat> outs;
	net.forward(outs, outNames);

	postprocess(outputImg, outs);

	// Draw rect and other info in output image.
	vector<double> layersTimes;
	double freq = getTickFrequency() / 1000;
	double t = net.getPerfProfile(layersTimes) / freq;
	string label_inferTime = format ("Inference time: %.2f ms", t);
    string label_confThreshold = format ("confThreshold: %.1f", confThreshold);
    string label_resolution = format ("Resolution: %d X %d", outputImg.cols, outputImg.rows);
    string label_people = format ("People: %d", this->people);
	putText (outputImg, label_inferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (outputImg, label_confThreshold, Point(0, 70), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (outputImg, label_resolution, Point(0, 105), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (outputImg, label_people, Point(0, 140), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
}
*/

void 
OpenCV_DNN::MachineLearning (Mat inputImg) {
    this->outputImg = inputImg.clone();
    this->people = 0;

    // Image processig.
	// preprocess(outputImg);
    int halfWidth, halfHeight;

    if (outputImg.cols % 2 == 0)
        halfWidth = outputImg.cols/2;
    else
        halfWidth = outputImg.cols/2 - 1;

    if (outputImg.rows % 2 == 0)
        halfHeight = outputImg.rows/2;
    else
        halfHeight = outputImg.rows/2 - 1;
    
    // Shallow copy
    Mat LT = Mat(outputImg, Rect (0, 0, halfWidth, halfHeight));
    Mat RT = Mat(outputImg, Rect (halfWidth, 0, halfWidth, halfHeight));
    Mat LD = Mat(outputImg, Rect (0, halfHeight, halfWidth, halfHeight));
    Mat RD = Mat(outputImg, Rect (halfWidth, halfHeight, halfWidth, halfHeight));
    vector<Mat> outsLT, outsRT, outsLD, outsRD;

    preprocess (LT);
    net.forward(outsLT, outNames);
    postprocess(LT, outsLT);

    preprocess (RT);
    net.forward(outsRT, outNames);
    postprocess(RT, outsRT);

    preprocess (LD);
    net.forward(outsLD, outNames);
    postprocess(LD, outsLD);

    preprocess (RD);
    net.forward(outsRD, outNames);
    postprocess(RD, outsRD);

	// Draw rect and other info in output image.
	vector<double> layersTimes;
	double freq = getTickFrequency() / 1000;
	double t = net.getPerfProfile(layersTimes) / freq;
	string label_inferTime = format ("Inference time of last picture: %.2f ms", t);
    string label_confThreshold = format ("confThreshold: %.1f", confThreshold);
    string label_resolution = format ("Resolution: %d X %d", outputImg.cols, outputImg.rows);
    string label_people = format ("People: %d", this->people);
	putText (outputImg, label_inferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (outputImg, label_confThreshold, Point(0, 70), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (outputImg, label_resolution, Point(0, 105), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    putText (outputImg, label_people, Point(0, 140), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    line(Mat& outputImg, Point(0, halfHeight), Point(outputImg.cols, halfHeight), Scalar(255, 0, 0), int thickness=1, int lineType=8, int shift=0);
    line(Mat& outputImg, Point(halfWidth, 0), Point(halfWidth, outputImg.rows), Scalar(255, 0, 0), int thickness=1, int lineType=8, int shift=0);
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
        if (classIds[idx] == 0) { // Draw rectangle if class is a person.
            people++;
            Rect box = boxes[idx];
            drawPred(classIds[idx], confidences[idx], box.x, box.y,
                    box.x + box.width, box.y + box.height, frame);
        }
    }
}

/* Draw rectangle in frame. */
void
OpenCV_DNN::drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame) {
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0));

    /* // Write additional info (class name, inference rate) in rectangle.

    string label = format("%.2f", conf);
    if (!this->classes.empty())
    {
        CV_Assert(classId < (int)this->classes.size());
        label = this->classes[classId] + ": " + label;
    }

    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    top = max(top, labelSize.height);

    rectangle(frame, Point(left, top - labelSize.height), Point(left + labelSize.width, top + baseLine), Scalar::all(255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar());
    
    */
}
