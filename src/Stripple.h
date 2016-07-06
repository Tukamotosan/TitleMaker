#ifndef _STRIPPLE_H_
#define _STRIPPLE_H_
/*!
 StripplingImageを生成する関数の定義
 */
#include <vector>
#include <boost/polygon/voronoi.hpp>

using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::x;
using boost::polygon::y;
using boost::polygon::low;
using boost::polygon::high;

/*!
 構造体の定義
 */
struct Point{
	int a;
	int b;
	Point(int x, int y):a(x), b(y) {}
};
namespace boost {
namespace polygon {

template <>
struct geometry_concept<Point> {
  typedef point_concept type;
};

template <>
struct point_traits<Point> {
  typedef int coordinate_type;

  static inline coordinate_type get(
      const Point& point, orientation_2d orient) {
    return (orient == HORIZONTAL) ? point.a : point.b;
  }
};
}  // polygon
}  // boost


#endif