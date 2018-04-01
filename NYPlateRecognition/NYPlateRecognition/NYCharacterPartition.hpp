//
//  NYCharacterPartition.hpp
//  NYPlateRecognition
//
//  Created by NathanYu on 28/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#ifndef NYCharacterPartition_hpp
#define NYCharacterPartition_hpp

#include <stdio.h>
#include "NYPlateDetect.hpp"


class NYCharacterPartition {
    
public:
    
    float kPlateMaxSymbolCount = 7.5;   // 车牌字符最大个数
    int kSymbolIndex = 2;
    int m_theMatWidth = 136;    // 车牌Mat宽度
    
    
    
    // 划分车牌上的字符
    vector<Mat> divideCharacters(NYPlate &plate);
    
private:
    // 清除车牌上的柳钉
    bool clearLiuDing(Mat &src);
    
    // 判断车牌字符尺寸
    bool verifyCharSizes(Mat src);
        
    // 找出指示城市字符Rect的位置
    int getSpecificRect(vector<Rect>& rectVec);
    
    // 获取中文字符Rect的位置
    Rect getChineseRect(Rect rect);
    
    // 调整字符Vector
    void rebuildRectVec(const vector<Rect>& rectVec, vector<Rect>& outRect, int specIndex);
    
    // 将字符Mat调整为标准尺寸
    Mat resizeToNormalCharSize(Mat char_mat);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
};

#endif /* NYCharacterPartition_hpp */
