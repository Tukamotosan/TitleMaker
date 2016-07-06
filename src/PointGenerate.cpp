/**
点をランダムに生成する関数の定義
*/

#include "PointGenerate.h"

/*!
 [min,max]の乱数を生成して返す
*/
int getRandomVal(const int min, const int max){
	static boost::mt19937 gen(static_cast<unsigned int>(time(0)));
	static boost::uniform_int<> dst(min, max);
	static boost::variate_generator< boost::mt19937&, boost::uniform_int<> > rand(gen, dst);
	int x = rand();
	return x;
}

/*!
 n個の[xMin,xMax],[yMin,yMax]の点をランダムに生成して返す
*/
vector<Point> genRandomPoint(const int n, const int xMin, const int xMax, const int yMin, const int yMax){
	vector<Point> points;
	for(int i=0; i<n; i++){
		boost::int64_t x = getRandomVal(xMin, xMax), y = getRandomVal(yMin, yMax);
		points.push_back(Point(x,y));
	}
	return points;
}