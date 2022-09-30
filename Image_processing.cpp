#include <iostream>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


void gammaCorrection(const Mat &src, Mat &dst, const float gamma)
{
    float invGamma = 1 / gamma;

    Mat table(1, 256, CV_8U);
    uchar *p = table.ptr();
    for (int i = 0; i < 256; ++i) {
        p[i] = (uchar) (pow(i / 255.0, invGamma) * 255);
    }

    LUT(src, table, dst);
}

int main() // we can use int main as well just don't forget
// to add return 0 in the end
{
    string path = "/home/meibel/Descargas/convertidorImg-main/untitled/stitch.jpg";
    Mat img = imread(path);

    Mat imgGray, Blur_img, imgBrighnessHigh100, imgBrighnessLow100, gammaImg;;//Defining Output Image matrix
    img.convertTo(imgBrighnessHigh100, -1, 1, 100); //increase the brightness by 100
    img.convertTo(imgBrighnessLow100, -1, 1, -100); //decrease the brightness by 100



    cvtColor(img, imgGray,
             COLOR_BGR2GRAY); // To convert image to
    // grayscale image
    GaussianBlur(img, Blur_img, Size(7, 7), 5,
                 0); // Now finally adding blur to the image
    gammaCorrection(img, gammaImg, 2.2);


    imshow("Original Image", img); // Image before the conversion
    imshow("Image Gray",imgGray); // After Conversion to GrayScale
    imshow("Image Blur", Blur_img); // Blurred Image
    imshow("Image Brighness High", imgBrighnessHigh100); // Brighness High Image
    imshow("Image Brighness Low", imgBrighnessLow100); // Brighness Low Image
    imshow("Image Gamma Correction", gammaImg); // Gamma Correction Image

    waitKey(0); // wait for keystroke
}


