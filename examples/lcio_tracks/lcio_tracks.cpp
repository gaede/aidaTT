#ifdef AIDATT_USE_DD4HEP
#ifdef USE_LCIO

#include "lcio.h"
#include "IO/LCReader.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/TrackerHit.h"
#include "EVENT/TrackerHitPlane.h"
#include "EVENT/Track.h"
#include "UTIL/ILDConf.h"

#include <IMPL/LCCollectionVec.h>
#include "IMPL/TrackImpl.h"
#include "IMPL/TrackStateImpl.h"

// DD4hep
#include "DD4hepGeometry.hh"
#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"
#include "DDRec/SurfaceManager.h"


// aidaTT
#include "AidaTT.hh"
#include "ConstantSolenoidBField.hh"
#include "analyticalPropagation.hh"
#include "simplifiedPropagation.hh"
#include "GBLInterface.hh"
#include "fitResults.hh"
#include "Vector5.hh"
#include "utilities.hh"
#include "LCIOPersistency.hh"
#include "Vector3D.hh"
#include "IGeometry.hh"


// ROOT
#include <TTree.h>
#include <TFile.h>

#include <map>

using namespace std ;
using namespace lcio;
using namespace aidaTT ;

/* this is an example how to use aidaTT with LCIO data
 *
 * in the absence of a track finding procedure, this is a refitting procedure:
 *  - read in the silicon tracks
 *  - read in initial parameters
 *  - read in hits
 *  => create trajectory object from this:
 *          dd4hep surfaces, constant magnetic field, analytical propagation, gbl fitting
 */


int main(int argc, char** argv)
{
    if(argc < 3)
        {
            std::cout << " usage: ./lcio_read_example ILDEx.xml ILDExSimu.slcio" << std::endl ;
            return 1;
        }

    /// dd4hep stuff
    std::string inFile =  argv[1] ;

    /// preamble: load the geo info, get all surfaces => entry point for intersection calculation
    DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();
    lcdd.fromCompact(inFile);

    DD4hep::Geometry::DetElement world = lcdd.world() ;

    aidaTT::DD4hepGeometry geom(world);

    const std::list<const aidaTT::ISurface*>& surfaces = geom.getSurfaces() ;

    // create map of surfaces
    std::map< long64, const aidaTT::ISurface* > surfMap ;

    for(std::list<const aidaTT::ISurface*>::const_iterator surf = surfaces.begin() ; surf != surfaces.end() ; ++surf)
        {
            surfMap[(*surf)->id() ] = (*surf) ;
        }

    /// lcio stuff
    std::string lcioFileName = argv[2] ;

    //*********************************************************************

    int counter = 0 ;

    TFile *ofile = new TFile("ofile.root", "RECREATE");
    //Create tree
    TTree *t1 = new TTree("t1", "t1");
    vector<double> TrackHitResidualsU ;
    t1->Branch("TrackHitResidualsU", &TrackHitResidualsU);
    vector<double> TrackHitResidualsV ;
    t1->Branch("TrackHitResidualsV", &TrackHitResidualsV);
    vector<double> pullU ;
    t1->Branch("pullU", &pullU);
    vector<double> pullV ;
    t1->Branch("pullV", &pullV);
    //int VXDlayer;
    //t1->Branch("VXDlayer",&VXDlayer,"VXDlayer/I");
    vector<int> VXDlayer ;
    t1->Branch("VXDlayer", &VXDlayer);
    vector<double> pullLCIO_U ;
    t1->Branch("pullLCIO_U", &pullLCIO_U);
    vector<double> pullLCIO_V ;
    t1->Branch("pullLCIO_V", &pullLCIO_V);

    //*********************************************************************

    LCReader* rdr = LCFactory::getInstance()->createLCReader() ;
    rdr->open(lcioFileName) ;
    LCWriter* wrt = LCFactory::getInstance()->createLCWriter() ;

    if(argc == 4)
        {
            std::string outFile = argv[3];
            wrt->open(outFile) ;
        }
    else
        wrt->open("innowaythisisnorway.slcio", lcio::LCIO::WRITE_NEW) ;

    LCEvent* evt = 0 ;

    std::string trackCollectionName = "SiTracks";

    UTIL::BitField64 idDecoder(ILDCellID0::encoder_string) ;

    // create the different objects needed for fitting
    // first a constant field parallel to z, 1T
    aidaTT::ConstantSolenoidBField*  bfield = new aidaTT::ConstantSolenoidBField(3.5);

    // create the propagation object
    aidaTT::analyticalPropagation* propagation = new aidaTT::analyticalPropagation();
    //aidaTT::simplifiedPropagation* propagation = new aidaTT::simplifiedPropagation();

    // create the fitter object
    aidaTT::GBLInterface* fitter = new aidaTT::GBLInterface();


    /// event loop
    while((evt = rdr->readNextEvent()) != 0)
        {


            TrackHitResidualsU.clear();
            TrackHitResidualsV.clear();
            pullU.clear();
            pullV.clear();
            VXDlayer.clear();
            pullLCIO_U.clear();
            pullLCIO_V.clear();

            LCCollection* trackCollection = evt->getCollection(trackCollectionName) ;

            // add output track collection to the event
            LCCollectionVec* outCol = new LCCollectionVec(LCIO::TRACK) ;
            evt->addCollection(outCol ,  "AidaTTTracks") ;
            LCFlagImpl trkFlag(0) ;
            trkFlag.setBit(LCIO::TRBIT_HITS) ;
            outCol->setFlag(trkFlag.getFlag()) ;
            TrackImpl* outTrk = new TrackImpl ;
            outCol->addElement(outTrk) ;


            int nTracks = trackCollection->getNumberOfElements();

            // ignore event if more or less than a single track is present
            if(nTracks != 1)
                continue;

            Track* initialTrack = (Track*)trackCollection->getElementAt(0);

            aidaTT::trackParameters iTP(aidaTT::readLCIO(initialTrack->getTrackState(lcio::TrackState::AtIP)));

            std::vector<TrackerHit*> initialHits = initialTrack->getTrackerHits();


            TrackStateImpl* ts;

            bool success;

            aidaTT::trajectory fitTrajectory(iTP, fitter, bfield, propagation, &geom);
            const aidaTT::fitResults* result = &fitTrajectory.getFitResults();


            //********************************************************************************************
            // Checking for LCIO track - hit residuals

            for(std::vector<TrackerHit*>::iterator lhit = initialHits.begin(), endIter = initialHits.end(); lhit < endIter; ++lhit)
                {
                    long64 hitid = (*lhit)->getCellID0() ;
                    idDecoder.setValue(hitid) ;

                    if(idDecoder[ lcio::ILDCellID0::subdet] == lcio::ILDDetID::VXD)
                        {
                            idDecoder[lcio::ILDCellID0::side] = ((*lhit)->getPosition()[2]  >  0  ?   +1 : -1) ;
                            hitid = idDecoder.lowWord() ;
                        }

                    int test_layer = idDecoder[lcio::ILDCellID0::layer] ;

                    const aidaTT::ISurface* surf3 = surfMap[ hitid ] ;

                    if(surf3 != NULL)
                        {

                            TrackerHit* testhit3 = dynamic_cast<TrackerHit*>(*lhit);

                            //in order to calculate track-hit residuals
                            // hits are from LCIO -> unit is [mm], needs to be meter [m]
                            double X2 = testhit3->getPosition()[0] * mm;
                            double Y2 = testhit3->getPosition()[1] * mm;
                            double Z2 = testhit3->getPosition()[2] * mm;

                            //std::cout << " layer " << test_layer << " X " << X2 << " Y " << Y2 << " Z " << Z2 << std::endl ;

                            float globpos2[3] = {X2, Y2, Z2};

                            aidaTT::Vector3D globalPos2(globpos2) ;
                            aidaTT::Vector2D* localPos2 = new Vector2D() ;

                            fitTrajectory._calculateLocalCoordinates(surf3, globalPos2, localPos2);

                            aidaTT::Vector2D* localUV2 = new Vector2D();
                            //Vector3D* xx = new Vector3D();
                            double s2 = 0.;

                            bool doesIt2 = fitTrajectory._calculateIntersectionWithSurface(surf3, s2, localUV2);

                            if(doesIt2)
                                {


                                    double U = localPos2->u();
                                    double V = localPos2->v();

                                    double tU = localUV2->u();
                                    double tV = localUV2->v();

                                    double resU = tU - U ;
                                    double resV = tV - V ;

//          std::cout << " ########## I found the intersection in tU, TV "  << tU << ", "  << tV << " while hit position is at " << U << ", " << V <<  std::endl ;


                                    if(BitSet32(testhit3->getType())[ UTIL::ILDTrkHitTypeBit::COMPOSITE_SPACEPOINT ])        //it is a composite spacepoint
                                        {

                                            // backup stupid methode
                                            //double deltaU = 0.007 * dd4hep::mm  ;
                                            //double deltaV = 0.050 * dd4hep::mm  ;

                                            const LCObjectVec rawObjects = testhit3->getRawHits();

                                            for(unsigned k = 0; k < rawObjects.size(); k++)
                                                {

                                                    TrackerHit* rawHit = dynamic_cast< TrackerHit* >(rawObjects[k]);

                                                    TrackerHitPlane* planarhit3 = dynamic_cast<TrackerHitPlane*>(rawHit);

                                                    double deltaU = planarhit3->getdU() * mm  ;

                                                    pullLCIO_U.push_back(resU / deltaU);

                                                    //std::cout << " 1-dim hit uncertainty in U " << deltaU << std::endl ;

                                                }

                                        }

                                    else
                                        {

                                            TrackerHitPlane* planarhit3 = dynamic_cast<TrackerHitPlane*>(*lhit);

                                            if(planarhit3 != NULL)
                                                {


                                                    double deltaU = planarhit3->getdU() * mm  ;
                                                    double deltaV = planarhit3->getdV() * mm  ;

                                                    //~ std::cout << " AND THE PLANARHIT EXISTS!?!?! dU, dV " << deltaU << ", " << deltaV << std::endl ;

                                                    pullLCIO_U.push_back(resU / deltaU);
                                                    pullLCIO_V.push_back(resV / deltaV);
                                                }
                                        }
                                }
                        }
                }

            //********************************************************************************************

            for(int n = 0; n < 1 ; n++)
                {


                    //aidaTT::trajectory fitTrajectory(iTP, fitter, bfield, propagation, &geom);

                    //~ Vector3D atIP(0.,0.,0.);
                    //~ fitTrajectory.addElement(atIP);

                    for(std::vector<TrackerHit*>::iterator thit = initialHits.begin(), endIter = initialHits.end(); thit < endIter; ++thit)
                        {
                            long64 hitid = (*thit)->getCellID0() ;
                            idDecoder.setValue(hitid) ;

                            if(idDecoder[ lcio::ILDCellID0::subdet] != lcio::ILDDetID::VXD)
                                continue;


                            if(idDecoder[ lcio::ILDCellID0::subdet] == lcio::ILDDetID::VXD)
                                {
                                  //  idDecoder[lcio::ILDCellID0::side] = ((*thit)->getPosition()[2]  >  0  ?   +1 : -1) ;

                                    // increase the layerid by one:
//            unsigned layerID = idDecoder[lcio::ILDCellID0::layer] ;
                                    //idDecoder[lcio::ILDCellID0::layer] = layerID + 1 ;

                                    hitid = idDecoder.lowWord() ;
                                }

                            const aidaTT::ISurface* surf = surfMap[ hitid ] ;

                            if(surf == NULL)
                                {
                                    std::cerr << " lcio_tracks : no surface found for id : " << idDecoder.valueString() << std::endl ;
                                    continue;
                                }


                            double hitpos[3] = {0., 0., 0.};
                            for(unsigned int i = 0; i < 3; ++i)
                                hitpos[i] = (*thit)->getPosition()[i] * mm;

                            std::vector<double> precision;

                            TrackerHitPlane* planarhit = dynamic_cast<TrackerHitPlane*>(*thit);
                            if(planarhit != NULL)
                                {
                                    //we need 1./variance for the precision:
                                    double du = planarhit->getdU() * mm  ;
                                    double dv = planarhit->getdV() * mm  ;

                                    precision.push_back(1. / (du * du)) ;
                                    precision.push_back(1. / (dv * dv)) ;
                                }

                            fitTrajectory.addMeasurement(hitpos, precision, *surf, (*thit));

                            outTrk->addHit(*thit) ;

                        }

                    fitTrajectory.prepareForFitting();

                    success = fitTrajectory.fit();

                    result = &fitTrajectory.getFitResults();


                    //**********************************************************************************************************
                    // Examining track - hit residuals
                    // And write them down to a tree
                    //**********************************************************************************************************

                    aidaTT::trackParameters aidaFittedTP = result->estimatedParameters();

                    aidaTT::trajectory fitTrajectoryDebug(aidaFittedTP, fitter, bfield, propagation, &geom);

                    std::vector<TrackerHit*> finalHits = outTrk->getTrackerHits();

                    for(std::vector<TrackerHit*>::iterator fthit = finalHits.begin(), endIter = finalHits.end(); fthit < endIter; ++fthit)
                        {

                            long64 hitid = (*fthit)->getCellID0() ;
                            idDecoder.setValue(hitid) ;
                           // idDecoder[lcio::ILDCellID0::side] = ((*fthit)->getPosition()[2]  >  0  ?   +1 : -1) ;
                            hitid = idDecoder.lowWord() ;

                            int layerVXD = idDecoder[lcio::ILDCellID0::layer] ;

                            const aidaTT::ISurface* surf2 = surfMap[ hitid ] ;

                            //std::cout << " hit's layer " << layerVXD << " surface " << surf2 << std::endl ;

                            TrackerHit* testhit = dynamic_cast<TrackerHit*>(*fthit);

                            //in order to calculate track-hit residuals
                            double X = testhit->getPosition()[0] * mm;
                            double Y = testhit->getPosition()[1] * mm;
                            double Z = testhit->getPosition()[2] * mm;

                            float globpos[3] = {X, Y, Z};

                            aidaTT::Vector3D globalPos(globpos) ;
                            aidaTT::Vector2D* localPos = new Vector2D() ;

                            fitTrajectoryDebug._calculateLocalCoordinates(surf2, globalPos, localPos);

                            aidaTT::Vector2D* localUV = new Vector2D();
                            //Vector3D* xx = new Vector3D();
                            double s = 0.;

                            bool doesIt = fitTrajectoryDebug._calculateIntersectionWithSurface(surf2, s, localUV);

                            if(doesIt)
                                {

                                    double U = localPos->u();
                                    double V = localPos->v();

                                    double tU = localUV->u();
                                    double tV = localUV->v();

                                    double resU = tU - U ;
                                    double resV = tV - V ;

                                    TrackerHitPlane* planarhit2 = dynamic_cast<TrackerHitPlane*>(*fthit);

                                    double deltaU = planarhit2->getdU() * mm  ;
                                    double deltaV = planarhit2->getdV() * mm  ;

                                    pullU.push_back(resU / deltaU);
                                    pullV.push_back(resV / deltaV);

                                    //std::cout << " res in U = " << resU << " res in V = " << resV << std::endl ;

                                    TrackHitResidualsU.push_back(resU);
                                    TrackHitResidualsV.push_back(resV);

                                    VXDlayer.push_back(layerVXD);

                                    counter++;

                                }

                        }

                    t1->Fill();

                    //***********************************************************************************************************

                    //~ std::cout << " loop " << n << std::endl ;
                    //~ std::cout << " refitted values " << std::endl;
                    //~ std::cout << result->estimatedParameters() << std::endl;
                    //~
                    //iTP = result->estimatedParameters();  // valid only when we make an iterative fitting


                    if(! success)
                        {

                            std::cout << " ********** ERROR:  Fit Failed !!!!! ******************************* " << std::endl ;
                        }


                }




            //~ std::cout << " initial values " << std::endl;
            //~ std::cout << iTP << std::endl;
            //~ std::cout << " refitted values " << std::endl;
            //~ std::cout << result->estimatedParameters() << std::endl;
            //~
            // add Track State to track:
            ts = aidaTT::createLCIO(result->estimatedParameters());
            //ts = aidaTT::createLCIO( iTP );  // only to check the initial helix

            outTrk->setChi2(result->chiSquare()) ;
            outTrk->setNdf(result->ndf()) ;
            outTrk->subdetectorHitNumbers().resize(10.) ;

            outTrk->subdetectorHitNumbers()[0] = outTrk->getTrackerHits().size() ;

            float ref[3] = { 0., 0. , 0. } ;
            ts->setReferencePoint(ref);

            ts->setLocation(lcio::TrackState::AtIP);


            outTrk->addTrackState(ts);

            wrt->writeEvent(evt) ;

        }

    ofile->Write("t1");

    std::cout << " counter = " << counter << std::endl ;

    return 0;
}


#endif // USE_LCIO
#endif // USE_DD4HEP
