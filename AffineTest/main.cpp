//
//  main.cpp
//  AffineTest
//
//  Created by masakazu nakazawa on 2016/06/02.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <Eigen/Core>
#include <Eigen/QR>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Rotation.hpp"

int main(int argc, const char * argv[])
{
    Eigen::Vector3f v = Eigen::Vector3f::Identity();
    
    Eigen::Matrix3d e; e << 2,0,0,0,3,0,0,0,1;
//    Eigen::Matrix3d t; t << 1,0,100,0,1,200,0,0,1;
    Eigen::Matrix3d t; t << 1,0,0,0,1,0,0,0,1;
    Eigen::Matrix3d s; s << 1,1,0,0,1,0,0,0,1;
    const float angle = M_PI / 3.0;
    Eigen::Matrix3d r; r << cos(angle), -sin(angle), 0,
                            sin(angle), cos(angle),  0, 0, 0, 1;
    Eigen::Matrix3d m = e * t * s * r;
    
    std::cout << "R = " << std::endl << e*t*s << std::endl;
    std::cout << "Q = " << std::endl << r << std::endl;
    
    Eigen::HouseholderQR<Eigen::Matrix3d> qr(m);
//    Eigen::FullPivHouseholderQR<Eigen::Matrix3f> qr(m);
//    Eigen::ColPivHouseholderQR<Eigen::Matrix3d> qr(m);
    
    Eigen::Matrix3d R = qr.matrixQR().triangularView<Eigen::Upper>();
    Eigen::Matrix3d Q = qr.householderQ();
//    Eigen::Matrix3d Q = qr.matrixQ();
    
    std::cout << "R = " << std::endl << R << std::endl;
    std::cout << "Q = " << std::endl << Q << std::endl;
    
    
    return 0;
    /*
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
    */
    
}
