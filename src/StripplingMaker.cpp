/*!
 画像オブジェクトと生成する点数を受け取り,ボロノイ図を返す
*/
#include "StripplingMaker.h"

StripplingMaker::StripplingMaker(){
	delta = 10.0;
	iMax = 50;
}

StripplingMaker::~StripplingMaker(){
}

void StripplingMaker::generate(const cv::Mat *image, const int n){
	int width = image->size().width;
	int height = image->size().height;
	vector<Point> points2;

	printf("(%d,%d)\n", width, height);
	printf("n=%d\n",n);

	points = genRandomPoint(n, 10, width-10, 10, height-10);
	points0 = copyPoints(points);
	
	int i = 0;
	double distMax = width;
	while(distMax > delta && i < iMax){
		voronoi_diagram<double> vdTemp;

		// generate voronoi diagram
		construct_voronoi(points.begin(), points.end(), &vdTemp);

		// Centroidal Voronoi Diagramを計算する
		points2 = centroidalVoronoiDiagram(image, &vdTemp, points);
	
		// 最大移動距離を求める
		distMax = getMaxMove(points, points2);

		// 点群を更新する
		points = copyPoints(points2);

		printf("i:%d/%d\tdistMax:%.1f\tdelta:%.1f\n", i, iMax, distMax, delta);
		i++;
	}
	construct_voronoi(points.begin(), points.end(), &vd);
	printf("end\n");
}

/*!
 Centroidal Voronoi Diagramを計算する
*/
vector<Point> StripplingMaker::centroidalVoronoiDiagram(const cv::Mat *image, const voronoi_diagram<double> *vd, vector<Point> ps0){
	vector<Point> ps1;
	for(int i=0; i<ps0.size(); i++){
		ps1.push_back(Point(ps0[i].a, ps0[i].b));
	}

	// iterate cells
	for (voronoi_diagram<double>::const_cell_iterator it = vd->cells().begin();
			it != vd->cells().end(); ++it) {
		const voronoi_diagram<double>::cell_type& cell = *it;
		
		if(it->contains_point()){
			std::size_t index = it->source_index();
			Point p = getCentroid(image, ps1, cell, index);
			ps1[index].a = p.a;
			ps1[index].b = p.b;
		}
	}
	return ps1;
}

/*!
 指定されたCellの重心位置を返す
 */
Point StripplingMaker::getCentroid(const cv::Mat *image, const vector<Point> ps, const voronoi_diagram<double>::cell_type& cell, const int i){
	Point p = ps[i];
	double denom = 0.0, numeX = 0.0, numeY = 0.0;

	const voronoi_diagram<double>::edge_type* edge = cell.incident_edge();
	try{
		do{
			if (edge->is_finite() && edge->is_primary()){
				const voronoi_diagram<double>::vertex_type v0 = *edge->vertex0();
				double x0 = (double)v0.x();
				double y0 = (double)v0.y();
				double g = getGrayScale(image, (int)x0, (int)y0);
				denom += 255.0 - g;
				numeX += x0*(255.0 - g);
				numeY += y0*(255.0 - g);
			}
			edge = edge->next();
		}while(edge != cell.incident_edge());
	}catch(boost::exception &e){
		printf("%s\n", &e);
	}

	double g = getGrayScale(image, p.a, p.b);
	denom += 255.0 - g;
	numeX += p.a*(255.0 - g);
	numeY += p.b*(255.0 - g);

	if(denom == 0.0){
		return Point(p.a, p.b);
	}else{
		return Point((int)(numeX / denom), (int)(numeY / denom));
	}
}

double StripplingMaker::getMaxMove(const vector<Point> ps1, const vector<Point> ps2){
	double max = 0, l = 0.0;
	for(int i=0; i<ps1.size(); i++){
		l = length(ps1[i], ps2[i]);
		if(l > max){max = l;}
	}
	return max;
}

double StripplingMaker::getGrayScale(const cv::Mat *image, const int x, const int y){
	int width = image->size().width;
	int height = image->size().height;

	if(!(y > (height - 1) || y < 0 || x > (width - 1) || x < 0)){
		cv::Vec3b bgr = image->at<cv::Vec3b>(y,x);
		return 0.298912 * bgr[2] + 0.58611 * bgr[1] + 0.114478 * bgr[0];
	}else{
		return 255.0;
	}
}

voronoi_diagram<double>* StripplingMaker::getVoronoiDiagram(){
	return &vd;
}

vector<Point> StripplingMaker::copyPoints(const vector<Point> ps0){
	vector<Point> ps;
	for(int i=0; i<ps0.size(); i++){
		ps.push_back(Point(ps0[i].a, ps0[i].b));
	}
	return ps;
}

double StripplingMaker::length(const Point p1, const Point p2){
	double a = (double)(p1.a - p2.a);
	double b = (double)(p1.b - p2.b);
	return sqrt(a*a + b*b);
}