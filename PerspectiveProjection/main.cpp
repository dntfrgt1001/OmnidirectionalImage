//
//  main.cpp
//  PerspectiveProjection
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Transform.hpp"
#include "Perspective.hpp"
#include "Rotation.hpp"
#include "CalcOpticalFlow.hpp"
//#include "Range.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/TestImage/";
    const std::string inputName1 = path + "hall.jpg";
    const std::string inputName2 = path + "rothallz12.jpg";
    
    const cv::Size fsOut(1280, 640);
    const cv::Size fsProc(5376, 2688);
    Map::fsOut = fsOut;
    Map::fsProc = fsProc;
    Map::setProcSize();
    
    cv::Mat input1, img1, input2, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, fsProc);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, fsProc);
    
    const int persRad = 200;
    const float rangeAngle = M_PI / 4.0;
    Perspective pers(persRad, rangeAngle);

    const cv::Vec3f rotVec = cv::Vec3f(0.0, 0.0, 1.0);
    cv::Mat froMat = Rotation::getFroChgMat(rotVec);

    cv::Mat persImg1, persImg2;
    const bool isFront = true;
    pers.getPersImg(img1, froMat, isFront, persImg1);
    pers.getPersImg(img2, froMat, isFront, persImg2);
    
    cv::Mat mask = pers.getMask(0.1);
    cv::namedWindow("mask");
    cv::imshow("mask", mask);
    
    cv::namedWindow("pers 1");
    cv::namedWindow("pers 2");
    cv::imshow("pers 1", persImg1);
    cv::imshow("pers 2", persImg2);
    
    cv::waitKey();
    
    const float margin = 0.1;
    const float angRag = M_PI/4.0;
    const float normRat = 2.0;
    
    CalcOpticalFlow optflow(margin, pers, angRag, normRat);
    
    std::vector<Pers> forPerss, latPerss;
    optflow.getOpticalFlow(persImg2, persImg1, forPerss, latPerss);
    
    std::vector<Normal> forNormals, latNormals;
    Map::pers2normal(forPerss, forNormals, pers.getInParaMat());
    Map::pers2normal(latPerss, latNormals, pers.getInParaMat());
    optflow.remOrthOutlier(forNormals, latNormals);
    optflow.remRotOutlier(forNormals, latNormals);
    
    std::vector<Pers> forPerssLast, latPerssLast;
    Map::normal2pers(forNormals, forPerssLast, pers.getInParaMat());
    Map::normal2pers(latNormals, latPerssLast, pers.getInParaMat());
    
    cv::Mat persGrayImg;
    cv::cvtColor(persImg1, persGrayImg, CV_BGR2GRAY);
    cv::Mat persGrayRGBImg;
    cv::cvtColor(persGrayImg, persGrayRGBImg, CV_GRAY2BGR);
    cv::Mat persDrawImg;
    optflow.drawOpticalFlow(persGrayRGBImg, forPerssLast, latPerssLast, persDrawImg);
    
    cv::namedWindow("opt flow");
    cv::imshow("opt flow", persDrawImg);
    
    cv::waitKey();
    
    const std::string persName1 = path + "pers1.png";
    const std::string persName2 = path + "pers2.png";
    const std::string flowName = path + "optcalflow.png";
    cv::imwrite(persName1, persImg1);
    cv::imwrite(persName2, persImg2);
    cv::imwrite(flowName, persDrawImg);
    
    /*
    const float rangeAngle2 = M_PI/3.0;
    Range rg(frameSize, tf, rangeAngle2);
    const int numThre = 10;
    Estimate est(tf, rg, numThre);
    OpticalFlow opf(tf, pers, est);
    std::vector<cv::Point2f> forPerss, latPerss;
    
    
    cv::Mat mask;
    //pers.getMask(mask);
    opf.getFeatures(persImg1, forPerss, mask);
    
    cv::Mat keyImg;
    opf.drawPoint(persImg1, forPerss, keyImg);
    cv::namedWindow("key");
    cv::imshow("key", keyImg);
    
    opf.getOpticalFlow(persImg1, persImg2, forPerss, latPerss);
    
    cv::Mat rawOptImg;
    opf.drawOpticalFlow(persImg1, forPerss, latPerss, rawOptImg);
    
    cv::namedWindow("raw opt");
    cv::imshow("raw opt", rawOptImg);
    
    opf.removeOutlier(forPerss, latPerss);
    
    cv::Mat optImg;
    opf.drawOpticalFlow(persImg1, forPerss, latPerss, optImg);
    
    cv::namedWindow("opt");
    cv::imshow("opt", optImg);
    
//    const std::string outName = "5.jpg";
//    cv::imwrite(path + outName, persImg);
    
    */

    
    return 0;
}
