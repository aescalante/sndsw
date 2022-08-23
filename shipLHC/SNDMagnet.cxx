//
// SNDMagnet.cxx
//
//
//
//

#include "SNDMagnet.h"

#include "TGeoManager.h"
#include "FairRun.h"                    // for FairRun
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include <iosfwd>                    // for ostream
#include "TList.h"                      // for TListIter, TList (ptr only)
#include "TObjArray.h"                  // for TObjArray
#include "TString.h"                    // for TString

#include "TGeoBBox.h"
#include "TGeoTrd1.h"
#include "TGeoSphere.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoTrd1.h"
#include "TGeoArb8.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoTransform.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoBuilder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "ShipDetectorList.h"
#include "ShipUnit.h"
#include "ShipStack.h"

#include "TGeoTrd2.h" 
#include "TGeoCompositeShape.h"

#include "TGeoUniformMagField.h"
#include "TVector3.h"
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream,etc
#include <string.h>

using std::cout;
using std::endl;

using namespace ShipUnit;

SNDMagnet::~SNDMagnet()
{}

SNDMagnet::SNDMagnet():FairModule("SNDMagnet", "")
{}

void SNDMagnet::SetMagneticField(Double_t B)
{
  fField=B;
}

// -----   Private method InitMedium
Int_t SNDMagnet::InitMedium(const char* name)
{
  static FairGeoLoader *geoLoad=FairGeoLoader::Instance();
  static FairGeoInterface *geoFace=geoLoad->getGeoInterface();
  static FairGeoMedia *media=geoFace->getMedia();
  static FairGeoBuilder *geoBuild=geoLoad->getGeoBuilder();
    
  FairGeoMedium *ShipMedium=media->getMedium(name);
    
  if (!ShipMedium)
    {
      Fatal("InitMedium","Material %s not defined in media file.", name);
      return -1111;
    }
  TGeoMedium* medium=gGeoManager->GetMedium(name);
  if (medium!=NULL)
    return ShipMedium->getMediumIndex();
  return geoBuild->createMedium(ShipMedium);
}

void SNDMagnet::ConstructGeometry()
{
    TGeoVolume *top=gGeoManager->GetTopVolume();
    TGeoVolume *tunnel = gGeoManager->FindVolumeFast("Tunnel");
    if(!tunnel)  LOG(ERROR) << "no Tunnel volume found " ;

    // Materials

    InitMedium("air");
    TGeoMedium *air =gGeoManager->GetMedium("air");  

    InitMedium("iron");
    TGeoMedium *Fe =gGeoManager->GetMedium("iron");

    InitMedium("CoilCopper");
    TGeoMedium *Cu  = gGeoManager->GetMedium("CoilCopper");

    // Magnetic Field

    TGeoUniformMagField *magField = new TGeoUniformMagField(-fField,0, 0);

    TGeoVolumeAssembly *MagnetVol = new TGeoVolumeAssembly("Magnet");

    // TO BE CHANGED USING CONF_FLOATS
    Double_t fInMagX = 120; // cm
    Double_t fInMagY = 60; // cm
    Double_t fIronYokeX = 30; // cm
    Double_t fIronYokeY = 25; // cm
    Double_t fCoilX = fInMagX;
    Double_t fCoilY = 23; // cm
    Double_t fOutMagX = fInMagX + 2*fIronYokeX;
    Double_t fOutMagY = fInMagY + 2*(fCoilY+fIronYokeY);
    Double_t fMagZ = 200; // cm

    // Shapes creation
    TGeoBBox *CoilContainer = new TGeoBBox("CoilContainer", fOutMagX/2., fOutMagY/2., fMagZ/2.);
    TGeoBBox *MagRegion = new TGeoBBox("MagRegion", fInMagX/2., fInMagY/2., fMagZ/2.+0.5);
    TGeoBBox *Coil = new TGeoBBox("Coil", fCoilX/2., fCoilY/2., fMagZ/2.+0.5);

    // Translations
    TGeoTranslation *CoilUpPos = new TGeoTranslation("CoilUpPos", 0, (fInMagY+fCoilY)/2.-0.001, 0);
    TGeoTranslation *CoilDownPos = new TGeoTranslation("CoilDownPos", 0, -(fInMagY+fCoilY)/2.+0.001, 0);
    CoilUpPos->RegisterYourself();
    CoilDownPos->RegisterYourself();

    // Yoke shape
    TGeoCompositeShape *FeYoke = new TGeoCompositeShape("FeYoke", "CoilContainer-MagRegion-(Coil:CoilUpPos)-(Coil:CoilDownPos)");

    // Volumes
    TGeoVolume *volFeYoke = new TGeoVolume("volFeYoke", FeYoke, Fe);
    volFeYoke->SetLineColor(kGray);
    TGeoVolume *volCoil = new TGeoVolume("volCoil", Coil, Cu);
    volCoil->SetLineColor(kOrange+1);

    // Positioning
    MagnetVol->AddNode(volFeYoke, 0);
    MagnetVol->AddNode(volCoil, 0, new TGeoTranslation(0, (fInMagY+fCoilY)/2., 0));
    MagnetVol->AddNode(volCoil, 1, new TGeoTranslation(0, -(fInMagY+fCoilY)/2., 0));

    Double_t fTrackerZ = 0.5; //cm
    Double_t fTSpacingZ = 2; // cm
    Double_t fLevArm = 100; // cm

    TGeoBBox *TrackPlane = new TGeoBBox("TrackPlane", fInMagX/2., fInMagY/2., fTrackerZ/2.);

    // TO BE PROPERLY ADDED

    /*TGeoVolume *volTrackPlane = new TGeoVolume("volTrackPlane", TrackPlane, 0);
    volTrackPlane->SetLineColorAlpha(kBlue, 0.4);
    //AddSensitiveVolume(volTrackPlane);

    volMother->AddNode(volTrackPlane, 0, new TGeoTranslation(0, 0, -fMagZ/2.-fTSpacingZ-fTrackerZ/2.));
    volMother->AddNode(volTrackPlane, 1, new TGeoTranslation(0, 0, +fMagZ/2.+fTSpacingZ+fTrackerZ/2.));
    volMother->AddNode(volTrackPlane, 2, new TGeoTranslation(0, 0, -fMagZ/2.-fTSpacingZ-fTrackerZ-fLevArm-fTrackerZ/2.));
    volMother->AddNode(volTrackPlane, 3, new TGeoTranslation(0, 0, +fMagZ/2.+fTSpacingZ+fTrackerZ+fLevArm+fTrackerZ/2.));*/

    TGeoVolume *volMagRegion = new TGeoVolume("volMagRegion", MagRegion, air);
    volMagRegion->SetField(magField);

    MagnetVol->AddNode(volMagRegion, 0, 0);


    tunnel->AddNode(MagnetVol, 0, 0)
}