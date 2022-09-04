#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

/**
* @brief Transform an image from RGB to Grayscaled
* @param frame Original image
* @param grayFrame Pointer to destination (Grayscaled)
*/
void toGray(Mat frame, Mat* grayFrame){
    
    int width = frame.cols;
    int height = frame.rows;
    float r,g,b;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            r = frame.at<Vec3b>(i, j)[2];
            g = frame.at<Vec3b>(i, j)[1];
            b = frame.at<Vec3b>(i, j)[0];
            grayFrame->at<uchar>(i, j) = round((r + g + b) / 3);
        }
    }
}

/**
* @brief Take an image and it smooth it by averaging the value of each pixel with the surrounding one
* @param frame Original grayscaled image
* @param grayFrame Pointer to destination of the smoothed image
* @param kernelSize Dimension of the kernel, which determined with respect of how many pixel the image will be smoothed
*/
void toSmothed(Mat frame, Mat* smoothedFrame, int kernelSize){
    int width = frame.cols;
    int height = frame.rows;
    int count = 0;
    float acc = 0;

    int offset = (kernelSize-1)/2;

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){

            acc = 0;
            count = 0;

            for(int x = i-offset; x <= i + offset; x++) for(int y = j-offset; y <= j + offset; y++)
                // to handle out of frame filter with check that coordinate are positive
                if(x >= 0 && y >= 0 && x < height && y < width){
                    acc += frame.at<uchar>(x,y);
                    count++;
                }

            smoothedFrame->at<uchar>(i,j) = round(acc/count);
        }
    }
}

/**
* @brief Take an image and it smooth it by averaging the value of each pixel with the surrounding one
* @param background Background frame
* @param frame Next processed frame, subsequent to the background
* @param percentageTrehsold Percentage of pixel that needs to differ between the frames to be considered different
*/
int areDifferent(Mat background, Mat frame, float percentageTreshold){
    int width = frame.cols;
    int height = frame.rows;

    int totalPixel = width * height;
    int differentPixelCount = 0;

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if( (background.at<uchar>(i, j) - frame.at<uchar>(i, j)) != 0 )
                differentPixelCount++;            
        }
    }
    
    float percentage = ((float)differentPixelCount/(float)totalPixel); // percentage of different pixel
    return percentage >= percentageTreshold /100; // if the percentage is greater than the treshold they differ
}