#ifndef _POINT_GENERATE_
#define _POINT_GENERATE_
/**
点をランダムに生成する関数の定義
*/
#include "Stripple.h"
#include <boost/random.hpp>
#include <ctime>
#include <vector>

using namespace std;

/*!
 [min,max]の乱数を生成して返す
*/
int getRandomVal(const int min, const int max);

/*!
 n個の[xMin,xMax],[yMin,yMax]の点をランダムに生成して返す
*/
vector<Point> genRandomPoint(const int n, const int xMin, const int xMax, const int yMin, const int yMax);

#endif