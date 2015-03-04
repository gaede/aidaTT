#ifndef IGEOMETRY_HH
#define IGEOMETRY_HH

#include <list>
#include <string>
#include <ostream>

#include "Vector3D.hh"
#include "Vector2D.hh"

/* This hides the actual implementation of geometry information.
 * Geometry is needed for the calculation of
 *  - crossing points between elements and a track
 *  - measurement directions (if a sensitive element)
 *  - material properties for multiple scattering and enery loss
 *
 * There are several elements to the geometry:
 *  - the abstract material definition
 *  - the abstract surface definition
 *
 *  @author: Christoph Rosemann, DESY, Frank Gaede, DESY and Christian Grefe, CERN
 */

namespace aidaTT
{

    class IMaterial
    {

        public:

            virtual std::string getName() const = 0;

            /// averaged proton number
            virtual double getZ() const = 0 ;

            /// averaged atomic number
            virtual double getA() const = 0 ;

            /// density - units ?
            virtual double getDensity() const = 0 ;

            /// radiation length - units ?
            virtual double getRadiationLength() const = 0 ;

            /// interaction length - units ?
            virtual double getInteractionLength() const = 0 ;
    };

    /// dump IMaterial operator
    inline std::ostream& operator<<(std::ostream& os , const IMaterial& m)
    {

        os << "  " << m.getName() << ", A: " << m.getA()  << ", Z: " << m.getZ() << ", density: " << m.getDensity() << ", radiationLength: " <<  m.getRadiationLength()
           << ", interactionLength: " << m.getInteractionLength() ;

        return os ;
    }



    class ISurface
    {

        public:
            /// Destructor
            virtual ~ISurface() {}

            /// The id of this surface
            virtual long long int id() const = 0 ;

            /// Checks if the given point lies within the surface
            virtual bool insideBounds(const Vector3D& point, double epsilon = 1.e-4) const = 0 ;

            /** First direction of measurement U */
            virtual Vector3D u(const Vector3D& point = Vector3D()) const = 0 ;

            /** Second direction of measurement V */
            virtual Vector3D v(const Vector3D& point = Vector3D()) const = 0 ;

            /// Access to the normal direction at the given point
            virtual Vector3D normal(const Vector3D& point = Vector3D()) const = 0 ;

            /** Convert the global position to the local position (u,v) on the surface */
            virtual Vector2D globalToLocal(const Vector3D& point) const = 0 ;

            /** Convert the local position (u,v) on the surface to the global position*/
            virtual Vector3D localToGlobal(const Vector2D& point) const = 0 ;

            /** Get Origin of local coordinate system on surface */
            virtual Vector3D origin() const = 0 ;

            /// Access to the material in opposite direction of the normal
            virtual const IMaterial& innerMaterial() const = 0 ;

            /// Access to the material in direction of the normal
            virtual const IMaterial& outerMaterial() const = 0 ;

            /** Thickness of inner material */
            virtual double innerThickness() const = 0 ;

            /** Thickness of outer material */
            virtual double outerThickness() const = 0 ;

            /** Distance to surface */
            virtual double distance(const Vector3D& point) const = 0 ;

            /// true if surface is sensitive
            virtual bool isSensitive() const = 0;

            /// true if this a planar surface
            virtual bool isPlane() const = 0;

            /// true if this a cylindrical surface
            virtual bool isCylinder() const = 0;

            /// true if surface is parallel to Z
            virtual bool isParallelToZ() const = 0;

            /// true if surface is orthogonal to Z
            virtual bool isOrthogonalToZ() const = 0;

            /// true if this is a cylinder parallel to Z
            virtual bool isZCylinder() const = 0;

            /// true if this is a plane parallel to Z
            virtual bool isZPlane() const = 0;

            /// true if this is a plane orthogonal to Z
            virtual bool isZDisk() const = 0;

            /** True if surface is parallel to Z with accuracy epsilon*/
            virtual bool checkParallelToZ(double epsilon = 1.e-6) const = 0;

            /** True if surface is orthogonal to Z with accuracy epsilon  */
            virtual bool checkOrthogonalToZ(double epsilon = 1.e-6) const = 0;
    } ;


    /// dump ISurface operator
    inline std::ostream& operator<<(std::ostream& os , const ISurface& s)
    {

        os <<  "   id: " << std::hex << s.id() << std::dec  << std::endl
           <<  "   u : " << s.u() << " v : " << s.v() << " normal : " << s.normal() << " origin : " << s.origin() << std::endl   ;
        os <<  "   inner material : " << s.innerMaterial() << "  thickness: " <<  s.innerThickness()  << std::endl
           <<  "   outerMaterial :  " << s.outerMaterial() << "  thickness: " <<  s.outerThickness()  << std::endl   ;
        return os;
    }

    class ICylinder
    {
        public:
            /// Destructor
            virtual ~ICylinder() {}
            virtual double radius() const  = 0 ;
            virtual Vector3D center() const = 0 ;
    };

    class IGeometry
    {
        public:
            virtual const std::list<const ISurface*>& getSurfaces() = 0;
    };
}
#endif // IGEOMETRY_HH
