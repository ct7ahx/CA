/*
 * Utils.h
 *
 *  Created on: Mar 16, 2014
 *      Author: ruben
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <highgui.h>
#include <cv.h>

#define PI 3.14159265

using namespace cv;

///default capture width and height
const int height = 640;
const int width = 480;
const int upperLimit = 200;
const int lowerLimit= 220;
const int straightAngle= 37;

class Timer
{
public:
    Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec +
            (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_, end_;
};

void drawObject(int x, int y,Mat &frame);
Mat convert_binary(Mat img_rgb);

#endif /* UTILS_H_ */
