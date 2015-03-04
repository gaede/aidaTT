#ifdef AIDATT_USE_DD4HEP

#ifndef DD4HEPSURFACE_HH
#define DD4HEPSURFACE_HH

#include <ostream>

#include "IGeometry.hh"
#include "DD4hepMaterial.hh"
#include "aidaTT-Units.hh"

#include "DDSurfaces/ISurface.h"


namespace aidaTT
{

    class DD4hepSurface : public ISurface
    {
        public:
            DD4hepSurface(const DDSurfaces::ISurface* is) : _surface(is) {};

            long long int     id() const
            {
                return _surface->id();
            }

            /// Checks if the given point lies within the surface
            // ! aidaTT is in [m], dd4hep in [cm]
            // argument needs to be converted to cm
            bool     insideBounds(const Vector3D& point, double epsilon = 1.e-4) const
            {
                return _surface->insideBounds(m2cm * point, epsilon);
            }

            /** First direction of measurement U */
            Vector3D  u(const Vector3D& point = Vector3D()) const
            {
                return _surface->u(point);
            }

            /** Second direction of measurement V */
            Vector3D v(const Vector3D& point = Vector3D()) const
            {
                return _surface->v(point);
            }

            /// Access to the normal direction at the given point
            // ! aidaTT is in [m], dd4hep in [cm]
            // convert argument to cm
            Vector3D normal(const Vector3D& point = Vector3D()) const
            {
                return _surface->normal(m2cm * point);
            }

            /** Convert the global position to the local position (u,v) on the surface */
            // ! aidaTT is in [m], dd4hep in cm:
            //  argument needs to be converted from m to cm, return value from cm to m
            Vector2D globalToLocal(const Vector3D& point) const
            {
                Vector2D local = _surface->globalToLocal(m2cm * point);
                local.u() *= cm;
                local.v() *= cm;

                return local;
            }

            /** Convert the local position (u,v) on the surface to the global position*/
            // ! aidaTT is in [m], dd4hep in [cm]
            // convert argument from m to cm, return value from cm to m
            Vector3D localToGlobal(const Vector2D& point) const
            {
                Vector2D local = point;
                local.u() *= m2cm;
                local.v() *= m2cm;
                return cm * _surface->localToGlobal(local) ;
            }

            /** Get Origin of local coordinate system on surface */
            // ! aidaTT is in [m], dd4hep in [cm]
            // convert return value to m
            Vector3D origin() const
            {
                return cm * _surface->origin();
            }

            /** Thickness of inner material */
            // ! aidaTT is in [m], dd4hep in [cm]
            // convert return value to m
            double innerThickness() const
            {
                return _surface->innerThickness() * cm;
            }

            /** Thickness of outer material */
            // ! aidaTT is in [m], dd4hep in [cm]
            // convert return value to m
            double outerThickness() const
            {
                return _surface->outerThickness() * cm;
            }

            /** Distance to surface */
            // ! aidaTT is in [m], dd4hep in [cm]
            // convert return value to m
            double distance(const Vector3D& point) const
            {
                return _surface->distance(point) * cm;
            }

            /// true if surface is sensitive
            bool isSensitive() const
            {
                return _surface->type().isSensitive();
            }

            /// true if this a planar surface
            bool isPlane() const
            {
                return _surface->type().isPlane();
            }

            /// true if this a cylindrical surface
            bool isCylinder() const
            {
                return _surface->type().isCylinder();
            }

            /// true if surface is parallel to Z
            bool isParallelToZ() const
            {
                return _surface->type().isParallelToZ();
            }

            /// true if surface is orthogonal to Z
            bool isOrthogonalToZ() const
            {
                return _surface->type().isOrthogonalToZ();
            }

            /// true if this is a cylinder parallel to Z
            bool isZCylinder() const
            {
                return _surface->type().isZCylinder();
            }

            /// true if this is a plane parallel to Z
            bool isZPlane() const
            {
                return _surface->type().isZPlane();
            }

            /// true if this is a plane orthogonal to Z
            bool isZDisk() const
            {
                return _surface->type().isZDisk();
            }

            /** True if surface is parallel to Z with accuracy epsilon*/
            bool checkParallelToZ(double epsilon = 1.e-6) const
            {
                return _surface->type().checkParallelToZ(*_surface, epsilon);
            }

            /** True if surface is orthogonal to Z with accuracy epsilon  */
            bool checkOrthogonalToZ(double epsilon = 1.e-6) const
            {
                return _surface->type().checkOrthogonalToZ(*_surface, epsilon);
            }

            /// Access to the material in opposite direction of the normal
            const IMaterial& innerMaterial() const
            {
                return DD4hepMaterial(_surface->innerMaterial());
            }

            /// Access to the material in direction of the normal
            const IMaterial& outerMaterial() const
            {
                return DD4hepMaterial(_surface->outerMaterial());
            }


        private:
            const DDSurfaces::ISurface* _surface;
    };

}
#endif // DD4HEPSURFACE_HH

#endif // AIDATT_USE_DD4HEP
