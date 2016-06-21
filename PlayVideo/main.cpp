//
//  main.cpp
//  PlayVideo
//
//  Created by masakazu nakazawa on 2016/02/16.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "VideoReader.hpp"
#include "VideoWriter.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/phi/";
    const cv::Size frameSize(1920, 960);
    const std::string inputName = "phi.mp4";
//    const std::string outputName = "out.mov";
    const std::string outputPath = "/Users/masakazu/Desktop/phi/";
    const std::string outputPrefix = "phi";
    
    VideoReader videoReader(frameSize, path + inputName);
//    VideoWriter videoWriter(frameSize, path + outputName);
    
    cv::namedWindow("img");
    
       int i=0;
    while (videoReader.hasNext()) {
        cv::Mat img;
        
        videoReader.readImg(img);
        
//        videoWriter.writeImg(img);
        
        //cv::imwrite(outputPath + outputPrefix + std::to_string(i) + ".jpg", img);
        
        cv::imshow("img", img);
        
        cv::waitKey(-1);
        
        i++;

        
        std::cout << i << std::endl;
    }
    
    return 0;
    
    /*
    const std::string workDir = "/Users/masakazu/Desktop/working/";
    const cv::Size frameSize(640, 320);
    const std::string videoName = "out11.mp4";
    const std::string videoName2 = "phi.mp4";
    
    
    
    cv::VideoCapture capture(workDir + videoName);
    if (!capture.isOpened()) return -1;
    
    cv::VideoCapture capture2(workDir + videoName2);
    if (!capture2.isOpened()) return -1;
    
    cv::namedWindow("Current Image");
    cv::namedWindow("Original Image");
    
    int i = 0;
    while (true) {
        cv::Mat input;
        cv::Mat curImg(frameSize, CV_8UC3);
        cv::Mat input2;
        cv::Mat curImg2(frameSize, CV_8UC3);
        
        capture >> input;
        if (input.empty()) break;
        cv::resize(input, curImg, frameSize);
        
        capture2 >> input2;
        if (input2.empty()) break;
        cv::resize(input2, curImg2, frameSize);
        
        cv::imshow("Current Image", curImg);
        cv::imshow("Original Image", curImg2);
        
        std::cout << "frame number = " << i << std::endl;
        
        cv::waitKey(-1);
        
        i++;
    }
    */
     
    return 0;
}
