#ifdef AIDATT_USE_DD4HEP // only use when not the DD4hep version is available
#include "DDSurfaces/Vector2D.h"

namespace aidaTT
{
    typedef DDSurfaces::Vector2D Vector2D;
}
#else
#ifndef VECTOR2D_HH
#define VECTOR2D_HH

#include <ostream>
namespace aidaTT
{
    /** Helper class for 2d vectors */
    class Vector2D
    {
        public:
            Vector2D() : _u(0.), _v(0.) {}
            Vector2D(double u, double v) : _u(u), _v(v) {}

            double operator[](unsigned i) const
            {
                return i == 0 ? _u : _v ;
            }

            double u() const
            {
                return _u ;
            }

            double v() const
            {
                return _v ;
            }

            double& u()
            {
                return _u;
            }
            double& v()
            {
                return _v;
            }

        private:
            double _u, _v ;

    };

    // streaming
    inline std::ostream & operator << (std::ostream & os, const Vector2D &v)
    {
        os << "( " << v[0] << ", " << v[1] << " )" ;
        return os ;
    }

}
# endif // VECTOR2D_HH
# endif // AIDATT_USE_DD4HEP
