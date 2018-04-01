//
//  NYPlateDetect.hpp
//  NYPlateRecognition
//
//  Created by NathanYu on 24/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#ifndef NYPlateDetect_hpp
#define NYPlateDetect_hpp

#include <stdio.h>
#include "NYPlate.hpp"
#include "NYPlateJudge.hpp"
#include "NYPlateLocate.hpp"

class NYPlateDetect {
    
public:
    
    // 检测给定图片上的车牌 (综合sobel与颜色识别)
    vector<NYPlate> detectPlates(Mat src);
    
private:
    
    // 去除重复的车牌
    vector<NYPlate> deleteRepeatPlates(vector<NYPlate> colorVec, vector<NYPlate> sobelVec);
    
    // 将车牌输出到指定目录中
    void outputPlates(vector<NYPlate> platesVec);
};


#endif /* NYPlateDetect_hpp */
