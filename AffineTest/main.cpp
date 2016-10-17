//
//  main.cpp
//  AffineTest
//
//  Created by masakazu nakazawa on 2016/06/02.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Rotation.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/rotation/";
    const std::string imgName1 = path + "img.jpg";
    const std::string imgName2 = path + "rotimg.jpg";
    const std::string outName = path + "match.jpg";
    
    const cv::Size fs(600, 300);

    cv::Mat input1, input2;
    cv::Mat img1, img2;
    
    input1 = cv::imread(imgName1);
    cv::resize(input1, img1, fs);
    input2 = cv::imread(imgName2);
    cv::resize(input2, img2, fs);
    
    for (int u=0; u<fs.width; u++) {
        for (int v=0; v<fs.height; v++) {
            cv::Vec3b tmpVec = img1.at<cv::Vec3b>(v, u);
            uchar R = tmpVec[0];
            uchar G = tmpVec[1];
            uchar B = tmpVec[2];
            
            uchar umax = MAX(MAX(R, G), B);
            uchar umin = MIN(MIN(R, G), B);
            
            uchar gray = umax/2 + umin/2;
            
            img1.at<cv::Vec3b>(v, u) = cv::Vec3b(gray, gray, gray);
        }
    }
    
    for (int u=0; u<fs.width; u++) {
        for (int v=0; v<fs.height; v++) {
            cv::Vec3b tmpVec = img2.at<cv::Vec3b>(v, u);
            uchar R = tmpVec[0];
            uchar G = tmpVec[1];
            uchar B = tmpVec[2];
            
            uchar umax = MAX(MAX(R, G), B);
            uchar umin = MIN(MIN(R, G), B);
            
            uchar gray = umax/2 + umin/2;
            
            img2.at<cv::Vec3b>(v, u) = cv::Vec3b(gray, gray, gray);
        }
    }

    
    const Transform tf(fs);
    
    const int divNum = 6;
    const ExtractFeaturePoint efp(fs, tf, divNum);
    
    const float distThre = 180;
    const float coordThre = 0.7;
    const MatchFeaturePoint mfp(fs, tf, distThre, coordThre);
    
    std::vector<cv::KeyPoint> forKeyPoints, latKeyPoints;
    cv::Mat forDesc, latDesc;
    
    efp.extractFeaturePoint(img1, forKeyPoints, forDesc);
    efp.extractFeaturePoint(img2, latKeyPoints, latDesc);
    
    std::vector<cv::DMatch> dMatchs;
    mfp.match(forDesc, latDesc, dMatchs);
    mfp.filterMatchDistance(dMatchs);
    
    std::vector<cv::Point2f> forEquirects, latEquirects;
    mfp.sortMatchedPair(forKeyPoints, latKeyPoints, dMatchs,
                        forEquirects, latEquirects);
    
    std::vector<cv::Point3f> forSpheres, latSpheres;
    tf.equirect2sphere(forEquirects, forSpheres);
    tf.equirect2sphere(latEquirects, latSpheres);
    mfp.filterCoordDistance(forSpheres, latSpheres);
    
    std::vector<cv::Point2f> forEquirectsLast, latEquirectsLast;
    tf.sphere2equirect(forSpheres, forEquirectsLast);
    tf.sphere2equirect(latSpheres, latEquirectsLast);
    
    cv::Mat matchImg;
    mfp.drawMatchesVertical(img1, forEquirectsLast, img2, latEquirectsLast,
                         matchImg);
    
    cv::namedWindow("match img");
    cv::imshow("match img", matchImg);
    
    cv::waitKey(-1);
    
    cv::imwrite(outName, matchImg);
    
    return 0;
}
