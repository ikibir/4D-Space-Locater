#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

Mat Match(Mat source, Mat target) {
	/// Create the result matrix
	Mat result(source.rows, source.cols, CV_32FC1);
	int result_cols =  source.cols;
	int result_rows = source.rows;
	result.create(result_rows, result_cols, CV_32FC1 );

    // Tm Coeff Normed Method
    matchTemplate(source, target, result, 5);
    //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    return result;
}

int Draw_Match(Mat image, Mat target_image, Mat result, double threshold = 0.7) {
    double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

	// Get The Best Score
	if (maxVal>threshold){
		matchLoc = maxLoc;
		rectangle( image, Point(matchLoc.x, matchLoc.y), Point( matchLoc.x + target_image.cols , matchLoc.y + target_image.rows), Scalar(0,255,0), 2);
		return 1;
	}
	return 0;
}

// Rotates image and returns rotated image
Mat rotate(Mat& image, double angle) {
	int fix_angle = 90;
	int i_angle = (int) angle;
    Point2f src_center(image.cols/2.0F, image.rows/2.0F);

    Mat rot_matrix = getRotationMatrix2D(src_center, angle, 1.0);

    Mat rotated_img(Size(image.size().height, image.size().width), image.type());
    //Mat1f m(image.rows+(i_angle%fix_angle)*5, image.cols+(i_angle%fix_angle)*5); 
    warpAffine(image, rotated_img, rot_matrix, image.size());
    return rotated_img;
}

int main(int argc, char **argv){
	// Checking Arguments
	if (argc < 3) {
		cerr<<"Argument Error! \nTry using like: ./app SourceImage.png SubImage.png"<<endl;
		exit(-1);
	}

	Mat image, target_image, img_display, temp, result;
	double temp_double; Point location; Point temp_location;

    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);	
    target_image = imread(argv[2], CV_LOAD_IMAGE_COLOR);
	image.copyTo(img_display);
	target_image.copyTo(temp);

	// Try for every 15 angle from 0 to 360 
	for (int angle = 0; angle<360; angle+=15){
		temp = rotate(target_image, (double) angle);
		// matches images using Template Mathing
		result = Match(image, temp);
		
		// if result is bigger then threshold value draws corresponding rectangle in image and returns 1 else 0
		int draw_res = Draw_Match(img_display, temp, result);
		if (draw_res == 1) {
			// Calculating sub image locations
			minMaxLoc( result, &temp_double, &temp_double, &temp_location, &location, Mat() );

			cout<<"Sub Image Found in Big Image"<<endl;
			cout<<"Positons\n"<< "X1:"<<location.x << " Y1:"<<location.y << endl;
			cout<<"X2:"<<location.x+target_image.rows << " Y2:"<<location.y+target_image.cols << endl;
			cout<<"Image Saved as Result.jpg"<<endl;
	    	imwrite("Result.jpg", img_display);
	    	break;
		}
	}

    return 0;
}