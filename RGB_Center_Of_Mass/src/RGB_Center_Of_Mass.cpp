#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define BOUNDING_AREA 2500		//Min area to consider for analysis
#define BINARY_THRESHOLD 50		//Binary Threshold for filter binary calculation
#define RESIZE_XY_SIZE Size(50,50)

#define IMAGE_COMPARISON_THRESHOLD 750000.0

#define SMALL_NAME_BYTES	50

using namespace cv;
using namespace std;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

typedef struct _canny_def{
	Mat src_gray;
	Mat bin;
	char filter_name[20];
	vector<Point2i> filter_mc_points;
}define_canny;
define_canny struct_canny[3], *pcanny_struct_canny = struct_canny;

typedef struct _compare_signal{
	Mat ref;
	string name;
}define_compare_signal;
vector<define_compare_signal> compare_signals;

vector<Mat> possible_signals;

///Point2i coord = struct_canny[0].filter_mc_points.front();
Mat src;
Mat src_bgr[3];
Mat test_gray;
int thresh = 60;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void find_possible_signals(int, void* argv);
//void test_compare_for_signal(void*arg);

/** @function main */
int main( int argc, char** argv )
{
	//Get Signals and Resize
	const char *lista_sinais = "Signs/Signals.txt";	//caminho que contem todos os sinais para contra-teste
	fstream fp;
	fp.open(lista_sinais,std::fstream::in);
	Mat aux, resize_aux;
	string name, append = "Signs/";;
	//if(fp.eof) return "error";
	while(!fp.eof()){
		define_compare_signal aux_compare_signal;
		fp >> name;
		name = (append + name);
		aux_compare_signal.name = name;
		aux = imread(aux_compare_signal.name.c_str(),CV_LOAD_IMAGE_COLOR);
		resize(aux, resize_aux, RESIZE_XY_SIZE);
		resize_aux.copyTo(aux_compare_signal.ref);
		compare_signals.push_back(aux_compare_signal);
	}
	fp.close();//O PROBLEMA COM AS SETAS E DEPOIS E A CENA DE SUBTRAIR
	/*define_compare_signal cmp_signal;
	while(!compare_signals.empty()){
		cmp_signal = compare_signals.back();
		imshow(cmp_signal.name,cmp_signal.ref);
		compare_signals.pop_back();
	}*/
	waitKey(0);
	VideoCapture capture;

	///open capture object at location zero (default location for webcam)
		//capture.open(0);

	///set height and width of capture frame
		//capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
		//capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	Mat Aux0;
	Mat Aux1;
	Mat Aux2;

	while(1){
		/// Load source image and convert it to gray
		 //src = imread( "Signs/bus A4.png",CV_LOAD_IMAGE_COLOR);
		//capture.read(src);

		///Get image from camera and split(or to get patern signals)
		src = imread("Test_Signals/10.jpg",CV_LOAD_IMAGE_COLOR);
		//src = imread("Test_Signals/garrafa.jpg",CV_LOAD_IMAGE_COLOR); //RESOLUCAO MUITO GRANDE, USE AT UR OWN RISK!!!
		//src = imread("Signs/yellow_left_arrow.png",CV_LOAD_IMAGE_COLOR);
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
	//imshow("B", src_bgr[0]);
	//imshow("G", src_bgr[1]);
	//imshow("R", src_bgr[2]);

	threshold(struct_canny[0].src_gray,struct_canny[0].bin, BINARY_THRESHOLD, 255,THRESH_BINARY);
	threshold(struct_canny[1].src_gray,struct_canny[1].bin, BINARY_THRESHOLD, 255,THRESH_BINARY);
	threshold(struct_canny[2].src_gray,struct_canny[2].bin, BINARY_THRESHOLD, 255,THRESH_BINARY);
	Aux0 = struct_canny[0].bin - (struct_canny[0].bin & struct_canny[1].bin & struct_canny[2].bin);
	Aux1 = struct_canny[1].bin - (struct_canny[0].bin & struct_canny[1].bin & struct_canny[2].bin);
	Aux2 = struct_canny[2].bin - (struct_canny[0].bin & struct_canny[1].bin & struct_canny[2].bin);
	struct_canny[0].bin = Aux0;
	struct_canny[1].bin = Aux1;
	struct_canny[2].bin = Aux2;

	//imshow("BAND", Aux0);
	//imshow("GAND", Aux1);
	//imshow("RAND", Aux2);

		///Aply Canny to each filter RGB (CAN SPLIT INTO THREADS)
		createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh);//, find_possible_signals ); //define threshold
		//Aply for each -> find_possible_signals(num_args,Mat *src_grey,const char <window name>)
		find_possible_signals( 1, (void *) &struct_canny[0]);
		find_possible_signals( 1, (void *) &struct_canny[1]);
		find_possible_signals( 1, (void *) &struct_canny[2]);

		Scalar min_sums;			//vector<double> 4 channels
		Mat test_matrix;							//test matrix to run tests
		Mat aux_test_matrix;						//test matrix to suffeer operations
		define_compare_signal *ppossible_signal;	//pointer to possible signal, with lowest sumed value
		Scalar sum_color;
		while(!possible_signals.empty()){
			test_matrix = possible_signals.back();
			for(unsigned num_tests=0; num_tests<compare_signals.size();num_tests++){
				min_sums = Scalar(DBL_MAX,DBL_MAX,DBL_MAX);				//reset values
				test_matrix.copyTo(aux_test_matrix);					//copy image to new test slate
				aux_test_matrix -= compare_signals[num_tests].ref;		//make diference operation
				sum_color = sum(aux_test_matrix);						//sum total pix value by channel
				imshow("Result",aux_test_matrix);						//print (debug)
				if(sum_color[0]<=min_sums[0] || sum_color[1]<=min_sums[1] || sum_color[2]<=min_sums[2]){//Test if new lowest value
					min_sums = sum_color;
					ppossible_signal = &compare_signals[num_tests];
				}
				else; //image had not so good oods
				cout << sum_color[0] << " " << sum_color[1] << " " << sum_color[2] << endl;
				waitKey(2000);
			}
			if((min_sums[0]+min_sums[1]+min_sums[2]) <= IMAGE_COMPARISON_THRESHOLD){
				//Show text in image to help show and see if its right
				cout << (min_sums[0]+min_sums[1]+min_sums[2]) << endl;
				imshow(ppossible_signal->name,ppossible_signal->ref);
			}
			possible_signals.pop_back();
			waitKey(4000);
		}

		//cout << struct_canny[0].filter_mc_points;
		//Point2i ponto = struct_canny[0].filter_mc_points.front(); //tirar ponto
		//struct_canny[0].filter_mc_points.front()
		waitKey(0);
	}
  return(0);
}

/** @function find_possible_signals */
void find_possible_signals(int num, void *argv)
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  Mat bin;
  define_canny *data = (define_canny *) argv;

  /// Detect edges using canny
  //Canny( data->src_gray, canny_output, thresh, thresh*2, 3 );
  Canny( data->bin, canny_output, thresh, thresh*2, 3 );

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
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat aux_resize;
  for( int i = 0; i< contours.size(); i++ ){
		if(boundRect[i].area()>BOUNDING_AREA){
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
			rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
			cout << boundRect[i] <<endl;
			//strcpy(buf,"Croped ");strcat(buf,data->filter_name);sprintf(num,"%d",i);strcat(buf,num);
			resize(src(boundRect[i]),aux_resize,RESIZE_XY_SIZE);
			possible_signals.push_back(aux_resize);
             //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
		}
     }

  /// Show in a window
  //namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  //strcpy(buf,"Contours");strcat(buf,data->filter_name);
  //imshow(buf, drawing );
}
