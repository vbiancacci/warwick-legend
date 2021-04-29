//
// Created by moritz on 3/4/21.
//


#include <iostream>

using namespace std;
#include "WLGDTrackingAction.hh"
#include "WLGDSteppingAction.hh"
#include "WLGDRunAction.hh"

#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLGDSteppingAction::UserSteppingAction(const G4Step *aStep) {


  // Edit: 2021/03/05 by Moritz Neuberger
  // Adding tracking of amount of neutrons crossing the detectors
  if(aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "neutron")
  {
    if(aStep->GetTrack()->GetNextVolume())
    {
      auto physVol1 = aStep->GetTrack()->GetVolume();
      auto physVol2 = aStep->GetTrack()->GetNextVolume();
      if(physVol1->GetName() != "Ge_phys" && physVol2->GetName() == "Ge_phys"){
        if(fRunAction->getWriteOutGeneralNeutronInfo() == 1) fRunAction->increaseNumberOfCrossingNeutrons();
      }
    }
  }

  // Edit: 2021/04/07 by Moritz Neuberger TODO
  // Adding total energy deposition inside LAr
  /*
  if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Lar_log")
  {
    fEventAction->IncreaseLArEnergyDeposition(aStep->GetTotalEnergyDeposit());
  }*/

}
