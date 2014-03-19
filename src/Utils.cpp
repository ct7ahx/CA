/*
 * Utils.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: ruben
 */
#include "Utils.h"

using namespace cv;

void drawObject(int x, int y,Mat &frame){

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
	if(y-25>0)
		line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
	if(y+25<height)
		line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(x,height),Scalar(0,255,0),2);
	if(x-25>0)
		line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
	if(x+25<width)
		line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(width,y),Scalar(0,255,0),2);

}

Mat convert_binary(Mat img_rgb){
	Mat img_bw;

	cvtColor( img_rgb, img_bw, CV_BGR2GRAY );
	return (img_bw > 128);
}
