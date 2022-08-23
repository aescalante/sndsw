//
//  Magnet.cxx
//
//  by A.Escalante
//

#include "Magnet.h"

#include "TGeoManager.h"
#include "TString.h"                    // for TString

#include "TClonesArray.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"

#include "TParticle.h"
#include "TParticlePDG.h"
#include "TParticleClassPDG.h"
#include "TVirtualMCStack.h"

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

#include "ShipDetectorList.h"
#include "ShipUnit.h"
#include "ShipStack.h"

#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream,etc
#include <string.h>
#include <cstring>

using std::cout;
using std::endl;
using std::to_string;
using std::string;
using namespace ShipUnit;

Magnet::Magnet()
: FairDetector("Magnet", "",kTRUE),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1)
{
}

Magnet::Magnet(const char* name, Bool_t Active,const char* Title)
: FairDetector(name, true, kTRUE),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1)
{
}

Magnet::~Magnet()
{
}

void Magnet::Initialize()
{
  FairDetector::Initialize();
}

// -----  Private method InitMedium
Int_t Magnet::InitMedium(const char* name)
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

void Magnet::ConstructGeometry()
{

  // MAGNET STRUCTURE
  Double_t fInMagX = conf_floats["Magnet/InMagX"];
  Double_t fInMagY = conf_floats["Magnet/InMagY"];
  Double_t fIronYokeX = conf_floats["Magnet/IronYokeX"];
  Double_t fIronYokeY = conf_floats["Magnet/IronYokeY"];
  Double_t fCoilX = conf_floats["Magnet/CoilX"];
  Double_t fCoilY = conf_floats["Magnet/CoilY"];
  Double_t fOutMagX = conf_floats["Magnet/OutMagX"];
  Double_t fOutMagY = conf_floats["Magnet/OutMagY"];
  Double_t fMagZ = conf_floats["Magnet/MagZ"];
  Double_t fMother = conf_floats["Magnet/Mother"];
  
  // TRACKING STATIONS STRUCTURE
  Double_t fTrackerZ = conf_floats["Magnet/TrackerZ"];
  Double_t fTSpacingZ = conf_floats["Magnet/TSpacingZ"];
  Double_t fLevArm = conf_floats["Magnet/LevArm"];

  // MAGNETIC FIELD
  Double_t fField = conf_floats["Magnet/Field"];
  
  TGeoVolume *top=gGeoManager->FindVolumeFast("Detector");
  if(!top)  LOG(ERROR) << "no Detector volume found " ;

  //Materials 
  // InitMedium("iron");
  // TGeoMedium *Fe =gGeoManager->GetMedium("iron");
  // InitMedium("aluminium");
  // TGeoMedium *Al =gGeoManager->GetMedium("aluminium");
  // InitMedium("polyvinyltoluene");
  // TGeoMedium *Scint =gGeoManager->GetMedium("polyvinyltoluene");
  // InitMedium("Concrete");
  // TGeoMedium *concrete = gGeoManager->GetMedium("Concrete");

  // Shapes creation
  TGeoBBox *MagRegion = new TGeoBBox("MagRegion", fInMagX/2., fInMagY/2., fMagZ/2.+0.5);
  TGeoBBox *CoilContainer = new TGeoBBox("CoilContainer", fOutMagX/2., fOutMagY/2., fMagZ/2.);
  TGeoBBox *Coil = new TGeoBBox("Coil", fCoilX/2., fCoilY/2., fMagZ/2.+0.5);

  TGeoTranslation *CoilUpPos = new TGeoTranslation("CoilUpPos", 0, (fInMagY+fCoilY)/2.-0.001, 0);
  TGeoTranslation *CoilDownPos = new TGeoTranslation("CoilDownPos", 0, -(fInMagY+fCoilY)/2.+0.001, 0);

  // Yoke shape
  TGeoCompositeShape *FeYoke = new TGeoCompositeShape("FeYoke", "CoilContainer-MagRegion-(Coil:CoilUpPos)-(Coil:CoilDownPos)");
  //FeYoke->Draw("ogl");

  // Volumes
  TGeoVolume *volFeYoke = new TGeoVolume("volFeYoke", FeYoke, 0);
  volFeYoke->SetLineColor(kGray);

  TGeoVolume *volCoil = new TGeoVolume("volCoil", Coil, 0);
  volCoil->SetLineColor(kOrange+1);

  // Positioning
  top->AddNode(volFeYoke, 0);
  top->AddNode(volCoil, 0, new TGeoTranslation(0, (fInMagY+fCoilY)/2., 0));
  top->AddNode(volCoil, 1, new TGeoTranslation(0, -(fInMagY+fCoilY)/2., 0));
  
  TGeoBBox *TrackPlane = new TGeoBBox("TrackPlane", fInMagX/2., fInMagY/2., fTrackerZ/2.);

  TGeoVolume *volTrackPlane = new TGeoVolume("volTrackPlane", TrackPlane, 0);
  volTrackPlane->SetLineColor(kBlue);
  volTrackPlane->SetTransparency(60);

  TGeoUniformMagField *magField = new TGeoUniformMagField(-fField,0, 0);
  TGeoVolume *volMagRegion = new TGeoVolume("volMagRegion", MagRegion, 0);
  volMagRegion->SetField(magField);
  top->AddNode(volMagRegion, 0);
}

Bool_t  Magnet::ProcessHits(FairVolume* vol)
{
  //ALB: to be done
}

void Magnet::EndOfEvent()
{
  //ALB: to be done
}

void Magnet::Register()
{
  //ALB: to be done
}

TClonesArray* Magnet::GetCollection(Int_t iColl) const
{
  //ALB: to be done
}

void Magnet::Reset()
{
  //ALB: to be done
}

void Magnet::GetLocalPosition(Int_t fDetectorID, TVector3& vLeft, TVector3& vRight)
{
  //ALB: to be done
}

void Magnet::GetPosition(Int_t fDetectorID, TVector3& vLeft, TVector3& vRight)
{
  //ALB: to be done
}

ClassImp(Magnet)
