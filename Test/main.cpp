//
//  main.cpp
//  Test
//
//  Created by masakazu nakazawa on 2015/12/31.
//  Copyright © 2015年 masakazu. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, const char * argv[])
{
    std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/";
    std::string inputName = "R0010050.JPG";
    
    /*
    const cv::Size frameSize(1001, 500);
    
    cv::Mat input, img;
    
    input = cv::imread(workDir + inputName);
    cv::resize(input, img, frameSize);
    */
    
    
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) return -1;
    
    int frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    cv::Size frameSize(frameWidth, frameHeight);
    int frameRate = capture.get(CV_CAP_PROP_FPS);
    
    std::cout << frameWidth << std::endl;
    std::cout << frameHeight << std::endl;
    std::cout << frameRate << std::endl;
    
    std::string outputName = "test.avi";
    std::string fileName = workDir + outputName;
    int codec = CV_FOURCC('m', 'p', '4', 'v');
    cv::VideoWriter writer(fileName, codec, 30, frameSize);
    if (!writer.isOpened()) return -1;
        
    cv::namedWindow("live", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
    cv::Mat img(frameSize, CV_8UC3);
    while (true) {
        capture >> img;
        
        writer << img;
        
        cv::imshow("live", img);
        
        if (cv::waitKey(10) > 0) break;
    }
    
    return 0;
}
