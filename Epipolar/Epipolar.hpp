//
//  Epipolar.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Epipolar_hpp
#define Epipolar_hpp

#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"
#include "Range.hpp"
#include "Rotation.hpp"

class Epipolar {
public:
    Epipolar(const int numThre): numThre(numThre) {};
    
    // 8点アルゴリズムで回転行列を推定する
    cv::Mat getRotMatEssMat
    (const std::vector<Normal>& forNormals,
     const std::vector<Normal>& latNormals, cv::Mat& mask) const;
    
    cv::Mat getRotMatEssMat2
    (const std::vector<Normal>& forNormals,
     const std::vector<Normal>& latNormals,
     const std::vector<Normal>& fn1, const std::vector<Normal>& ln1,
     const std::vector<Normal>& fn2, const std::vector<Normal>& ln2,
     const std::vector<Normal>& fn3, const std::vector<Normal>& ln3,
     const std::vector<Normal>& fn4, const std::vector<Normal>& ln4) const;
     
    // マスクから重みを計算する
    float getWeight(const cv::Mat& mask) const;
    
    // 最も重みの大きいインデックスを返す
    int getMaxWeightIndex(const std::vector<float>& weights) const;
    
private:
    const int numThre;
};

#endif /* Epipolar_hpp */
