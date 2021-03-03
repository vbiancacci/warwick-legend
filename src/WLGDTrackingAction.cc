#include "WLGDTrackingAction.hh"
#include "WLGDTrackInformation.hh"
#include "WLGDTrajectory.hh"

#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

WLGDTrackingAction::WLGDTrackingAction() = default;

void WLGDTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track if requested
  if(fpTrackingManager->GetStoreTrajectory() > 0)
  {
    fpTrackingManager->SetTrajectory(new WLGDTrajectory(aTrack));
  }

  if(aTrack->GetParticleDefinition()->GetParticleName() == "neutron"){
    auto tmp_vector = aTrack->GetVertexPosition();
    tmp_neutronXpos = tmp_vector.getX();
    tmp_neutronYpos = tmp_vector.getY();
    tmp_neutronZpos = tmp_vector.getZ();
  }

}

void WLGDTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if(secondaries != nullptr)
  {
    WLGDTrackInformation* info  = (WLGDTrackInformation*) (aTrack->GetUserInformation());
    size_t                nSeco = secondaries->size();
    if(nSeco > 0)
    {
      for(size_t i = 0; i < nSeco; i++)
      {
        WLGDTrackInformation* infoNew = new WLGDTrackInformation(info);
        (*secondaries)[i]->SetUserInformation(infoNew);
      }
    }
  }

  if(aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "nCapture")
  {
    G4cout << "Got through nCapture" << G4endl;
    int NumberOfSecundaries = aTrack->GetStep()->GetSecondaryInCurrentStep()->size();
    for(int i = 0; i < NumberOfSecundaries; i++)
    {
      G4cout << "Looking for Ge77" << G4endl;
      if(aTrack->GetStep()->GetSecondaryInCurrentStep()->at(i)->GetParticleDefinition()->GetAtomicMass() == 77
         && aTrack->GetStep()->GetSecondaryInCurrentStep()->at(i)->GetParticleDefinition()->GetPDGCharge() == 32){
        G4cout << "Got it!" << G4endl;
        fEventAction->AddNeutronxLoc(tmp_neutronXpos);
        fEventAction->AddNeutronyLoc(tmp_neutronYpos);
        fEventAction->AddNeutronzLoc(tmp_neutronZpos);
      }
    }
  }
}
