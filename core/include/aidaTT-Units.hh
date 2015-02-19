/* this is the declaration of units used in exchange to the outside world inside aidaTT
 *
 * the first main interaction point is the geometry, that is
 *  -- the information whether a point is on a surface
 *  -- the material information
 *  regarding the lengths, the standard unit is meter.
 *
 * the second is the declaration of track parameters which is the
 * L3 track parametrization [ Omega, tan(lambda), phi_0, d_0, z_0 ]
 *      Omega is the inverse curvature, therefore [mm^{-1}]
 *      d_0 is in [m]
 *      z_0 is in [m]
 *
 * This then means that the conversion from curvature in [mm^{-1}]
 * to inverse momentum in [GeV^{-1}] needs
 *      a Bfield given in [T], and
 *      a speed of light given in [m/s]
 */
//~ 
//~ #ifdef AIDATT_USE_DD4HEP
//~ #include "DD4hep/DD4hepUnits.h"
//~ 
//~ namespace aidaTT
//~ {
    //~ using  dd4hep::millimeter;
    //~ using  dd4hep::centimeter;
    //~ using  dd4hep::meter;
//~ 
    //~ using  dd4hep::megaelectronvolt;
    //~ using  dd4hep::electronvolt;
    //~ using  dd4hep::kiloelectronvolt;
    //~ using  dd4hep::gigaelectronvolt;
    //~ using  dd4hep::teraelectronvolt;
    //~ using  dd4hep::petaelectronvolt;
//~ 
    //~ //static const double convertBr2P_cm = 0.299792458 * (centimeter / millimeter);
    //~ //static const double convertBr2P_cm = 1.;//0.299792458 * (centimeter / meter);
//~ }
//~ #endif // AIDATT_UNITS_HH


#ifndef AIDATT_UNITS_HH
#define AIDATT_UNITS_HH
namespace aidaTT
{
    static const double mm = 1.;
    static const double cm = 1e1;
    static const double mum = 1e-3;
    static const double m = 1e3;
}

#endif // AIDATT_USE_DD4HEP

