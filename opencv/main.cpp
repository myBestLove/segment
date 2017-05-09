#include <vector>  
#include <sys/time.h>  
#include "opencv2/opencv.hpp"  
#include "segmentimage.h"  
#include <iostream>
  
using namespace std;  
using namespace cv;  
// Computer Vision A Reference Guide  
int main()  
{  
    //const char* imagename = "G:\\Pic\\101087_big.jpg";  
    string imagename = "/home/jason/Desktop/slam/Geometric Reasoning for Single Image Structure Recovery/VP/good/312.jpg";  
    //const char* imagename = "grain.bmp";  
    //const char* imagename = "person_272.png";  
    //从文件中读入图像  
    Mat img = imread(imagename);  
  
    //如果读入图像失败  
    if (img.empty())  
    {  
        cout<<"Can not load image "<<imagename<<endl;;  
        return -1;  
    }  
  
    //显示图像  
    imshow("image", img);  
    //cvtColor(img, img, CV_BGR2Lab);// May be using lab color space will be better  
    Mat gray;  
    cvtColor(img, gray, CV_BGR2GRAY);  
    img.convertTo(img,CV_32FC3);  
  
    float sigma = 0.8;  
    float k = 100;  
    int min_size = 100;  
    int num_ccs;  
    
    struct timeval start, end;
    gettimeofday(&start,NULL);  
    Mat result = segment_image(img, sigma, k, min_size, &num_ccs);  
  
  
    gettimeofday(&end,NULL);
    double time = (end.tv_sec-start.tv_sec)*1000 +(end.tv_usec-start.tv_usec)/1000.0;
    cout << "get " << num_ccs << " components" << endl;  
    cout << "time:"<<time<<" ms"<<endl;
    
    imshow("process result", result);  
      
    cvtColor(gray, gray, CV_GRAY2BGR);  
    imshow("overlay result", gray*0.25 + result*0.75); 
    imwrite("is.pnm",result);
  
    //此函数等待按键，按键盘任意键就返回  
    waitKey(0);  
  
    return 0;  
  
}  