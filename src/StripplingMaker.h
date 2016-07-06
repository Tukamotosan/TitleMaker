#ifndef _STRIPPLING_MAKER_
#define _STRIPPLING_MAKER_
/*!
 画像オブジェクトと生成する点数を受け取り,ボロノイ図を返す
 関数の定義
*/
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/version.hpp>
#include <boost/random.hpp>
#include <boost/polygon/voronoi.hpp>
#include <vector>
#include <algorithm>    // std::copy
#include <iterator>
#include <limits.h>
#include "Stripple.h"
#include "PointGenerate.h"

using namespace std;
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::x;
using boost::polygon::y;
using boost::polygon::low;
using boost::polygon::high;

/*!
 画像オブジェクトと生成する点数を受け取り,ボロノイ図を返す
*/
class StripplingMaker{
public:
	vector<Point> points0;
	vector<Point> points;
	voronoi_diagram<double> vd;
	double delta;
	int iMax;

	StripplingMaker();
	~StripplingMaker();

	void generate(const cv::Mat *image, const int n);

	voronoi_diagram<double>* getVoronoiDiagram();

private:
	/*!
	 Centroidal Voronoi Diagramを計算する
	*/
	vector<Point> centroidalVoronoiDiagram(const cv::Mat *image, const voronoi_diagram<double> *vd, vector<Point> ps0);

	/*!
	 指定されたCellの重心位置を返す
	 */
	Point getCentroid(const cv::Mat *image, const vector<Point> ps, const voronoi_diagram<double>::cell_type& cell, const int i);

	/*!
	 二つの点群を受け取り最大移動距離を返す
	*/
	double getMaxMove(const vector<Point> ps1, const vector<Point> ps2);

	/*!
	 rgb値を受け取り,グレイスケール値[0,255]に変換して返す
	*/
	double getGrayScale(const cv::Mat *image, const int x, const int y);
	vector<Point> copyPoints(const vector<Point> ps0);
	double length(const Point p1, const Point p2);
};

#endif