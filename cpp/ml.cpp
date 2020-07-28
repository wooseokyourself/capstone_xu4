#include "ml.hpp"

OpenCV_DNN::OpenCV_DNN (const ConfigData& data) {
    // Set path.
    this->MODEL_PATH = BIN_PATH + "/model/yolov3.weights";
    this->CFG_PATH = BIN_PATH + "/model/yolov3.cfg";
    this->CLASSES_PATH = BIN_PATH + "/model/coco.names";

    // Set DNN.
    this->mean = Scalar(); // 0
    // (each pixel of blob) == (each pixel of input image * @scale * @scalarfactor)
    this->scalarfactor = 1; // parameter of blobFromImage()
    this->scale = 1/255.0; // parameter of net.setInput()
    this->swapRB = true;

    // Open file with classes names.
    string file = CLASSES_PATH;
    ifstream ifs(file.c_str());
    ASSERT (ifs.is_open() == true);
    string line;
    while (getline(ifs, line)) {
        this->classes.push_back(line);
    }

	// Load model.
    this->net = readNet (MODEL_PATH, CFG_PATH);
    this->net.setPreferableBackend(DNN_BACKEND_OPENCV);
    this->net.setPreferableTarget(DNN_TARGET_CPU);
    this->outNames = net.getUnconnectedOutLayersNames();

    // Admin Mode initial setting
    this->update (data);
}

/* data 의 config 정보들을 dnn 클래스의 설정값에 적용 */
void
OpenCV_DNN::update (const ConfigData& data) {
    this->resizeRes.width = data.resizeResWidth;
    this->resizeRes.height = data.resizeResHeight;
    this->confThreshold = data.confThreshold;
    this->nmsThreshold = data.nmsThreshold;
    this->overlaps = data.overlaps;
}

void
OpenCV_DNN::inference (IOdata& ioData) {
    printf ("in ioData, number of image is %d\n", ioData.imgs.size());
    for (int i=0; i<ioData.imgs.size(); i++) {
        int eachPicPeopleNum = inferUtil (i, ioData.imgs[i]);
        ioData.nums[i] = eachPicPeopleNum;
        ioData.totalPeopleNum += eachPicPeopleNum;
    }
}

/* Change @img to result image, and returns people number of the @img. */
int 
OpenCV_DNN::inferUtil (const int& idx, Mat& img) {
    // Image processig.
    vector<Mat> outs;
    preprocess(idx, img);
    net.forward(outs, outNames);
    int peopleNum = postprocess(img, outs);

    // Draw rect and other info in output image.
    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    
    string labelInferTime = format ("Inference time: %.2f ms", t);
    // string labelConfThreshold = format ("confThreshold: %.1f", confThreshold);
    // string labelResolution = format ("Resolution: %d X %d", img.cols, img.rows);
    string labelPeople = format ("People: %d", peopleNum);
    putText (img, labelInferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
    // putText (img, labelConfThreshold, Point(0, 70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
    // putText (img, labelResolution, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 0.5);
    putText (img, labelPeople, Point(0, 70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);

    return peopleNum;
}


inline void
OpenCV_DNN::preprocess (const int& idx, Mat& frame) {
    removeOverlaps (idx, frame);
    imagePadding (frame);
    static Mat blob;
    // Create a 4D blob from a frame.
    if (this->resizeRes.width <= 0)
        this->resizeRes.width = frame.cols;
    if (this->resizeRes.height <= 0)
        this->resizeRes.height = frame.rows;

    blob = blobFromImage (frame, this->scalarfactor, this->resizeRes, Scalar(), this->swapRB, false, CV_8U);

    // Run a model.
    this->net.setInput (blob, "", this->scale, this->mean);
    
    if (net.getLayer(0)->outputNameToIndex("imInfo") != -1)  // Faster-RCNN or R-FCN
    {
        resize(frame, frame, this->resizeRes);
        Mat imInfo = (Mat_<float>(1, 3) << this->resizeRes.height, this->resizeRes.width, 1.6f);
        this->net.setInput(imInfo, "imInfo");
    }
}

/* Returns people number */
int
OpenCV_DNN::postprocess (Mat& frame, const vector<Mat>& outs) {
    int people = 0;
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
    return people;
}

void
OpenCV_DNN::removeOverlaps (const int& idx, Mat& frame) {
    const vector<int>& ref = this->overlaps[idx];
    for (int i=0; i<ref.size(); i+=4) {
        int width = ref[i+2] - ref[i];
        int height = ref[i+3] - ref[i+1];
        int x = ref[i];
        int y = ref[i+1];
        rectangle (frame, Point(ref[i], ref[i+1]), Point(ref[i+2], ref[i+3]), Scalar(255,255,255), FILLED);
    }
}

/* Make the frame a square with padded space. */
void
OpenCV_DNN::imagePadding (Mat& frame) {
    if (frame.rows == frame.cols)
        return;

    int length = frame.cols > frame.rows ? frame.cols : frame.rows;
    if (frame.cols < length) {
        Mat pad (length, length - frame.cols, frame.type(), Scalar(255, 255, 255));
        hconcat (pad, frame, frame);
    }
    else {
        Mat pad (length - frame.rows, length, frame.type(), Scalar(255, 255, 255));
        vconcat (pad, frame, frame);
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
