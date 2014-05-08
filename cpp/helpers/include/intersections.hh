#ifndef INTERSECTIONS_HH
#define INTERSECTIONS_HH

#include <utility>
#include <vector>
#include <cmath>

/** simple helper class to calculate the analytical solutions of intersections
* a helix is a circle in the projection perpendicular to the magnetic field
*    and a straight line in the the direction of the magnetic field
*
*  analytical solutions can be calculated for a number of shapes;
*  most often in use are:
*   circle and circle
*   circle and line
*   line and line
**/

namespace aidaTT
{

    class circle
    {
        public:
            circle(double c0, double c1, double r) : _c0(c0), _c1(c1), _r(r) {};
            double radius() const
            {
                return _r;
            }
            double r2() const
            {
                return _r * _r;
            }
            std::pair<double, double> center() const
            {
                return std::make_pair(_c0, _c1);
            };
        private:
            circle();
            double _c0, _c1, _r;
    };



    class straightLine
    {
        public:
            straightLine(double n0, double n1, double d) : _n0(n0), _n1(n1), _d0(d) {};
            double distance() const
            {
                return _d0;
            }
            double d2() const
            {
                return _d0 * _d0;
            }
            std::pair<double, double> normal() const
            {
                return std::make_pair(_n0, _n1);
            }
            double normalSquare() const
            {
                return _n0 * _n0 + _n1 * _n1;
            }
            void move(double d1, double d2)
            {
                _d0 -= _n0 * d1 + _n1 * d2;
            }

        private:
            straightLine();
            double _n0, _n1, _d0;
    };


    class intersections
    {
        public:
            intersections()
            {
                _points.clear();
            };
            unsigned int number() const
            {
                return _points.size();
            }
            void add(double x, double y)
            {
                _points.push_back(std::make_pair(x, y));
            }
            const std::pair<double, double>& operator[](unsigned int) const;

        private:
            std::vector<std::pair<double, double> > _points;
    };

    intersections intersectCircleCircle(const circle&, const circle&);

    intersections intersectCircleStraightLine(const circle&, straightLine);

    intersections intersectStraightLineStraightLine(const straightLine&, const straightLine&);

}
#endif // INTERSECTIONS_HH
