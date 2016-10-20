//
//  Perspective.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/07/13.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Perspective.hpp"


Perspective::Perspective
(const Transform& tf, const cv::Mat& inParaMat, const float rangeAngle):
tf(tf), inParaMat(inParaMat), rangeRadius(tanf(rangeAngle)),
ellLengHori(inParaMat.at<float>(0,0) * rangeRadius),
ellLengVert(inParaMat.at<float>(1,1) * rangeRadius),
persWidth(inParaMat.at<float>(0,2) + ellLengHori),
persHeight(inParaMat.at<float>(1,2) + ellLengVert)
{
    getMask(mask);
}
/*
void Perspective::persProjImg
(const cv::Mat &img, float rtheta, float rphi, cv::Mat &persedImg)
{
    float dud, dvd;
    calcPersLength(rtheta, rphi, dud, dvd);
    setPersCenter(dud / 2.0, dvd / 2.0);
    
    std::cout << -rtheta/2.0 << " <= " << rtheta/2.0 << std::endl;
    std::cout << -rphi/2.0 << " <= " << rphi/2.0 << std::endl;
    std::cout << "camera matrix = " << cameraMat << std::endl;
    
    int dudInt = (int) floorf(dud);
    int dvdInt = (int) floorf(dvd);

    std::cout << "width = " << dudInt << std::endl;
    std::cout << "height = " << dvdInt << std::endl;
    
    persedImg = cv::Mat(cv::Size(dudInt, dvdInt), CV_8UC3);
    
    for (int ud = 0; ud < dudInt; ud++) {
        for (int vd = 0; vd < dvdInt; vd++) {
            cv::Point2f pers(ud, vd);
            
            //float x, y;
            cv::Point2f regular;
            //pers2regular(ud, vd, x, y);
            pers2regular(pers, regular);
            
            //float theta, phi;
            cv::Point2f psphere;
            //regular2psphere(x, y, theta, phi);
            regular2psphere(regular, psphere);
            
            //float u, v;
            cv::Point2f equirect;
            //transform.psphere2equirect(theta, phi, u, v);
            transform.psphere2equirect(psphere, equirect);
            
            cv::Vec3b dot;
            //transform.getDotBilinear(img, u, v, dot);
            transform.getDotBilinear(img, equirect, dot);
            persedImg.at<cv::Vec3b>(vd, ud) = dot;
            
 
//            if (ud == 0 && vd == 0) {
//                std::cout << "(ud, vd) = (0, 0), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
//            }
//            if (ud == (int) dudInt/2 && vd == (int)dvdInt/2) {
//                std::cout << "(ud, vd) = (rud/2, dvd/2), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
//            }
//            if (ud == dudInt - 1 && vd == dvdInt - 1) {
//                std::cout << "(ud, vd) = (rud/2, dvd/2), " << "(theta, phi) = (" << theta << ", " << phi << ")" << std::endl;
//            }
 
        }
    }
    
    cv::Mat distCoeffs = (cv::Mat_<float>(5, 1) <<
                          -0.2107004, 0.06231665, -0.0008263, -0.00171503, -0.0109820);
    
    cv::Mat tmp = persedImg.clone();
    
   // cv::undistort(tmp, persedImg, cameraMat, distCoeffs);
}
*/

void Perspective::getPersImg
(const cv::Mat &img, cv::Mat &persImg,
 const cv::Mat& froMat, const bool isFront) const
{
    const cv::Mat froMatInv = froMat.inv();
    
    persImg = cv::Mat(persWidth, persHeight, CV_8UC3);
    
    for (int u = 0; u < persWidth; u++) {
        for (int v = 0; v < persHeight; v++) {
            cv::Point2f pers(u, v);
            cv::Point2f normal;
            tf.pers2normal(pers, normal, inParaMat);
            
            float lmr = normal.x*normal.x + normal.y*normal.y -
            rangeRadius*rangeRadius;
            
            if (lmr < 0) {
                cv::Point3f sphere, sphereRot;
                tf.normal2sphere(normal, sphere, isFront);
                tf.rotateSphere(sphere, sphereRot, froMat.inv());
                
                cv::Point2f equirect;
                tf.sphere2equirect(sphereRot, equirect);
                
                cv::Vec3b pixel;
                tf.getBilinearPixel(img, equirect, pixel);
                persImg.at<cv::Vec3b>(v, u) = pixel;
            }
        }
    }

}

void Perspective::getMask(cv::Mat &mask)
{
    mask = cv::Mat::zeros(persWidth, persHeight, CV_8U);
    
    for (int u = 0; u < persWidth; u++) {
        for (int v = 0; v < persHeight; v++) {
            cv::Point2f pers(u, v);
            cv::Point2f normal;
            
            tf.pers2normal(pers, normal, inParaMat);
            
            float lmr = normal.x*normal.x + normal.y*normal.y -
            (rangeRadius-0.1)*(rangeRadius-0.1);
            
            if (lmr < 0) {
                mask.at<uchar>(v, u) = 255;
            }
        }
    }
}
