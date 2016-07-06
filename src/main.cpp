#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/version.hpp>
#include <boost/random.hpp>
#include <boost/polygon/voronoi.hpp>
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>

#include "Stripple.h"
#include "PointGenerate.h"
#include "StripplingMaker.h"


using namespace std;

double getLength(const Point p1, const Point p2){
	double a = (double)(p1.a - p2.a);
	double b = (double)(p1.b - p2.b);
	return sqrt(a*a + b*b);
}

void drawVoronoi(const vector<Point> points, const voronoi_diagram<double> *vd, cv::Mat *image){
	int width = image->size().width, height = image->size().height;
	// iterate cells
	for (voronoi_diagram<double>::const_cell_iterator it = vd->cells().begin();
			it != vd->cells().end(); ++it) {
		const voronoi_diagram<double>::cell_type& cell = *it;
		const voronoi_diagram<double>::edge_type* edge = cell.incident_edge();
		
		if(it->contains_point()){
			std::size_t index = it->source_index();
			Point p = points[index];
			try{
				do{
					if (edge->is_finite() && edge->is_primary()){
						const voronoi_diagram<double>::vertex_type v0 = *edge->vertex0();
						const voronoi_diagram<double>::vertex_type v1 = *edge->vertex1();
					

						double x1 = (double)v0.x();
						double y1 = (double)v0.y();

						double x2 = (double)v1.x();
						double y2 = (double)v1.y();

						double length = getLength(Point(x1,y1), Point(x2,y2));
						if(length < width){
							cv::line(*image, cv::Point(x1,y1),cv::Point(x2,y2),cv::Scalar(0,0,0),1,CV_AA);
						}
					}
					edge = edge->next();
				}while(edge != cell.incident_edge());
			}catch(boost::exception &e){
				printf("%s\n", &e);
			}

			cv::circle(*image, cv::Point(p.a,p.b), 2, cv::Scalar(0,255,0), -1, CV_AA);
		}
	}
	cv::imwrite("E:\temp\aaa.png", *image);
}

bool contains(const vector<cv::Point2f> ps, const Point p){
	for(int i=0; i<ps.size(); i++){
		if(ps[i].x == (double)p.a && ps[i].y == (double)p.b){
			return true;
		}
	}
	return false;
}

bool isInArea(const int width, const int height, const Point p){
	int x = p.a, y = p.b;
	return x >= 0 && x <= width && y >= 0 && y <= height;
}

/*!
 ì_åQÇéÛÇØéÊÇË,ÇªÇÃ4ã˜Ç©ÇÁÇ»ÇÈcv::RectÇï‘Ç∑
 */
cv::Rect makeRect(const vector<Point> ps){
	int xMin = 1900, xMax = -1900, yMin = 1200, yMax = 1200;
	for(int i=0; i<ps.size(); i++){
		Point p = ps[i];
		if(p.a < xMin){xMin = p.a;}
		if(p.a > xMax){xMax = p.a;}
		if(p.b < yMin){yMin = p.b;}
		if(p.b > yMax){yMax = p.b;}
	}
	return cv::Rect(xMin-1, yMin-1, 10 + xMax - xMin, 10 + yMax - yMin);
}

/*!
 ì¸óÕÇµÇΩì_åQÇÃÉhÉçÉlÅ[éOäpå`ï™äÑÇãÅÇﬂÇÈ
*/
cv::Subdiv2D makeDelaunay(const vector<Point> ps, const int width, const int height){
	cv::Subdiv2D subdiv;
	vector<cv::Point2f> ps2;
	for(int i=0; i<ps.size(); i++){
		if(!contains(ps2, ps[i])){
			ps2.push_back(cv::Point2f(ps[i].a, ps[i].b));
		}
	}

	subdiv.initDelaunay(makeRect(ps));
	subdiv.insert(ps2);

	return subdiv;
}

bool isInArea(const int width, const int height, const cv::Point p){
	int x = p.x, y = p.y;
	return x >= 0 && x <= width && y >= 0 && y <= height;
}

cv::Scalar getColor(const cv::Point p1, const cv::Point p2, const cv::Point p3, 
					const cv::Mat *image){
	cv::Vec3b bgr1 = image->at<cv::Vec3b>(p1.y, p1.x);
	cv::Vec3b bgr2 = image->at<cv::Vec3b>(p2.y, p2.x);
	cv::Vec3b bgr3 = image->at<cv::Vec3b>(p3.y, p3.x);
	cv::Vec3b bgr4 = image->at<cv::Vec3b>((p1.y + p2.y + p3.y) / 3.0, 
											(p1.x + p2.x + p3.x) / 3.0);

	return cv::Scalar((bgr1[0] + bgr2[0] + bgr3[0] + bgr4[0]) / 4.0, 
						(bgr1[1] + bgr2[1] + bgr3[1] + bgr4[1]) / 4.0,
						(bgr1[2] + bgr2[2] + bgr3[2] + bgr4[2]) / 4.0);
}

double myMap(const double v, const double min1, const double max1, const double min2, const double max2){
	double a = (max2-min2)/(max1-min1);
	return a*v + min2;
}

cv::Scalar getColorGrad(const cv::Scalar v, const cv::Scalar min1, const cv::Scalar max1, const cv::Scalar min2, const cv::Scalar max2){
	return cv::Scalar(
			myMap(v[0], min1[0], max1[0], min2[0], max2[0]),
			myMap(v[1], min1[1], max1[1], min2[1], max2[1]),
			myMap(v[2], min1[2], max1[2], min2[2], max2[2])
		);
}

void drawDelaunay(cv::Mat *image, const cv::Subdiv2D subdiv, const cv::Mat *image0){
	std::vector<cv::Vec6f> triangles;
	subdiv.getTriangleList(triangles);
	int width = image->size().width, height = image->size().height;
	cv::Scalar gradMin(26,177,231), gradMax(156,142,3);
	cv::Scalar imMin(0,0,0), imMax(255,255,255);

	for(auto it = triangles.begin(); it != triangles.end(); it++){
		cv::Vec6f &vec = *it;
		cv::Point p1(vec[0], vec[1]);
		cv::Point p2(vec[2], vec[3]);
		cv::Point p3(vec[4], vec[5]);
		if(!isInArea(width, height, p1) || 
				!isInArea(width, height, p2) || 
				!isInArea(width, height, p3)){
			continue;
		}
		cv::Point pts[3];
		pts[0].x = p1.x; pts[0].y = p1.y;
		pts[1].x = p2.x; pts[1].y = p2.y;
		pts[2].x = p3.x; pts[2].y = p3.y;

		cv::Scalar c = getColorGrad(getColor(p1, p2, p3, image0), imMin, imMax, gradMin, gradMax);
		cv::fillConvexPoly(*image, pts, 3, c);
		cv::line(*image, p1, p2, cv::Scalar(0,0,0));
		cv::line(*image, p2, p3, cv::Scalar(0,0,0));
		cv::line(*image, p3, p1, cv::Scalar(0,0,0));
	}
}

void writeCSV(const cv::Subdiv2D subdiv, const cv::Mat *image, const string path){
	ofstream ofs(path);
	std::vector<cv::Vec6f> triangles;
	subdiv.getTriangleList(triangles);
	int width = image->size().width, height = image->size().height;

	ofs << "x1,y1,r1,g1,b1,x2,y2,r2,g2,b2,x3,y3,r3,g3,b3\n";
	for(auto it = triangles.begin(); it != triangles.end(); it++){
		cv::Vec6f &vec = *it;
		cv::Point p1(vec[0], vec[1]);
		cv::Point p2(vec[2], vec[3]);
		cv::Point p3(vec[4], vec[5]);

		if(!isInArea(width, height, p1) || 
				!isInArea(width, height, p2) || 
				!isInArea(width, height, p3)){
			continue;
		}
		cv::Vec3b bgr1 = image->at<cv::Vec3b>(p1.y, p1.x);
		cv::Vec3b bgr2 = image->at<cv::Vec3b>(p2.y, p2.x);
		cv::Vec3b bgr3 = image->at<cv::Vec3b>(p3.y, p3.x);

		ofs << p1.x << "," << p1.y << "," << (int)bgr1[3] << "," << (int)bgr1[2] << "," << (int)bgr1[0] << ",";
		ofs << p2.x << "," << p2.y << "," << (int)bgr2[3] << "," << (int)bgr2[2] << "," << (int)bgr2[0] << ",";
		ofs << p3.x << "," << p3.y << "," << (int)bgr3[3] << "," << (int)bgr3[2] << "," << (int)bgr3[0] << "\n";
	}
}

int main(){
	// generate point by using mersenne twister
	// RGBÇîΩì]Ç≥ÇπÇΩâÊëú
	cv::Mat image = cv::imread("E:\\temp\\top_reverse.png");
	cv::Mat image0 = cv::imread("E:\\temp\\top_reverse.png");
	int width = image.size().width, height = image.size().height;
	cv::Mat whiteImage(cv::Size(width, height), CV_8UC3, cv::Scalar(255, 255, 255));
	
	int n = 50;
	StripplingMaker maker;

	maker.generate(&image, n);
	voronoi_diagram<double> *vd = maker.getVoronoiDiagram();
	vector<Point> points = maker.points;
	vector<Point> points0 = maker.points0;

	//drawVoronoi(points, vd, &whiteImage);
	
	cv::Subdiv2D subdiv = makeDelaunay(points, width, height);
	drawDelaunay(&whiteImage, subdiv, &image0);

	//cv::imwrite("E:\\temp\\delaunay_top_20000_2.png", 
	//	whiteImage);
	writeCSV(subdiv, &image0, "E:\\users\\J0431298\\Theme\\61th\\EB0A\\images\\delaunay_top_50.csv");
	cv::namedWindow("kongo", cv::WINDOW_AUTOSIZE);
	cv::moveWindow("kongo", 10, 10);
	cv::imshow("kongo", whiteImage);
	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}