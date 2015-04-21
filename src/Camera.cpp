/*
Copyright (c) by Emil Valkov,
All rights reserved.
License: http://www.opensource.org/licenses/bsd-license.php
Edited and implemented with opencv by faelnpaiva
 */
//#include <cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Camera.h"
#include "RaspiCamCV.h"

using namespace std;
using namespace cv;
void detectAndDisplay( Mat frame );


/** Global variables */
	char  face_cascade_name[100] = "haarcascade_frontalface_alt.xml";
	String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;
	string window_name = "Capture - Face detection";
	RNG rng(12345);



int main(int argc, char *argv[ ]){
	RASPIVID_CONFIG * config = (RASPIVID_CONFIG*)malloc(sizeof(RASPIVID_CONFIG));


	config->width=320;
	config->height=240;
	config->bitrate=0; // zero: leave as default
	config->framerate=0;
	config->monochrome=0;

	/*
Could also use hard coded defaults method: raspiCamCvCreateCameraCapture(0)
	 */
	RaspiCamCvCapture * capture = (RaspiCamCvCapture *) raspiCamCvCreateCameraCapture2(0, config);
	free(config);
	CvFont font;
	double hScale=0.4;
	double vScale=0.4;
	int lineWidth=1;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale, vScale, 0, lineWidth, 8);
	cvNamedWindow("RaspiCamTest", 1);
	int exit =0;

	//-- 1. Load the cascades
		if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
		if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };




	do {
		IplImage* image = raspiCamCvQueryFrame(capture);
		//char text[200];

		Mat img=cvarrToMat(image);  //IMPROTANTE

		/*sprintf(
				text
				, "w=%.0f h=%.0f fps=%.0f bitrate=%.0f monochrome=%.0f"
				, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_FRAME_WIDTH)
				, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_FRAME_HEIGHT)
				, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_FPS)
				, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_BITRATE)
				, raspiCamCvGetCaptureProperty(capture, RPI_CAP_PROP_MONOCHROME)
		);
		cvPutText (image, text, cvPoint(05, 40), &font, cvScalar(255, 255, 0, 0));
		sprintf(text, "Press ESC to exit");
		cvPutText (image, text, cvPoint(05, 80), &font, cvScalar(255, 255, 0, 0));

*/
		//-- 3. Apply the classifier to the frame
		 detectAndDisplay( img );



	//	cvShowImage("RaspiCamTest", image);


		char key = cvWaitKey(0);  //aqui talvez vá o sensor
		switch(key)
		{
		case 27: // Esc to exit
			cvSaveImage("Eye_Test.bmp",image);

			exit = 1;
			break;
		/*case 60: // < (less than)
			raspiCamCvSetCaptureProperty(capture, RPI_CAP_PROP_FPS, 25); // Currently NOOP
			break;
		case 62: // > (greater than)
			raspiCamCvSetCaptureProperty(capture, RPI_CAP_PROP_FPS, 30); // Currently NOOP
			break;*/
		}
	} while (!exit);
	cvDestroyWindow("RaspiCamTest");
	raspiCamCvReleaseCapture(&capture);
	return 0;
}

void detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

		Mat faceROI = frame_gray( faces[i] );
		std::vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		for( size_t j = 0; j < eyes.size(); j++ )
		{
			Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
			int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
			circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
		}
	}
	//-- Show what you got
	imshow( window_name, frame );
}
