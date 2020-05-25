#include "ml.hpp"

OpenCV_DNN::OpenCV_DNN (Size resize_res, float confThreshold, float nmsThreshold) {
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

    // Admin Mode
    this->resize_res = resize_res;
    this->confThreshold = confThreshold;
    this->nmsThreshold = nmsThreshold;

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
}

void
OpenCV_DNN::update (const config_data& data) {
    /*
        data 의 각 데이터를 현재 클래스에 적용
    */
    this->resize_res = data.resize_res;
    this->confThreshold = data.confThreshold;
    this->nmsThreshold = data.nmsThreshold;
}

void
OpenCV_DNN::inference (io_data& _io_data) {
    for (int i=0; i<_io_data.imgs.size(); i++) {
        int each_pic_people_num = infer_util (_io_data.imgs[i]);
        _io_data.nums[i] = each_pic_people_num;
        _io_data.total_people_num += each_pic_people_num;
    }
}

/* Change @img to result image, and returns people number of the @img. */
int 
OpenCV_DNN::infer_util (Mat& img) {
    // Image processig.
    vector<Mat> outs;
    preprocess(img);
    net.forward(outs, outNames);
    int people_num = postprocess(img, outs);

    // Draw rect and other info in output image.
    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    
    string label_inferTime = format ("Inference time: %.2f ms", t);
    // string label_confThreshold = format ("confThreshold: %.1f", confThreshold);
    // string label_resolution = format ("Resolution: %d X %d", img.cols, img.rows);
    string label_people = format ("People: %d", people_num);
    putText (img, label_inferTime, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
    // putText (img, label_confThreshold, Point(0, 70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
    // putText (img, label_resolution, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 0.5);
    putText (img, label_people, Point(0, 70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);

    return people_num;
}


inline void
OpenCV_DNN::preprocess (Mat& frame) {
    imagePadding (frame);
    static Mat blob;
    // Create a 4D blob from a frame.
    if (this->resize_res.width <= 0)
        this->resize_res.width = frame.cols;
    if (this->resize_res.height <= 0)
        this->resize_res.height = frame.rows;

    blob = blobFromImage (frame, this->scalarfactor, this->resize_res, Scalar(), this->swapRB, false, CV_8U);

    // Run a model.
    this->net.setInput (blob, "", this->scale, this->mean);
    
    if (net.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
    {
        resize(frame, frame, this->resize_res);
        Mat imInfo = (Mat_<float>(1, 3) << this->resize_res.height, this->resize_res.width, 1.6f);
        this->net.setInput(imInfo, "im_info");
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
