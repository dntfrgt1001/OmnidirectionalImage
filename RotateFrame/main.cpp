//
//  main.cpp
//  RotateFrame
//
//  Created by masakazu nakazawa on 2016/08/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Transform.hpp"
#include "Quarternion.hpp"
#include <libiomp/omp.h>

int main(int argc, const char * argv[])
{
    omp_set_num_threads(2);
#pragma omp parallel
{
    std::cout << "hello" << std::endl;;
}
    
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName = path + "test.jpg";
    const std::string outputName = path + "test_rot.jpg";
    const cv::Size frameSize(5376, 2688);
//    const cv::Size frameSize(2000, 1000);
    
    cv::Mat input, img;
    input = cv::imread(inputName);
    cv::resize(input, img, frameSize);
    
    Transform transform(frameSize);
    
    /*
    float chi = M_PI / 10.0;
    cv::Vec3f axis(1.0, 0.0, 0.0);
    cv::Mat rotMat;
    Quarternion::arbRotMat(chi, axis, rotMat);
    std::cout << rotMat << std::endl;
    cv::Mat tmp1;
    transform.rotateImgWithRotMat(img, tmp1, rotMat);
    cv::namedWindow("tmp1", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("tmp1", tmp1);
    
    float chi2 = M_PI / 4.0;
    cv::Vec3f axis2(0.0, 1.0, 0.0);
    cv::Mat rotMat2;
    Quarternion::arbRotMat(chi2, axis2, rotMat2);
    std::cout << rotMat2 << std::endl;
    cv::Mat tmp2;
    transform.rotateImgWithRotMat(tmp1, tmp2, rotMat2);
    cv::namedWindow("tmp2", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("tmp2", tmp2);
    
    float chi3 = M_PI / 8.0;
    cv::Vec3f axis3(1.0, 0.0, 0.0);
    cv::Mat rotMat3;
    Quarternion::arbRotMat(chi3, axis3, rotMat3);
    std::cout << rotMat3 << std::endl;
    cv::Mat tmp3;
    transform.rotateImgWithRotMat(tmp2, tmp3, rotMat3);
    cv::namedWindow("tmp3", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("tmp3", tmp3);
    
    float chi4 = M_PI;
    cv::Vec3f axis4(0.0, 1.0, 0.0);
    cv::Mat rotMat4;
    Quarternion::arbRotMat(chi4, axis4, rotMat4);
    std::cout << rotMat4 << std::endl;
    cv::Mat tmp4;
    transform.rotateImgWithRotMat(tmp3, tmp4, rotMat4);
    cv::namedWindow("tmp4", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("tmp4", tmp4);

    cv::imwrite(outputName, tmp4);
    */
    /*
    float angle = M_PI / 4.0;
    cv::Vec3f axis(0.0, 0.0, 1.0);
    Quarternion quart(angle, axis);
    std::cout << quart << std::endl;
    cv::Mat rotMat;
    Quarternion::Quart2RotMat(quart, rotMat);
    std::cout << rotMat << std::endl;
    float angle2;
    cv::Vec3f axis2;
    Quarternion::RotMat2AngleAxis(rotMat, angle2, axis2);
    std::cout << angle2 << ", " << axis2 << std::endl;
    */
    
    float angle = M_PI / 4.0;
    cv::Vec3f axis(0.0, 0.0, 1.0);
    cv::Mat rotMat;
    Quarternion::Rodrigues2RotMat(angle, axis, rotMat);
    std::cout << rotMat << std::endl;
    Quarternion quart;
    Quarternion::RotMat2Quart(rotMat, quart);
    std::cout << quart << std::endl;
    
    cv::Mat tmp1;
    
    int start = omp_get_wtime();
    transform.rotateImgWithRotMat(img, tmp1, rotMat);
    int end = omp_get_wtime();
    
    std::cout << "time = " << end - start << std::endl;
    
    cv::namedWindow("tmp1");
    cv::imshow("tmp1", tmp1);

    //cv::imwrite(outputName, tmp1);
    
    cv::waitKey(-1);

}
