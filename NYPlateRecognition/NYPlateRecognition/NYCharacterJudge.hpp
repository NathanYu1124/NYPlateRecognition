//
//  NYCharacterJudge.hpp
//  NYPlateRecognition
//
//  Created by NathanYu on 29/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#ifndef NYCharacterJudge_hpp
#define NYCharacterJudge_hpp

#include <stdio.h>
#include "NYCharacterPartition.hpp"
#include "Utils.hpp"

using namespace Utils;


class NYCharacterJudge {
   
public:
    
    // 训练ANN模型,字母，数字
    void trainANN();
    
    // 训练ANN模型,汉字
    void trainANN_ZH();
    
    // 训练所有ann模型
    void trainALL();
    
    // 识别车牌字符
    string classifyChars(NYPlate &plate);
    
    // ann 识别测试
    void test(Mat charMat);
    
    

    
private:
    
    string kChars[65] = {
        "0", "1", "2",
        "3", "4", "5",
        "6", "7", "8",
        "9",
        /*  10  */
        "A", "B", "C",
        "D", "E", "F",
        "G", "H", /* {"I", "I"} */
        "J", "K", "L",
        "M", "N", /* {"O", "O"} */
        "P", "Q", "R",
        "S", "T", "U",
        "V", "W", "X",
        "Y", "Z",
        /*  24  */
        "zh_cuan" , "zh_e"    , "zh_gan"  ,
        "zh_gan1" , "zh_gui"  , "zh_gui1" ,
        "zh_hei"  , "zh_hu"   , "zh_ji"   ,
        "zh_jin"  , "zh_jing" , "zh_jl"   ,
        "zh_liao" , "zh_lu"   , "zh_meng" ,
        "zh_min"  , "zh_ning" , "zh_qing" ,
        "zh_qiong", "zh_shan" , "zh_su"   ,
        "zh_sx"   , "zh_wan"  , "zh_xiang",
        "zh_xin"  , "zh_yu"   , "zh_yu1"  ,
        "zh_yue"  , "zh_yun"  , "zh_zang" ,
        "zh_zhe"
        /*  31  */
    };
    
    string zhChars[31] = {
        "zh_cuan" , "zh_e"    , "zh_gan"  ,
        "zh_gan1" , "zh_gui"  , "zh_gui1" ,
        "zh_hei"  , "zh_hu"   , "zh_ji"   ,
        "zh_jin"  , "zh_jing" , "zh_jl"   ,
        "zh_liao" , "zh_lu"   , "zh_meng" ,
        "zh_min"  , "zh_ning" , "zh_qing" ,
        "zh_qiong", "zh_shan" , "zh_su"   ,
        "zh_sx"   , "zh_wan"  , "zh_xiang",
        "zh_xin"  , "zh_yu"   , "zh_yu1"  ,
        "zh_yue"  , "zh_yun"  , "zh_zang" ,
        "zh_zhe"
    };
    
    string ZH[31] = {
        "川", "鄂", "赣",
        "甘", "贵", "桂",
        "黑", "沪", "冀",
        "津", "京", "吉",
        "辽", "鲁", "蒙",
        "闽", "宁", "青",
        "琼", "陕", "苏",
        "晋", "皖", "湘",
        "新", "豫", "渝",
        "粤", "云", "藏",
        "浙"
    };

    const int kCharsTotalNumber = 65;   // 车牌上所有可能出现的字符总数
    const int KChineseNumber = 31;      // 车牌上可能出现的中文字符总数
    const int kCharactersNumber = 34;   // 车牌上可能出现的字母，数字总数
    
    const int kAnnInput = 56;      // ann 输入节点个数 特征维数
    const int kNeurons = 40;        // 隐藏层 神经元个数
    
    Ptr<ml::ANN_MLP> ann_;          //  字母，数字ANN
    Ptr<ml::ANN_MLP> ann_zh;        //  汉字ANN
    
    
    // 加载字符训练数据
    void getCharsData(Mat &trainMat, Mat &trainLabels);
    
    // 加载汉字训练数据
    void getCharsZHData(Mat &trainMat, Mat &trainLabels);
    
    // 对车牌字符进行分类
    void classify(vector<NYCharacter>& charsVec);
    
    // 获取字符特征
    Mat charFeatures(Mat in, int sizeData);
    
    // 获取累计直方图特征
    Mat projectHistogram(Mat img, int model, int threshold = 0);
    
    // 计算Mat中行/列向量中大于阀值的个数
    float countBigValue(Mat &img, int iValue);
    
    // 
    void calPerFeature(Mat src, vector<float> &feats);
    


    
    
    
};

#endif /* NYCharacterJudge_hpp */
