#include "WLGDStackingAction.hh"
#include "WLGDTrackInformation.hh"

G4ClassificationOfNewTrack WLGDStackingAction ::ClassifyNewTrack(const G4Track* aTrack)
{
  G4cout << aTrack->GetParticleDefinition()->GetParticleName() << G4endl;
  if(aTrack->GetParticleDefinition()->GetParticleName() == "neutron")
  {
    G4cout << "Pre: " << aTrack->GetStep()->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
    G4cout << "Post: " << aTrack->GetStep()->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
    //if(aTrack->GetStep()->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "Ge_phys")
  }

  G4ClassificationOfNewTrack classification = fUrgent;

  if(aTrack->GetParentID() == 0)  // Primary particle
  {
    WLGDTrackInformation* trackInfo = new WLGDTrackInformation(aTrack);
    aTrack->SetUserInformation(trackInfo);  // attach own track info
  }
  return classification;
}

void WLGDStackingAction::NewStage() { ; }

void WLGDStackingAction::PrepareNewEvent() { ; }
