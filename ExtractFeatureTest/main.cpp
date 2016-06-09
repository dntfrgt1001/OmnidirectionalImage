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
#include "Rotate.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "AffineStore.hpp"

int main(int argc, const char * argv[])
{
    const std::string workDir = "/Users/masakazu/Desktop/";
    const std::string imgName1 = "img1.jpg";
    const std::string imgName2 = "img2.jpg";
    const cv::Size frameSize(4000, 2000);
    
    cv::Mat input1, input2;
    cv::Mat img1, img2;
    input1 = cv::imread(workDir + imgName1);
    cv::resize(input1, img1, frameSize);
    input2 = cv::imread(workDir + imgName2);
    cv::resize(input2, img2, frameSize);
    
    Transform transform(frameSize);
    Rotate rotate(frameSize, transform);
    MatchFeaturePoint matchFeature(frameSize);
    
    int divNum = 6;
    ExtractFeaturePoint extractFeature(frameSize, divNum, transform, rotate);
    
    extractFeature.setImage(img1);
    std::vector<cv::KeyPoint> keyPoints1;
    cv::Mat descriptors1;
//    extractFeature.extractFeaturePoint(keyPoints1, descriptors1);
//    extractFeature.extractRoiFeaturePoint(divNum/2, keyPoints1, descriptors1);
    extractFeature.extractRectRoiFeaturePoint(keyPoints1, descriptors1);
    
    extractFeature.setImage(img2);
    std::vector<cv::KeyPoint> keyPoints2;
    cv::Mat descriptors2;
//    extractFeature.extractFeaturePoint(keyPoints2, descriptors2);
//    extractFeature.extractRoiFeaturePoint(divNum/2, keyPoints2, descriptors2);
    extractFeature.extractRectRoiFeaturePoint(keyPoints2, descriptors2);
    
    
    std::vector<cv::DMatch> dMatches;
    matchFeature.match(descriptors1, descriptors2, dMatches);
    
    std::vector<cv::DMatch> dMatchesSub;
    for (int i=0; i<dMatches.size(); i++) {
        if (dMatches[i].distance < 30) {
            dMatchesSub.push_back(dMatches[i]);
        }
    }
    
    cv::Mat drawMatchImg;
    cv::drawMatches
    (img1, keyPoints1, img2, keyPoints2, dMatchesSub, drawMatchImg,
     cv::Scalar::all(-1), cv::Scalar::all(0));
//     cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    
    
    std::vector<cv::Point2f> point2ds1, point2ds2;
    std::vector<cv::Point3f> point3ds1, point3ds2;
    
    for (int i=0; i<dMatchesSub.size(); i++) {
        cv::Point2f tmpPoint2d1 = keyPoints1[dMatchesSub[i].queryIdx].pt;
        point2ds1.push_back(tmpPoint2d1);
        cv::Point3f tmpPoint3d1;
        transform.orth2d2orth3d(tmpPoint2d1, tmpPoint3d1);
        point3ds1.push_back(tmpPoint3d1);
        
        cv::Point2f tmpPoint2d2 = keyPoints2[dMatchesSub[i].trainIdx].pt;
        point2ds2.push_back(tmpPoint2d2);
        cv::Point3f tmpPoint3d2;
        transform.orth2d2orth3d(tmpPoint2d2, tmpPoint3d2);
        point3ds2.push_back(tmpPoint3d2);
    }
    
    for (int i=0; i<point2ds1.size(); i++) {
        std::cout << point2ds1[i] << " -> " << point2ds2[i] << ",  diff = " <<
        ((point2ds2[i].x - point2ds1[i].x) > 0?
        (point2ds2[i].x - point2ds1[i].x):
        0)
         //frameSize.width - (point2ds1[i].x - point2ds2[i].x))
        << std::endl;
    }
    /*
    for (int i=0; i<point3ds1.size(); i++) {
        std::cout << point3ds1[i] << " -> " << point3ds2[i] << std::endl;
    }
    */
    
    cv::Mat affineMat;
    std::vector<uchar> inliers;
    
    cv::estimateAffine3D(point3ds1, point3ds2, affineMat, inliers);

    std::cout << affineMat << std::endl;
    
    AffineStore affStore;
    
    cv::Mat rotMat;
    affStore.extractRotMatFromAffineMat(affineMat, rotMat);
   
    std::cout << "rotMat.inv() = " << rotMat.inv() << std::endl;
    
    std::cout << (rotMat.type() == CV_32FC1) << std::endl;
    std::cout << (rotMat.type() == CV_64FC1) << std::endl;
    
    cv::Mat rotImg;
    transform.rotateImgWithRotMat(img2, rotImg, rotMat);
    
    
    
    cv::namedWindow("img1");
    cv::imshow("img1", img1);
    cv::namedWindow("img2");
    cv::imshow("img2", img2);
    cv::namedWindow("mod img2");
    cv::imshow("mod img2", rotImg);
    
//    std::cout << "rotMat = " << rotMat << std::endl;
    
    
    
    cv::Size drawSize(1500, 800);
    cv::Mat drawImg;
    cv::resize(drawMatchImg, drawImg, drawSize);
    
    cv::namedWindow("match");
    cv::imshow("match", drawImg);
    
    cv::waitKey(-1);
    
    return 0;
}
