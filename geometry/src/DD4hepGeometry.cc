#ifdef AIDATT_USE_DD4HEP

#include "DD4hepGeometry.hh"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDRec/SurfaceManager.h"

namespace aidaTT
{
    DD4hepGeometry::~DD4hepGeometry()
    {
        for(std::list<ISurface*>::iterator surf  = _surfaceList.begin(), lend = _surfaceList.end(); surf != lend; ++surf)
            {
                if(*surf)
                    delete *surf;
            }
    }



    const std::list<const ISurface*>& DD4hepGeometry::getSurfaces()
    {
        // 1. read surface list from detector element
        // 2. convert to correct type

        // create a list of all surfaces in the detector:
        DD4hep::DDRec::SurfaceManager surfMan(_detelem) ;
        const DD4hep::DDRec::SurfaceList& sL = surfMan.surfaceList() ;

        for(DD4hep::DDRec::SurfaceList::const_iterator it = sL.begin() ; it != sL.end() ; ++it)
            {
                ISurface* surf = new DD4hepSurface(*it);
                _surfaceList.push_back(surf);
            }

        /// really, really, really ugly
        for(std::list<ISurface*>::const_iterator csurf = _surfaceList.begin(), last =  _surfaceList.end(); csurf != last; ++csurf)
            _constantSurfaceList.push_back(*csurf);

        return _constantSurfaceList;
    }
}

#endif // AIDATT_USE_DD4HEP
