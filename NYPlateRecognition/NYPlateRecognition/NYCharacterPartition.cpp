//
//  NYCharacterPartition.cpp
//  NYPlateRecognition
//
//  Created by NathanYu on 28/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#include "NYCharacterPartition.hpp"


#define CHAR_NORMAL_WIDTH   20    // 标准字符宽度
#define CHAR_NORMAL_HEIGHT  20   // 标准字符高度


// 划分车牌上的字符
vector<Mat> NYCharacterPartition::divideCharacters(NYPlate &plate)
{
    vector<Mat> charsVec;
    
    // 灰度化 颜色判断 二值化 取轮廓 找外接矩形 截取图块
    Mat src = plate.getPlateMat();
    
    // 灰度化
    Mat src_gray;
    cvtColor(src, src_gray, CV_BGR2GRAY);
    
    // 二值化
    Mat src_threshold;
    Color p_color = plate.getPlateColor();
    
    if (p_color == YELLOW) {  // 黄色车牌二值化参数 (THRESH_BINARY_INV)
        threshold(src_gray, src_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
        cout << "The color of plate is :" << "YELLOW" << endl;
    } else if(p_color == BLUE) {   // 蓝色车牌二值化参数 (THRESH_BINARY)
        threshold(src_gray, src_threshold, 0, 255, THRESH_BINARY + THRESH_OTSU);
        cout << "The color of plate is :" << "BLUE" << endl;
    } else {    // 其他颜色车牌 (暂默认为蓝色)
        threshold(src_gray, src_threshold, 0, 255, THRESH_BINARY + THRESH_OTSU);
        cout << "The color of plate is :" << "UNKNOWN!!!" << endl;
    }
    
    // 去除柳钉
    clearLiuDing(src_threshold);
  
    // 取轮廓
    Mat src_contours;
    src_threshold.copyTo(src_contours);
    vector<vector<Point>> contours;
    findContours(src_contours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    // 取字符外接矩形
    vector<vector<Point>>::iterator itr = contours.begin();
    vector<Rect> rectVec;
    
    while (itr != contours.end()) {
        Rect mr = boundingRect(*itr);
        Mat auxRoi(src_threshold, mr);
        
        if (verifyCharSizes(auxRoi)) {
            rectVec.push_back(mr);
        }
        itr++;
    }
    
    if (rectVec.size() != 0) {
        
        // 按x位置从左到右排序
        vector<Rect> sortedRect(rectVec);
        sort(sortedRect.begin(), sortedRect.end(), [](const Rect& r1, const Rect& r2) { return r1.x < r2.x;});
        
        // 找出城市后首字符的位置
        int specIndex = 0;
        specIndex = getSpecificRect(sortedRect);
        
        // 找出中文字符Rect位置
        Rect chineseRect;
        if (specIndex < sortedRect.size()) {
            chineseRect = getChineseRect(sortedRect[specIndex]);
        } else{
            cout << " Character Partition: specific index is wrong!!!" << endl;
        }
        
        // 调整字符Vector, 正常车牌字符顺序
        vector<Rect> newSortedRect;
        newSortedRect.push_back(chineseRect);
        rebuildRectVec(sortedRect, newSortedRect, specIndex);
        
#pragma mark - TODO 字符不足7个，直接舍弃
        if (newSortedRect.size() != 0) {
            
            // 存储字符二值化Mat
            for (int i = 0; i < newSortedRect.size(); i++) {
                Rect rect_new = newSortedRect[i];
                
                // 对字符1的宽度进行调整
                if (rect_new.width < 6) {
                    int x = rect_new.x - 4;
                    int y = rect_new.y;
                    int width = rect_new.width + 10;
                    int height = rect_new.height;
                    Rect rc = Rect(x,y,width,height);
                    rect_new = rc;
                }
                
                // 灰度图
                Mat char_mat = src_threshold(rect_new);
                
                // 将字符调整为标准尺寸
                Mat char_normal = resizeToNormalCharSize(char_mat);
                
                // 修改车牌信息
                NYCharacter plate_char = NYCharacter();
                plate_char.setCharacterMat(char_normal);
                bool isChinese = (i == 0);
                plate_char.setIsChinese(isChinese);
                plate.addPlateChar(plate_char);

                charsVec.push_back(char_normal);
        
            }
        } else { cout << " Character Partition: rebuilded character vector is empty!!! " << endl; }
    } else { cout << " Character Partition: character vector is empty!!! " << endl; }
    
    // 灰度字符Mat vec
    return charsVec;
}

// 去除车牌上的柳钉(根据每行的跳变次数)
bool NYCharacterPartition::clearLiuDing(Mat &src)
{
    vector<float> fJump;
    // 每行白色像素个数（柳钉和字符）
    int whiteCount = 0;
    const int x = 7;
    // 列向量
    Mat jump = Mat::zeros(1, src.rows, CV_32F);
    
    // 遍历Mat像素
    for (int i = 0; i < src.rows; i++) {
        int jumpCount = 0;
        for (int j = 0; j < src.cols; j++) {
            if (src.at<char>(i,j) != src.at<char>(i,j+1)) { // 行相邻像素跳变(0 -> 255  255 -> 0)
                jumpCount++;
            }
            if (src.at<uchar>(i,j) == 255) {   // 白色像素
                whiteCount++;
            }
        }
        // 记录每行跳变数
        jump.at<float>(i) = (float)jumpCount;
    }
    
    int iCount = 0;     // 字符行数
    for (int i = 0; i < src.rows; i++) {
        fJump.push_back(jump.at<float>(i));
        if (jump.at<float>(i) >= 16 && jump.at<float>(i) <= 45) {
            // 车牌字符满足一定跳变条件
            iCount++;
        }
    }

    // 字符行数不符合正常车牌特征
    if (iCount * 1.0 / src.rows <= 0.4) {
        return false;
    }
    
    // 白色像素不符合正常车牌特征
    if (whiteCount * 1.0 / (src.rows * src.cols) < 0.15 || whiteCount * 1.0 / (src.rows * src.cols) > 0.5) {
        return false;
    }
    
    // 将柳钉所在行的像素全变0
    for (int i = 0; i < src.rows; i++) {
        if (jump.at<float>(i) <= x) {
            for (int j = 0; j < src.cols; j++) {
                src.at<char>(i,j) = 0;
            }
        }
    }
    return true;
}

// 判断车牌字符尺寸(Mat)
bool NYCharacterPartition::verifyCharSizes(Mat src)
{
    // 字符尺寸 45x90
    float aspect = 45.0 / 90.0;
    float charAspect = (float)src.cols / (float)src.rows;   // 宽高比
    float error = 0.7;
    float minHeight = 10.0;
    float maxHeight = 35.0;
    
    float minAspect = 0.05;
    float maxAspect = aspect + aspect * error;
    
    int area = countNonZero(src);       // 非零像素个数(白色字符像素)
    int bbArea = src.cols * src.rows;   // 矩形像素总个数
    
    int percPixels = area / bbArea;
    if (percPixels <= 1 && charAspect > minAspect && charAspect < maxAspect && src.rows >= minHeight && src.rows < maxHeight) {
        // 对字符1的宽度进行调整
        return true;
    } else {
        return false;
    }
}

// 找出指示城市Rect的字符位置
int NYCharacterPartition::getSpecificRect(vector<Rect> &rectVec)
{
    vector<int> x_positions;
    int maxHeight = 0;
    int maxWidth = 0;
    
    for (int i = 0; i < rectVec.size(); i++) {
        x_positions.push_back(rectVec[i].x);
        
        if (rectVec[i].height > maxHeight) {  // 找出矩形vector中最大高度
            maxHeight = rectVec[i].height;
        }
        
        if (rectVec[i].width > maxWidth) {   // 找出矩形vector中最大宽度
            maxWidth = rectVec[i].width;
        }
    }
    
    int specIndex = 0;
    for (int i = 0; i < rectVec.size(); i++) {
        Rect mr = rectVec[i];
        int x_mid = mr.x + mr.width / 2;
        
        // 定位车牌准确时，通常位于车牌宽度的 1/7 与 2/7之间
        if ((mr.width > maxWidth * 0.6 || mr.height > maxHeight * 0.6) && (x_mid <int(m_theMatWidth / kPlateMaxSymbolCount) * kSymbolIndex
             && x_mid > int(m_theMatWidth / kPlateMaxSymbolCount) * (kSymbolIndex - 1))) {
            specIndex = i;
        }
        
    }
    return specIndex;
}

// 获取中文字符Rect的位置
Rect NYCharacterPartition::getChineseRect(Rect rect)
{
    int height = rect.height;
    float newWidth = rect.width * 1.15;
    int x = rect.x;
    int y = rect.y;
    
    int newX = x - int(newWidth * 1.15);
    newX = newX > 0 ? newX : 0;
    
    Rect a(newX, y, int(newWidth), height);
    
    return  a;
}

// 调整字符Vector
void NYCharacterPartition::rebuildRectVec(const vector<Rect> &rectVec, vector<Rect> &outRect, int specIndex)
{
    // 从特殊字符Rect位置开始,依次选择后面6个Rect，多余的舍去(排除两端的缝隙)
    int count = 6;
    
    for (int i = specIndex; i < rectVec.size() && count; ++i, --count) {
        outRect.push_back(rectVec[i]);
    }
}

// 将字符Mat调整为标准尺寸（提取字符类型CV_8UC1, 二值化图像）
Mat NYCharacterPartition::resizeToNormalCharSize(Mat char_mat)
{
    Mat char_resized;
    
    char_resized.create(CHAR_NORMAL_WIDTH, CHAR_NORMAL_HEIGHT, CV_8UC3);
    resize(char_mat, char_resized, char_resized.size(),0,0,INTER_CUBIC);
    
    cout << char_resized << endl;
    cout << endl;
    cout << endl;
    
    return char_resized;
}





























































