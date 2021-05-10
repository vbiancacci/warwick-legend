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




WLGDSteppingAction::WLGDSteppingAction(WLGDEventAction* event,WLGDRunAction* run)
{
  fEventAction = event;
  fRunAction = run;
  DefineCommands();
}
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
  if(fDepositionInfo == 1)
  {
    if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "ULar_log" ||
       aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Ge_log")
    {
      if(aStep->GetTotalEnergyDeposit() > 0)
      {
        if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Ge_log")
        G4cout << "CopyNumber: " << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() << G4endl;

        G4double tmp_x = aStep->GetTrack()->GetVertexPosition().getX();
        G4double tmp_y = aStep->GetTrack()->GetVertexPosition().getY();
        G4double tmp_z = aStep->GetTrack()->GetVertexPosition().getZ();

        G4int whichReentranceTube;
        if(abs(tmp_x) > abs(tmp_y) && tmp_x > 0)
          whichReentranceTube = 0;
        if(abs(tmp_x) > abs(tmp_y) && tmp_x < 0)
          whichReentranceTube = 2;
        if(abs(tmp_x) < abs(tmp_y) && tmp_y > 0)
          whichReentranceTube = 1;
        if(abs(tmp_x) < abs(tmp_y) && tmp_y < 0)
          whichReentranceTube = 3;

        G4int whichVolume = -1;
        if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "ULar_log")
        {
          whichVolume = 0;
          if(aStep->GetPostStepPoint()->GetGlobalTime() / us < 10.)
          fEventAction->IncreaseLArEnergyDeposition(aStep->GetTotalEnergyDeposit() / eV,
                                                    whichReentranceTube);
          else
            fEventAction->IncreaseLArEnergyDeposition_delayed(aStep->GetTotalEnergyDeposit() / eV,
                                                      whichReentranceTube);
        }

        if(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName() == "Ge_log")
        {
          whichVolume = 1;
          if(aStep->GetPostStepPoint()->GetGlobalTime() / us < 10.)
          fEventAction->IncreaseGeEnergyDeposition(aStep->GetTotalEnergyDeposit() / eV,
                                                    whichReentranceTube);
          else
            fEventAction->IncreaseLArEnergyDeposition_delayed(aStep->GetTotalEnergyDeposit() / eV,
                                                              whichReentranceTube);
        }

        fEventAction->AddIndividualEnergyDeposition_Timing(
          aStep->GetPostStepPoint()->GetGlobalTime() / s);
        fEventAction->AddIndividualEnergyDeposition_Energy(
          aStep->GetTotalEnergyDeposit() / eV);
        fEventAction->AddIndividualEnergyDeposition_ReentranceTube(whichReentranceTube);
        fEventAction->AddIndividualEnergyDeposition_Position_x(tmp_x / m);
        fEventAction->AddIndividualEnergyDeposition_Position_y(tmp_y / m);
        fEventAction->AddIndividualEnergyDeposition_Position_z(tmp_z / m);
        fEventAction->AddIndividualEnergyDeposition_LArOrGe(whichVolume);
      }
    }
  }
}

void WLGDSteppingAction::GetDepositionInfo(G4int answer){fDepositionInfo = answer;}

void WLGDSteppingAction::DefineCommands(){
  // Define geometry command directory using generic messenger class
  fStepMessenger = new G4GenericMessenger(this, "/WLGD/step/",
                                           "Commands for controlling stepping action");

  // switch command
  fStepMessenger->DeclareMethod("getDepositionInfo", &WLGDSteppingAction::GetDepositionInfo)
    .SetGuidance("Set whether to obtain energy deposition information inside reentrance tubes")
    .SetGuidance("0 = don't")
    .SetGuidance("1 = do")
    .SetCandidates("0 1")
    .SetDefaultValue("0");

}