//TODO: MUDAR PARAMETROS DA IMAGEM esta a detectar poucos pontos
//tentar diminuir imagem ao minimo


#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//DEBUG
#define RAW_SIGNAL	true
#define DEBUG 		true

#define BOUNDING_AREA 1500		//Min area to consider for analysis
#define BINARY_THRESHOLD 50		//Binary Threshold for filter binary calculation
#define RESIZE_XY_SIZE Size(300,300)
#define CANNY_THRESHOLD 200

#define TINY_NAME_BYTES		10
#define SMALL_NAME_BYTES	50

using namespace cv;
using namespace std;

// set parameters

int numKeyPoints = 2500;
float distThreshold = 30.0;

//default capture width and height
#define CAMERA_FEED 1
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

typedef struct _define_raw_test{
	Mat src_gray;

	Mat bin;
	Mat equalized_histogram;
	string filter_name;
	vector<Point2i> filter_mc_points;
}define_raw_test;
define_raw_test src_filtered[3];

typedef struct _compare_signal{
	Mat ref;
	string name;
	Mat src_gray;
	vector<KeyPoint> keypoints;
	Mat descriptors;
}define_compare_signal;
vector<define_compare_signal> compare_signals;

vector<Mat> possible_signals;

//Camera Image
Mat src;
Mat src_bgr[3];
Mat src_gray;
vector<KeyPoint> keypoints;
Mat descriptors;
int thresh = CANNY_THRESHOLD;

//Other
Mat aux_mat[3];
Mat src_puro;

/// Function header
void find_possible_signals(int num, void *argv);
//void test_compare_for_signal(void*arg);

/** @function main */
int main( int argc, char** argv )
{
#if(RAW_SIGNAL == true)
	VideoCapture capture;
	capture.open(CAMERA_FEED);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
#endif
	//Get Signals and Resize
	const char *lista_sinais = "Signs/Signals.txt";	//caminho que contem todos os sinais para contra-teste
	fstream fp;
	fp.open(lista_sinais,std::fstream::in);
	Mat aux, resize_aux;
	Mat Aux[3];
	string name, append = "Signs/";;
	Ptr<FeatureDetector> detector(new OrbFeatureDetector(5000));
	Ptr<DescriptorExtractor> extractor(new OrbDescriptorExtractor(5000));
	BFMatcher matcher(NORM_HAMMING2);
	vector<vector<DMatch> > matches;
	Mat img_matches;
	Mat testest;
	int best_match;
	int number_best_match;

	while(!fp.eof()){
		define_compare_signal aux_compare_signal;
		fp >> name;
		name = (append + name);
		aux_compare_signal.name = name;
		aux = imread(aux_compare_signal.name.c_str(),CV_LOAD_IMAGE_GRAYSCALE);
		equalizeHist(aux, aux);
		resize(aux, resize_aux,RESIZE_XY_SIZE);
		resize_aux.copyTo(aux_compare_signal.ref);
		compare_signals.push_back(aux_compare_signal);
	}
	fp.close();
	for(unsigned int num_signals = 0; num_signals<compare_signals.size();num_signals++){
		//GET COMPARE IMAGE KEYPOINTS AND DESCRIPTORS
		blur(compare_signals[num_signals].ref,compare_signals[num_signals].src_gray, Size(3,3) );
		detector->detect(compare_signals[num_signals].src_gray,compare_signals[num_signals].keypoints);
		extractor->compute(compare_signals[num_signals].src_gray,compare_signals[num_signals].keypoints,compare_signals[num_signals].descriptors);
#if(false)//DEBUG == true)
		imshow("descriptors",compare_signals[num_signals].descriptors);
		compare_signals[num_signals].ref.copyTo(testest);
		drawKeypoints(testest,compare_signals[num_signals].keypoints,testest);
		imshow("Keypoints",testest);
#endif
		//waitKey(1);

	}
	while(1){
		//Sharpen image
		Mat image;
		capture.read(src);
		src_puro = src;
		GaussianBlur(src,image,cv::Size(0, 0), 3);
		//imshow("Blur",image);
		addWeighted(src, 1.5, image, -0.8, 0, image); //imagem peso imagem peso
		//imshow("sharpen",image);
		src=image;


		split(src,src_bgr);
		src_filtered[0].src_gray=src_bgr[0];
		src_filtered[1].src_gray=src_bgr[1];
		src_filtered[2].src_gray=src_bgr[2];
		blur(src_filtered[0].src_gray,src_filtered[0].src_gray, Size(3,3) );
		src_filtered[0].filter_name = "CannyB";
		blur(src_filtered[1].src_gray,src_filtered[1].src_gray, Size(3,3) );
		src_filtered[1].filter_name = "CannyG";
		blur(src_filtered[2].src_gray,src_filtered[2].src_gray, Size(3,3) );
		src_filtered[2].filter_name = "CannyR";

		//imshow("GREYSCALEB",src_filtered[0].src_gray);
		//imshow("GREYSCALEG",src_filtered[1].src_gray);
		//imshow("GREYSCALER",src_filtered[2].src_gray);

		threshold(src_filtered[0].src_gray,src_filtered[0].bin, BINARY_THRESHOLD, 255,THRESH_BINARY);
		threshold(src_filtered[1].src_gray,src_filtered[1].bin, BINARY_THRESHOLD, 255,THRESH_BINARY);
		threshold(src_filtered[2].src_gray,src_filtered[2].bin, BINARY_THRESHOLD, 255,THRESH_BINARY);
		Aux[0] = src_filtered[0].bin - (src_filtered[0].bin & src_filtered[1].bin & src_filtered[2].bin);
		Aux[1] = src_filtered[1].bin - (src_filtered[0].bin & src_filtered[1].bin & src_filtered[2].bin);
		Aux[2] = src_filtered[2].bin - (src_filtered[0].bin & src_filtered[1].bin & src_filtered[2].bin);
		src_filtered[0].bin = Aux[0];
		src_filtered[1].bin = Aux[1];
		src_filtered[2].bin = Aux[2];
		//imshow("BINB",Aux[0]);
		//imshow("BING",Aux[1]);
		//imshow("BINR",Aux[2]);
		//waitKey(1);
		char* source_window = "Source";
		namedWindow( source_window, CV_WINDOW_AUTOSIZE );
		imshow( source_window, src );
		createTrackbar( " Canny thresh:", "Source", &thresh, 255);//, find_possible_signals ); //define threshold
		//Aply for each -> find_possible_signals(num_args,Mat *src_grey,const char <window name>)
		find_possible_signals( 1, (void *) &src_filtered[0]);
		find_possible_signals( 1, (void *) &src_filtered[1]);
		find_possible_signals( 1, (void *) &src_filtered[2]);

		Mat test_src,test_gray;
		while(!possible_signals.empty()){
			test_src = possible_signals.back();
			cvtColor(test_src,test_gray,CV_RGB2GRAY);
			//equalizeHist(test_gray, test_gray);
			//			blur(test_gray,test_gray, Size(3,3) );
			detector->detect(test_gray,keypoints);
			extractor->compute(test_gray,keypoints,descriptors);
			best_match=0;
			number_best_match=5;
			for(unsigned int num_signals = 0,matchCount=0; num_signals<compare_signals.size();num_signals++){
				matcher.knnMatch(descriptors,compare_signals[0].descriptors,matches,1);
				matchCount=0;
				for (int n=0; n<matches.size(); ++n) {
					if (matches[n].size() > 0){
						if (matches[n][0].distance > distThreshold){
							matches[n].erase(matches[n].begin());
						}else{
							++matchCount;
						}

					}
				}
				cout << "MATCH COUNT " << matchCount << endl;
				if(matchCount>best_match){
					best_match = matchCount;
					number_best_match = num_signals;
				}
			}
			drawMatches(test_gray, keypoints, compare_signals[number_best_match].ref, compare_signals[number_best_match].keypoints, matches, img_matches );
			imshow("IMAGE MAtCHES",img_matches);
			waitKey(1);

			possible_signals.pop_back();
		}
	}
	destroyAllWindows();
	return 0;
}



/** @function find_possible_signals */
void find_possible_signals(int num, void *argv)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat bin;
	define_raw_test *data = (define_raw_test *) argv;
	RNG rng(12345);

	/// Detect edges using canny
	Canny( data->bin, canny_output, CANNY_THRESHOLD, CANNY_THRESHOLD*2, 3 );

	/// Find contours
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for( int i = 0; i < contours.size(); i++ )
	{ approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
	boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
	}

	/// Draw polygonal contour + bonding rects + circles
	Mat drawing;
	src.copyTo(drawing);

	Mat aux_resize;
	for( int i = 0; i< contours.size(); i++ ){
		if(boundRect[i].area()>BOUNDING_AREA && !((boundRect[i].height > (boundRect[i].width * 1.10)) || (boundRect[i].height < (boundRect[i].width * 0.90)))){
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
			rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
			resize(src_puro(boundRect[i]),aux_resize,RESIZE_XY_SIZE);
			possible_signals.push_back(aux_resize);
		}
	}
	imshow("Drawing",drawing);waitKey(1);
}
