#ifdef AIDATT_USE_DD4HEP

#ifndef DD4HEPMATERIAL_HH
#define DD4HEPMATERIAL_HH

#include "IGeometry.hh"
#include "DDSurfaces/IMaterial.h"
#include "aidaTT-Units.hh"

namespace aidaTT
{
    class DD4hepMaterial : public IMaterial
    {
        public:
            DD4hepMaterial(const DDSurfaces::IMaterial& im) : _material(im) {};

            std::string getName() const
            {
                return _material.name();
            };

            double getZ() const
            {
                return _material.Z();
            } ;

            /// averaged atomic number
            double getA() const
            {
                return _material.A();
            } ;

            /// density - TODO::Unit of density!
            double getDensity() const
            {
                return _material.density();
            } ;

            /// radiation length - TODO::Unit of radiation length!
            double getRadiationLength() const
            {
                return _material.radiationLength();
            } ;

            /// interaction length - TODO::Unit of interaction length!
            double getInteractionLength() const
            {
                return _material.interactionLength();
            } ;
        private:
            const DDSurfaces::IMaterial& _material;
    };


}
#endif // DD4HEPMATERIAL_HH

#endif // AIDATT_USE_DD4HEP
