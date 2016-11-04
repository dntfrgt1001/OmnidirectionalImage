//
//  CalcOpticalFlow.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef CalcOpticalFlow_hpp
#define CalcOpticalFlow_hpp

#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

#include "Perspective.hpp"

class CalcOpticalFlow {
public:
    CalcOpticalFlow
    (const float margin, const Perspective& per,
     const float angRag, const float normRat):
    optflowMask(per.getMask(margin)),
    cosRag(cosf(angRag)), normRat(normRat) {};
    
    // オプティカルフローを求める
    void getOpticalFlow
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss) const;
    
    // 正規化画像座標で半径方向と直交しないものを取り除く
    void remOrthOutlier
    (std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals) const;
    
    // オプティカルフローが半径方向のベクトルと直行するか
    bool isOrthCond
    (const cv::Point2f& forNormal, const cv::Point2f& latNormal) const {
        cv::Point2f rad = (forNormal + latNormal) / 2.0;
        cv::Point2f cir = latNormal - forNormal;
        
        // 半径方向となす角のコサイン
        float cosAng = rad.dot(cir) / (cv::norm(rad)*cv::norm(cir));
        
        // コサインの範囲内か
        if (-cosRag < cosAng && cosAng < cosRag) return true;
        else return false;
    }
    
    // 正規化画像座標で逆回転のものを取り除く
    void remRotOutlier
    (std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals) const;
    
    // オプティカルフローの回転方向が正しいか
    bool isRotDirCond
    (const cv::Point2f& forNormal, const cv::Point2f& latNormal) const {
        cv::Point2f rad = (forNormal + latNormal) / 2.0;
        cv::Point2f cir = latNormal - forNormal;
        
        // 半径ベクトルと円周ベクトルの外積
        float croPro = rad.cross(cir);
        
        // 正面で正方向，背面で負方向ならOK
        if (croPro > 0) return true;
        else return false;
    }
    
    // ノルムが大きいものを取り除く
    void remNormOutlier
    (std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals, const float rotAng) const;
    
    // オプティカルフローの長さが正しいか
    bool isNormCond
    (const cv::Point2f& forNormal, const cv::Point2f& latNormal,
     const float rotAng) const {
        cv::Point2f rad = (forNormal + latNormal) / 2.0;
        cv::Point2f cir = latNormal - forNormal;
        
        // 正規化画像と３次元空間の半径を計算
        const float normalRad = cv::norm(rad);
        const float sphereRad = normalRad / sqrtf(1+normalRad*normalRad);
        
        // 理想的な移動量
        const float ideNorm = 2.0 * sphereRad * sinf(rotAng/2.0);
        const float realNorm = cv::norm(cir) * (sphereRad/normalRad);
        
        std::cout << "ratio = " << realNorm / ideNorm << std::endl;
        
        if (realNorm / ideNorm < normRat) return true;
        else return false;
    }
    
    // オプティカルフロー描画
    void drawOpticalFlow
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& forPerss,
     const std::vector<cv::Point2f>& latPerss, cv::Mat& drawImg) const;
    

private:
    const cv::Mat optflowMask;
    const float cosRag;
    const float normRat;
};

#endif /* CalcOpticalFlow_hpp */
