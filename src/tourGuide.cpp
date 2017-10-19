//Compile with g++ -Wall -o tourGuide -lAria -ldl `pkg-config --cflags --libs opencv` -lpthread -L /usr/local/Aria/lib -I/usr/local/Aria/include tourGuide.cpp
#include "../include/eyesim.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

/* defines */

#define CAMERA_ON 0

#define REQUIRED_ARGUMENTS 1
#define IMAGE_SCALE_FACTOR 10
#define WINDOW_SIZE 300 // window size of image display
#define SUBTRACT_HISTORY 50 // compared histroy of image in subtractor
#define SUBTRACT_THRESHOLD 3 // threshold value for subtractor


/* Namespace */
using namespace std;
using namespace cv;

/* Global Variables */

/* Declare Function */

void printUsage();

class MotionDetector {
public:
    MotionDetector(int history = 500, double varThreshold = 16,
                   bool detectShadows = true) {
        bgSubtractor = createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);
    }

    void imShowForeground(int windowSizeX = 0, int windowSizeY = 0) {

        if ((windowSizeX == 0) & (windowSizeY == 0)) {
            namedWindow("foreground");
        } else {
            namedWindow("foreground", WINDOW_NORMAL);
        }

        resizeWindow("foreground", windowSizeX, windowSizeY);
        imshow("foreground", foreground);
    }

    void imShowBackground(int windowSizeX = 0, int windowSizeY = 0) {

        if ((windowSizeX == 0) & (windowSizeY == 0)) {
            namedWindow("background");
        } else {
            namedWindow("background", WINDOW_NORMAL);
        }

        resizeWindow("background", windowSizeX, windowSizeY);
        imshow("background", background);
    }

    Mat Detect(Mat frame) {
        this->bgSubtractor->apply(frame, this->foreground);
        this->bgSubtractor->getBackgroundImage(this->background);
        erode(this->foreground, this->foreground, Mat());
        dilate(this->foreground, this->foreground, Mat());
        findContours(this->foreground, this->contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        drawContours(frame, this->contours, -1, Scalar(0, 0, 255), 2);
        return frame;
    }


private:
    Ptr<BackgroundSubtractorMOG2> bgSubtractor;
    Mat foreground;
    Mat background;
    vector<vector<Point>> contours;
};

/* Main */

int main(int argc, char *argv[]) {
    //Usage
//    if (argc != REQUIRED_ARGUMENTS) {
//        printUsage();
//        exit(EXIT_FAILURE);
//    }

    int scan[181];
    ArArgumentParser parser(&argc,argv);
    eyesim Pioneer(parser);



    Pioneer.SIMLaserScan(scan);

//    int lineSpeed,angSpeed;
//    Pioneer.SetMaxSpeed(500,100);
//    Pioneer.GetMaxSpeed(&lineSpeed,&angSpeed);
//    cout<<" linM: "<< lineSpeed<<" angM: "<<angSpeed<<endl;
//    Pioneer.VWStraight(1000,100);
//    while(!Pioneer.VWDone()){
//        Pioneer.VWGetSpeed(&lineSpeed,&angSpeed);
//        cout<<" linS: "<<lineSpeed<<" angS: "<<angSpeed<<endl;
//    }

    if(CAMERA_ON) {
        //  OpenCV
        VideoCapture cap;
        if (!cap.open(1)) {
            cout << "Unable to open webcam\n";
            exit(EXIT_FAILURE);
        }
        /* video thread */
        Mat frame;
        MotionDetector motionDetector = MotionDetector(SUBTRACT_HISTORY, SUBTRACT_THRESHOLD);

        cap >> frame;
        Size frameSize(40, 30);
        namedWindow("webcam", WINDOW_NORMAL);
        resizeWindow("webcam", frame.cols, frame.rows);

        while (waitKey(10) != 27) {
            cap >> frame;
            if (frame.empty()) break; // end of video stream
            resize(frame, frame, frameSize);
            frame = motionDetector.Detect(frame);
            imshow("webcam", frame);
        }
    }
    Aria::exit(0);
//    Pioneer.Terminate();
    return 0;
}

/* Define Functions */

/* Usage Information */
void printUsage() {
    cout << "Usage: " << endl;
    cout << "\ttourGuide ";
    // add arguments here for help information
    cout << "";
    cout << endl;
}