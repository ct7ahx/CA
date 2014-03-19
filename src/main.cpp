#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <highgui.h>
#include <cv.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

#include "Utils.h"
#include "SerialPort.h"

#define N_DIV 10

using namespace cv;
using namespace std;

Timer timer;
vector<float> linReg;
vector<Point> continuousEdge;
vector<Point>  discontinuousEdge;
SerialPort serialPort;

void detect_lines(Mat frame_bin, Mat frame_rgb, string frame_name){
	int intensity=0, col, line, count=0;
	Mat img;
	std::stringstream message;
	char dir;

	img = Scalar::all(0);
	frame_rgb.copyTo(img, frame_bin);

	continuousEdge.clear();
	discontinuousEdge.clear();

	for( line=lowerLimit ; line>upperLimit ; line-=(lowerLimit-upperLimit)/(N_DIV+2)){
		for( col=height/2 ; col<height ; col++ ){
			intensity = frame_bin.at<uchar>(Point(col,line));
			if(intensity!=0){
				drawObject(col, line, img);
				continuousEdge.push_back(Point(col,line));
				count++;
				break;
			}
		}
		for( col=height/2 ; col>0 ; col-- ){
			intensity = frame_bin.at<uchar>(Point(col,line));
			if(intensity!=0){
				drawObject(col, line, img);
				discontinuousEdge.push_back(Point(col,line));
				count++;
				break;
			}
		}
	}

	fitLine(continuousEdge, linReg, CV_DIST_L2, 0, 0.01, 0.01);

	float teta = atan2(linReg.at(1),linReg.at(0)) * 180 / PI;
	cout  << "angulo: "<< teta << endl;

	if(straightAngle > teta){
		dir = 'r';
		teta=straightAngle-teta;
	}
	else{
		dir = 'l';
		teta=teta-straightAngle;
	}
	message << "s" << dir << teta;
	serialPort.sendArray(message.str(), message.tellp());
	imshow(frame_name,img);



}

int main()
{
	VideoCapture webcam(3);
	Mat frame;

	webcam.set(CV_CAP_PROP_FRAME_WIDTH, height);
	webcam.set(CV_CAP_PROP_FRAME_HEIGHT, width);

	if (serialPort.connect("//dev//ttymc3")==0) {
		cout << "Can't open serial port" << endl;
		return -1;
	}
	while(true)
	{
		timer.reset();
		webcam.read(frame);
		cv::blur( frame, frame, Size(4,4) );
		imshow("Blured",frame);

		Mat frame_can = Mat::zeros( frame.size(), CV_8UC3 );
		cv::Canny( frame, frame_can, 50, 200, 3 );
		detect_lines(frame_can, frame, "Canny");

		waitKey(5);

		cout << "Timer: "  << timer.elapsed() << endl;

	}
	return 0;
}
