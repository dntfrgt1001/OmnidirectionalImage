//
//  main.cpp
//  ExtractFeatureTest
//
//  Created by masakazu nakazawa on 2016/05/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Affine.hpp"
#include "Quarternion.hpp"

int main(int argc, const char * argv[])
{
    
    const std::string workDir = "/Users/masakazu/Desktop/";
    const std::string imgName1 = "img1.jpg";
    const std::string imgName2 = "outImg123.jpg";
    const cv::Size frameSize(1960, 960);
    
    cv::Mat input1, input2;
    cv::Mat img1, img2;
    input1 = cv::imread(workDir + imgName1);
    cv::resize(input1, img1, frameSize);
    input2 = cv::imread(workDir + imgName2);
    cv::resize(input2, img2, frameSize);
    
    Transform transform(frameSize);
    int divNum = 6;
    ExtractFeaturePoint(frameSize, transform, divNum);
    MatchFeaturePoint(frameSize, transform);
    Affine affine(transform);
    
    
    float theta = M_PI / 8.0;
    cv::Vec3f axis(1, 2, 1);
    
    Quarternion quart(theta, axis);
    
    cv::Mat trueRotMat;
    Quarternion::Quart2RotMat(quart, trueRotMat);
    
    std::cout << "true rotMat = " << std::endl << trueRotMat << std::endl;
    
//1    transform.rotateImgWithRotMat(img1, img2, trueRotMat);
    
    int divNum = 6;
    ExtractFeaturePoint extractFeature(frameSize, transform, divNum);
    
    std::vector<cv::KeyPoint> keyPoints1;
    cv::Mat descriptors1;
    extractFeature.extractRectRoiFeaturePoint(img1, keyPoints1, descriptors1);
    
    std::vector<cv::KeyPoint> keyPoints2;
    cv::Mat descriptors2;
    extractFeature.extractRectRoiFeaturePoint(img2, keyPoints2, descriptors2);
    
    MatchFeaturePoint matchFeature(frameSize, transform);
    
    std::vector<cv::DMatch> dMatches;
    matchFeature.match(descriptors1, descriptors2, dMatches);
    
    int distanceThreshold = 75;
    matchFeature.filterMatchDistance(dMatches, distanceThreshold);
    float coordThreshold = 0.3;
    matchFeature.filterMatchCoordinate
    (keyPoints1, keyPoints2, dMatches, coordThreshold);
    
    cv::Mat drawMatchImg;
    cv::drawMatches
    (img1, keyPoints1, img2, keyPoints2, dMatches, drawMatchImg,
     cv::Scalar::all(-1), cv::Scalar::all(0));
    

    Affine affine(transform);
    
    std::vector<cv::Point3f> for3DPoints, lat3DPoints;
    
    affine.get3DPointPair
    (keyPoints1, keyPoints2, dMatches, for3DPoints, lat3DPoints);
    
    cv::Mat affMat;
    affine.estimate3DAffineMat(for3DPoints, lat3DPoints, affMat);
    
    cv::Mat rotMat;
    affine.extractRotMatFromAffineMat(affMat, rotMat);
    
    Quarternion::normalizeRotMat(rotMat);

    cv::Mat rotImg;
    transform.rotateImgWithRotMat(img2, rotImg, rotMat);
    
    std::cout << "estimated rotMat = " << std::endl << rotMat.inv() << std::endl;
    
    Quarternion::normalizeRotMat(rotMat);
    std::cout << "normalized estimated rotMat = " << std::endl << rotMat.inv() << std::endl;

    
    
    
    cv::namedWindow("img1");
    cv::imshow("img1", img1);
    cv::namedWindow("img2");
    cv::imshow("img2", img2);
    cv::namedWindow("mod img2");
    cv::imshow("mod img2", rotImg);
    
    
    cv::Size drawSize(1500, 800);
    cv::Mat drawImg;
    cv::resize(drawMatchImg, drawImg, drawSize);
    cv::namedWindow("match");
    cv::imshow("match", drawImg);
    
    cv::waitKey(-1);
    
    return 0;
}
