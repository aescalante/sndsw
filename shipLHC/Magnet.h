//
//  Magnet.h
//
//  by A. Escalante

#ifndef Magnet_H
#define Magnet_H

#include "FairModule.h"                 // for FairModule
#include "FairDetector.h"

#include "Rtypes.h"                     // for ShipMuonShield::Class, Bool_t, etc

#include <string>                       // for string

#include "TVector3.h"
#include "TString.h"
#include "TLorentzVector.h"

class FairVolume;

class Magnet : public FairDetector
{
	public:
		Magnet(const char* name, Bool_t Active, const char* Title="Magnet");
		Magnet();
		virtual ~Magnet();

		/** Create the detector geometry **/
		void ConstructGeometry();

    /** Getposition **/
    void GetPosition(Int_t id, TVector3& vLeft, TVector3& vRight); // or top and bottom
    void GetLocalPosition(Int_t id, TVector3& vLeft, TVector3& vRight);

		void SetConfPar(TString name, Float_t value){conf_floats[name]=value;}
		void SetConfPar(TString name, Int_t value){conf_ints[name]=value;}
		void SetConfPar(TString name, TString value){conf_strings[name]=value;}
		Float_t  GetConfParF(TString name){return conf_floats[name];}
		Int_t       GetConfParI(TString name){return conf_ints[name];}
		TString  GetConfParS(TString name){return conf_strings[name];}

		/** Initialization of the detector is done here    */
		virtual void Initialize();

		/** Method called for each step during simulation (see FairMCApplication::Stepping()) */
		virtual Bool_t ProcessHits( FairVolume* v=0);  //ALB: to be done

		/**       Registers the produced collections in FAIRRootManager.     */
		virtual void   Register(); //ALB: to be done

		/** Gets the produced collections */
		virtual TClonesArray* GetCollection(Int_t iColl) const ; //ALB: to be done

		/**      has to be called after each event to reset the containers      */
		virtual void   Reset(); //ALB: to be done

		virtual void   CopyClones( TClonesArray* cl1,  TClonesArray* cl2 , Int_t offset) {;}
		virtual void   SetSpecialPhysicsCuts() {;}
		virtual void   EndOfEvent();  //ALB: to be done
		virtual void   FinishPrimary() {;}
		virtual void   FinishRun() {;}
		virtual void   BeginPrimary() {;}
		virtual void   PostTrack() {;}
		virtual void   PreTrack() {;}
		virtual void   BeginEvent() {;}

		Magnet(const Magnet&);
		Magnet& operator=(const Magnet&);

		ClassDef(Magnet,6) //IDs, ClassDef(Scifi,3), ClassDef(EmulsionDet,5), ClassDef(MuFilter,4), ClassDef(Floor,2)

	private:

			/** Track information to be stored until the track leaves the active volume. */
			Int_t          fTrackID;           //!  track index
			Int_t          fVolumeID;          //!  volume id
			TLorentzVector fPos;               //!  position at entrance
			TLorentzVector fMom;               //!  momentum at entrance
			Double32_t     fTime;              //!  time
			Double32_t     fLength;            //!  length
			Double32_t     fELoss;             //!  energy loss

			/** configuration parameters **/
			std::map<TString,Float_t> conf_floats;
			std::map<TString,Int_t> conf_ints;
			std::map<TString,TString> conf_strings;

	protected:

			Int_t InitMedium(const char* name);
};

#endif
