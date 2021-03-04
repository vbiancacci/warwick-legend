//
// Created by moritz on 3/4/21.
//


#include <iostream>

using namespace std;

#include "WLGDSteppingAction.hh"
#include "WLGDRunAction.hh"

#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLGDSteppingAction::UserSteppingAction(const G4Step *aStep) {


  if(aStep->GetTrack()->GetParticleDefinition()->GetParticleName() == "neutron")
  {
    if(aStep->GetTrack()->GetNextVolume())
    {
      auto physVol1 = aStep->GetTrack()->GetVolume();
      auto physVol2 = aStep->GetTrack()->GetNextVolume();
      if(physVol1->GetName() != "Ge_phys" && physVol2->GetName() == "Ge_phys"){
        fRunAction->increaseNumberOfCrossingNeutrons();
      }
    }
  }

}
