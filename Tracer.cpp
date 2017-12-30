#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\core\core.hpp>
#include<queue>
#include<stdlib.h>
#include<conio.h>
#include<vector>
using namespace cv;
using namespace std;
Mat flip(Mat im)
{
	int i, j;
	Mat flip(im.rows, im.cols, CV_8UC3);
	for (i = 0; i < im.rows; i++)
	{
		for (j = 0; j < im.cols; j++)
		{
			flip.at<Vec3b>(i, (im.cols - 1 - j))[2] = im.at<Vec3b>(i, j)[2];
			flip.at<Vec3b>(i, (im.cols - 1 - j))[1] = im.at<Vec3b>(i, j)[1];
			flip.at<Vec3b>(i, (im.cols - 1 - j))[0] = im.at<Vec3b>(i, j)[0];

		}
	}
	return flip;
}

int main(int argc, char** argv)
{
	VideoCapture cap(0); //capture the video from web cam
	Mat frame;
	cap >> frame;
	Mat output(frame.rows, frame.cols, CV_8UC1, Scalar(0));
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	int px = 311, py = 183;
	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		Mat imgHSV;
		Mat imgMirror = flip(imgOriginal); //mirror image of ca[ptured frame
		cvtColor(imgMirror, imgHSV, COLOR_BGR2HSV); //Convert the mirrored captured frame from BGR to HSV

		Mat imgThresholded;
		inRange(imgHSV, Scalar(0,0,253), Scalar(46,8,255), imgThresholded); //Threshold the image
		//imshow("Threshold", imgThresholded);
		//morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image
		int a = px, b = py;
		Moments o = moments(imgThresholded);
		double dm01 = o.m01;
		double dm10 = o.m10;
		double da = o.m00;
		px = dm10 / da;
	    py = dm01 / da;
		Point pt1, pt2;
		pt1.x = a; pt1.y = b;
		pt2.x = px; pt2.y = py;
		if (px > 0 && py > 0)
			line(output, pt1, pt2, Scalar(255));
	    imshow("Path", output);  //show the path traced by the centre of the hand
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
		return 0;
}
