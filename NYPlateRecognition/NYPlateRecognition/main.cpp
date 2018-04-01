//
//  main.cpp
//  NYPlateRecognition
//
//  Created by NathanYu on 22/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#include <iostream>
#include "NYAutoRecognize.hpp"

#include "NYCharacterJudge.hpp"


int main(int argc, const char * argv[]) {
  
    NYCharacterJudge judge;
    
    
    // 特征测试
//    string imgPath = "/Users/NathanYu/Desktop/ann/3/4-1.jpg";
//    Mat src = imread(imgPath);
//    cvtColor(src, src, CV_BGR2GRAY);
//    threshold(src, src, 0, 255, THRESH_OTSU+THRESH_BINARY);

//    cout << src << endl;
//
//    judge.test(src);
    
    // 特征训练
//    judge.trainALL();
    
    
    // 车牌识别测试
    string imgPath = "/Users/NathanYu/Desktop/Cars/1.jpg";
    Mat img = imread(imgPath);
    
    namedWindow("111");
    imshow("111", img);
    waitKey(0);
    
    NYPlateDetect detecter;
    detecter.detectPlates(img);
    
//    NYAutoRecognize re;
//    re.recognizePlateNumber(img);

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    
    

    
    return 0;
}
