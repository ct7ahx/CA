#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

typedef struct _canny_def{
	Mat src_gray;
	char filter_name[20];
	vector<Point2i> filter_mc_points;
}define_canny;
define_canny struct_canny[3];

///Point2i coord = struct_canny[0].filter_mc_points.front();
Mat src;
Mat src_bgr[3];
Mat test_gray;
int thresh = 60;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* argv);

/** @function main */
int main( int argc, char** argv )
{
	///video capture object to acquire webcam feed
		VideoCapture capture;

	///open capture object at location zero (default location for webcam)
		capture.open(0);

	///set height and width of capture frame
		capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	while(1){
		/// Load source image and convert it to gray
		 src = imread( "Signs/bus A4.png",CV_LOAD_IMAGE_COLOR);
		//capture.read(src);

		///Get image from camera and split(or to get patern signals)
		//src = imread("Test_Signals/1.jpg",CV_LOAD_IMAGE_COLOR);
		//src = imread("Test_Signals/rgb.png",CV_LOAD_IMAGE_COLOR);
		split(src,src_bgr);
		/// Convert each filter to gray and blur it(CAN SPLIT INTO THREADS
		//EDIT: As the images are in RGB separated filters, it counts as grayscale images
		struct_canny[0].src_gray=src_bgr[0];
		struct_canny[1].src_gray=src_bgr[1];
		struct_canny[2].src_gray=src_bgr[2];
		blur(struct_canny[0].src_gray,struct_canny[0].src_gray, Size(3,3) );
		strcpy(struct_canny[0].filter_name,"CannyB");
		blur(struct_canny[1].src_gray,struct_canny[1].src_gray, Size(3,3) );
		strcpy(struct_canny[1].filter_name,"CannyG");
		blur(struct_canny[2].src_gray,struct_canny[2].src_gray, Size(3,3) );
		strcpy(struct_canny[2].filter_name,"CannyR");

	/// Create and show DEFAULT IMAGE
	char* source_window = "Source";
	namedWindow( source_window, CV_WINDOW_AUTOSIZE );
	imshow( source_window, src );
	///DEBUG RGB IMAGE
	imshow("B", src_bgr[0]);
	imshow("G", src_bgr[1]);
	imshow("R", src_bgr[2]);

		///Aply Canny to each filter RGB (CAN SPLIT INTO THREADS)
		createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh);//, thresh_callback ); //define threshold
		//Aply for each -> thresh_callback(num_args,Mat *src_grey,const char <window name>)
		thresh_callback( 1, (void *) &struct_canny[0]);
		thresh_callback( 1, (void *) &struct_canny[1]);
		thresh_callback( 1, (void *) &struct_canny[2]);

		cout << struct_canny[0].filter_mc_points;
		//Point2i ponto = struct_canny[0].filter_mc_points.front(); //tirar ponto
		//TODO:PERCORRER VECTOR COM TEST obtido
		//struct_canny[0].filter_mc_points.front()
		struct_canny[0].filter_mc_points.clear();
		waitKey(1);
	}
  return(0);
}

/** @function thresh_callback */
void thresh_callback(int num, void *argv)
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  char buf[20];

  define_canny *data = (define_canny *) argv;

  /// Detect edges using canny
  Canny( data->src_gray, canny_output, thresh, thresh*2, 3 );
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Get the moments
  vector<Moments> mu(contours.size() );
  for( int i = 0; i < contours.size(); i++ )
     { mu[i] = moments( contours[i], false );}

  ///  Get the mass centers:
  vector<Point2f> mc( contours.size() );
  for( int i = 0; i < contours.size(); i++ )
     { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
     //SO ESTE E PRECISO, PARTE DE BAIXO E DESCARTAVEL
     data->filter_mc_points.push_back(Point2i((int) mu[i].m10/mu[i].m00,(int) mu[i].m01/mu[i].m00 ));}

  /// Draw contours (Nao e preciso isto e treta!!!)
 Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
 /* for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }*/
  /////ATEE AQUI

  /// Calculate the area with the moments 00 and compare with the result of the OpenCV function
  //printf("\t Info: Area and Contour Length \n");
  for( int i = 0; i< contours.size(); i++ )
     {
       //printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
     drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 3, color, -1, 8, 0 );
     sprintf(buf,"%lf %lf",mc[i].x,mc[i].y);
    //  putText(drawing,buf,mc[i],FONT_HERSHEY_SIMPLEX,0.5,color);
     }

  /// Show in a window
  namedWindow( data->filter_name, CV_WINDOW_AUTOSIZE );
  imshow( data->filter_name, drawing );
}
