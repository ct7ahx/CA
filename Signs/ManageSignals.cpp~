//Autonomous Robot Competition - ManageSignals.cpp
//
//OBJECTIVE
///////////////////////////////////////////////////////////////////////
//This program as the objective of calculating the necessary number
//of lines so that the camera from the Autonumous Conduction car 
//can distinguish between different road signs
//
//THOUGH PROCESS
///////////////////////////////////////////////////////////////////////
//The program loads a list <signs.txt> of the name of the signs that
//are pretended to analyse and computes the required number. Then
//the signal is downsized to a standard format ex <100px,100px>. The 
//lines start as one (from the center of the picture) and are
//incremented (having the center of the picture as point of reference)
//until obtaining the sufficient amount to be able to distinguish all
//the signs provided in the list.
///////////////////////////////////////////////////////////////////////
//
//INPUT: <name of file>
///////////////////////////////////////////////////////////////////////

//Written By Diogo Guerra

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <highgui.h>
#include <cv.h>

#define	FRAME_WIDTH	640
#define	FRAME_HEIGHT	480

#define IMG_NAME	"60.png"

using namespace cv;
using namespace std;

/// Global variables
Mat *signals;

void drawObject(int x, int y,Mat &frame){
	circle(frame,Point(x,y),1,Scalar(0,255,0),2);
}


int main(int argc, char* argv[])
{
	//Variables
	int number=0;
	ifstream fp;
	char signal_name[100];
	int resize_size = 200;

	//start conditioning
	if(argc<2){
		printf("\nERROR: No input file specified\n");
		return -1;}

	//Loading
	fp.open(argv[1],ios::in);
	if (!fp.is_open()){
		cout << "\nERROR: Cound not open specified file" << endl;
		return -1;}
	cout << endl;
	fp.getline(signal_name,100);
	while(!fp.eof()){
		number++;
		cout <<signal_name << endl;
		fp.getline(signal_name,100);
	}
	cout << "*******************************************************" << endl;
	cout << "Signals in file: " << number << endl;
	cout << "*******************************************************" << endl;
	cout << "Processing..." << endl;
	//fp.seekg(ios_base::beg);
	fp.close();fp.open(argv[1],ios::in);
	
	//Load Signals
	signals = new Mat[number];
	for(int n=0;n<number;n++){
		fp.getline(signal_name,100);
		signals[n] = imread(signal_name);
	}
	//Resize all images for processment
	Mat *resized_signals = new Mat[number];
	for(int n=0;n<number;n++){
		int color=0;
		char buf[20];
		sprintf(buf,"Signal %d",n);
		resize(signals[n], resized_signals[n], Size(resize_size, resize_size), 0, 0, INTER_CUBIC);
		//cvtColor(resized_signals[n], resized_signals[n],CV_RGB2GRAY); //convert to grey
		//resized_signals[n].reshape(1,0);	//1 channel reshape

		imshow(buf,resized_signals[n]);
		//cout << resized_signals[n].channels <<endl; getchar();
	}
	////Mat is a 3 Channel Matrix
	//Process signals
	/*H V*
	/*****
	1* * *
	2* * *
	3* * *
	4* * *
	5* * *
	6* * *
	7* * *
	8* * *
	9* * *
	/*****/
	int ***master_table = new int**[number]; //creates vector for the tables of each image
	while(1){
		int hlines=1,vlines=1;
		int color, prev_color=-1;
		for(int i=0;i<number;i++){
			master_table[i] = new int*[hlines];	//creates vector for horizontal and vertical lines line
			for(int h=0;h<hlines;h++){
				master_table[i][h] = new int[2];	//based on representative figure
				for(int v=0;v<vlines;v++)
					master_table[i][h][v] = 0;
			}
		}
		//MAIN ALGORITHM
		for(int i=0;i<number;i++){
			int counts=0;
			//h calculation
			for(int hrow=0;hrow<resize_size;hrow++){
				color=resized_signals[i].at<int>(hrow,resize_size/2);	//point color moving Horizontally
				if(color!=prev_color){
					cout << counts++ << endl;
					prev_color=color;
					drawObject(hrow,resize_size/2,resized_signals[i]);
				}	//save new transition
				cout << resized_signals[i].at<int>(hrow,resize_size/2)<<endl;
			}
			waitKey(0);

			//show tracker
			for(int n=0;n<number;n++){
				char buf[20];
				sprintf(buf,"Signal %d",n);
				imshow(buf,resized_signals[n]);
			}
		}
break; //TODO: DELETE AFTER MAKING V AND H 
		//Clean up memorty to prepare new hipothesis
		for(int i=0;i<number;i++){
			for(int h=0;h<hlines;h++)
				delete [] master_table[i][h];	//deletes H and V pair table from each image
			delete [] master_table [i];		//deletes H identifier
		}	
		//if(waitKey(0))
		//	break;
	}
	
	
		//TODO: height/2^(n++) alternado com vertica comeca com n=1
		//contagem contando as transicoes para fora ou sem contar as transicoes para fora?
		//Ter em atenção que depois na imagem da camara o existe gradiente nas cores do sinal o que trama a coisa...
	
	//Clean up
		delete [] master_table;			//deletes master_table
		fp.close();
		delete [] signals;

  return 0;
 }
