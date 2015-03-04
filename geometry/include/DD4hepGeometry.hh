#ifdef AIDATT_USE_DD4HEP

#ifndef DD4HEPGEOMETRY_HH
#define DD4HEPGEOMETRY_HH

#include "DD4hep/LCDD.h"
#include "DDSurfaces/ISurface.h"

#include "IGeometry.hh"
#include "DD4hepSurface.hh"
#include "DD4hepMaterial.hh"

#include "aidaTT-Units.hh"

namespace aidaTT
{

    class DD4hepCylinder : public ICylinder
    {
        public:
            DD4hepCylinder(const DDSurfaces::ICylinder& cyl) : _cylinder(cyl) {};

            double radius() const
            {
                return cm * _cylinder.radius();
            }

            Vector3D center() const
            {
                return cm * _cylinder.center();
            }

        private:
            const DDSurfaces::ICylinder& _cylinder;
    };


    class DD4hepGeometry : public IGeometry
    {
        public:
            DD4hepGeometry(const DD4hep::Geometry::DetElement& det) : _detelem(det) {};
            ~DD4hepGeometry();

            const std::list<const ISurface*>& getSurfaces();

        private:
            const ::DD4hep::Geometry::DetElement& _detelem;
            std::list<ISurface* > _surfaceList;
            std::list<const ISurface* > _constantSurfaceList;
    };
}
#endif // DD4HEPGEOMETRY_HH

#endif // AIDATT_USE_DD4HEP
