#include "WLGDTrackingAction.hh"
// #include "WLGDTrackInformation.hh"
#include "WLGDTrajectory.hh"

#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4UnitsTable.hh"
#include <map>

WLGDTrackingAction::WLGDTrackingAction() = default;

void WLGDTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{


    // Create trajectory for track if requested
    if(fpTrackingManager->GetStoreTrajectory() > 0)
    {
        fpTrackingManager->SetTrajectory(new WLGDTrajectory(aTrack));
    }

    if(aTrack->GetParticleDefinition()
               ->GetAtomicMass() == 77 &&
       aTrack->GetParticleDefinition()
               ->GetPDGCharge() == 32) {

        fEventAction->AddIDListOfGe77(aTrack->GetTrackID());
    }

    // Edit: 2021/03/30 by Moritz Neuberger
    // Adding tracking of neutrons being later captured by Ge-76 as well as general produced
    // in LAr

    if(aTrack->GetParticleDefinition()->GetParticleName() == "mu-" || aTrack->GetParticleDefinition()->GetParticleName() == "mu+") {
        auto tmp_vector = aTrack->GetVertexPosition();
        tmp_MuonXpos = tmp_vector.getX() / m;
        tmp_MuonYpos = tmp_vector.getY() / m;
        tmp_MuonZpos = tmp_vector.getZ() / m;
        tmp_vector   = aTrack->GetMomentumDirection();
        tmp_MuonXmom = tmp_vector.getX();
        tmp_MuonYmom = tmp_vector.getY();
        tmp_MuonZmom = tmp_vector.getZ();
    }


    if(aTrack->GetParticleDefinition()->GetParticleName() == "neutron")
    {

        auto tmp_vector = aTrack->GetVertexPosition();
        tmp_neutronXpos = tmp_vector.getX() / m;
        tmp_neutronYpos = tmp_vector.getY() / m;
        tmp_neutronZpos = tmp_vector.getZ() / m;
        tmp_vector      = aTrack->GetMomentumDirection();
        tmp_neutronXmom = tmp_vector.getX();
        tmp_neutronYmom = tmp_vector.getY();
        tmp_neutronZmom = tmp_vector.getZ();

        if(fRunAction->getWriteOutAllNeutronInfoRoot() == 1)
        {
            fEventAction->AddNeutronxLoc(tmp_neutronXpos);
            fEventAction->AddNeutronyLoc(tmp_neutronYpos);
            fEventAction->AddNeutronzLoc(tmp_neutronZpos);
            fEventAction->AddNeutronxMom(tmp_neutronXmom);
            fEventAction->AddNeutronyMom(tmp_neutronYmom);
            fEventAction->AddNeutronzMom(tmp_neutronZmom);
            fEventAction->AddNeutronTime(tmp_neutronTime);
        }

        fEventAction->SetMostOuterRadius(sqrt(tmp_neutronXpos*tmp_neutronXpos + tmp_neutronYpos*tmp_neutronYpos));

        //tmp_neutronTime = aTrack->GetStep()->GetPostStepPoint()->GetGlobalTime() / s;
        if(fRunAction->getWriteOutGeneralNeutronInfo() == 1)
            fEventAction->IncreaseByOne_NeutronInEvent();

        if(fRunAction->getWriteOutNeutronProductionInfo() == 1)
        {// Tank_log Cout_log Cvac_log Cinn_log Lid_log BoratedPET_Logical
        /*    if(aTrack->GetLogicalVolumeAtVertex()->GetName() == "Water_log" || 
		aTrack->GetLogicalVolumeAtVertex()->GetName() == "Lar_log" ||
               aTrack->GetLogicalVolumeAtVertex()->GetName() == "ULar_log" ||
               aTrack->GetLogicalVolumeAtVertex()->GetName() == "Ge_log" ||
               aTrack->GetLogicalVolumeAtVertex()->GetName() ==
               "Copper_log")  // ULar_phys  Ge_phys
            {*/
                fRunAction->increaseTotalNumberOfNeutronsInLAr();
                fRunAction->addCoordinatsToFile(tmp_neutronXpos, tmp_neutronYpos,
                                                tmp_neutronZpos);
                fRunAction->addMomentumToFile(tmp_neutronXmom, tmp_neutronYmom, tmp_neutronZmom);
                fRunAction->addEnergyToFile(aTrack->GetKineticEnergy() / eV);
                fRunAction->addParentParticleType(
                        fEventAction->neutronProducerMap.find(aTrack->GetParentID())->second);
            //}
        }
    }


}

void WLGDTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{

    if(fRunAction->getWriteOutAdvancedMultiplicity()){
        if(fEventAction->GetIDListOfGdSiblingParticles().count(aTrack->GetParentID())){
                fEventAction->AddIDListOfGdSiblingParticles(aTrack->GetTrackID());
        }
    }

    if(fEventAction->GetIDListOfGe77SiblingParticles().count(aTrack->GetParentID())){
            fEventAction->AddIDListOfGe77SiblingParticles(aTrack->GetTrackID());
    }

    if(fEventAction->GetIDListOfGe77().count(aTrack->GetParentID())){
            fEventAction->AddIDListOfGe77(aTrack->GetTrackID());
    }


    if(aTrack->GetTrackID() == 1){
        fEventAction->AddMuonxLoc(tmp_MuonXpos);
        fEventAction->AddMuonyLoc(tmp_MuonYpos);
        fEventAction->AddMuonzLoc(tmp_MuonZpos);
        fEventAction->AddMuonxMom(tmp_MuonXmom);
        fEventAction->AddMuonyMom(tmp_MuonYmom);
        fEventAction->AddMuonzMom(tmp_MuonZmom);
    }

    if(fRunAction->getWriteOutNeutronProductionInfo() == 1)
    {
        G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
        if(secondaries != nullptr)
        {
            size_t nSeco = secondaries->size();
            if(nSeco > 0)
            {
                for(size_t i = 0; i < nSeco; i++)
                {
                    // Edit: 2021/03/30 by Moritz Neuberger
                    // Adding map of parent particles that create neutrons used above
                    if((*secondaries)[i]->GetParticleDefinition()->GetParticleName() == "neutron")
                    {
                        fEventAction->neutronProducerMap.insert(
                                std::make_pair((int) aTrack->GetTrackID(),
                                               (int) aTrack->GetParticleDefinition()->GetPDGEncoding()));
                    }
                }
            }
        }
    }

    // For Ge77m IC readout

    if(aTrack->GetParticleDefinition()->GetPDGEncoding() == 1000320771){
        fEventAction->SetisMetastable(1);
        int NumberOfSecundaries = aTrack->GetStep()->GetSecondaryInCurrentStep()->size();
        for(int i = 0; i < NumberOfSecundaries; i++){
            if(aTrack->GetStep()
                    ->GetSecondaryInCurrentStep()
                    ->at(i)
                    ->GetParticleDefinition()
                    ->GetParticleName() == "gamma"
                    && abs(aTrack->GetStep()
                            ->GetSecondaryInCurrentStep()
                            ->at(i)
                            ->GetTotalEnergy() / eV - 160e3) < 1e3)
                fEventAction->SetisIC(1);
        }
    }



    // Edit: 2021/03/30 by Moritz Neuberger
    // Adding tracking of neutrons being later captured by Ge-76

    // RemoveAddIDListOfGe77OfGe77SiblingParticles
/*
    if((aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "neutronInelastic"
        || aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "nFission"
        || aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "hBertiniCaptureAtRest"
        || aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "pi-Inelastic"
        || aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "Decay")
        && aTrack->GetStep()->GetPostStepPoint()
                            ->GetTouchable()
                            ->GetVolume()
                            ->GetLogicalVolume()
                            ->GetName() == "Water_log"
            aTrack->GetVolume()->GetName() == "Water_phys"){
        fEventAction->AddIDListOfGdSiblingParticles(aTrack->GetTrackID());
    }
*/


    if(aTrack->GetParticleDefinition()->GetParticleName() == "neutron")
    {
        if(aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() ==
           "biasWrapper(nCapture)")
        {
            int NumberOfSecundaries = aTrack->GetStep()->GetSecondaryInCurrentStep()->size();
            for(int i = 0; i < NumberOfSecundaries; i++)
            {
                if(aTrack->GetStep()
                           ->GetSecondaryInCurrentStep()
                           ->at(i)
                           ->GetParticleDefinition()
                           ->GetAtomicMass() == 77 &&
                   aTrack->GetStep()
                           ->GetSecondaryInCurrentStep()
                           ->at(i)
                           ->GetParticleDefinition()
                           ->GetPDGCharge() == 32)
                {
                    double tmp_x, tmp_y, tmp_z;
                    tmp_x = aTrack->GetStep()->GetPostStepPoint()->GetPosition().getX() / m;
                    tmp_y = aTrack->GetStep()->GetPostStepPoint()->GetPosition().getY() / m;
                    tmp_z = aTrack->GetStep()->GetPostStepPoint()->GetPosition().getZ() / m;
/*
          G4cout << "Got it!" << G4endl;
          G4cout << "Position: " << tmp_neutronXpos << " " << tmp_neutronYpos << " "
                 << tmp_neutronZpos << G4endl;
          G4cout << "Direction: " << tmp_neutronXmom << " " << tmp_neutronYmom << " "
                 << tmp_neutronZmom << G4endl;
          G4cout << "Energy: "
                 << aTrack->GetStep()->GetPreStepPoint()->GetKineticEnergy() / eV
                 << G4endl;
          G4cout << "Position of generated Ge-77: " << tmp_x << " " << tmp_y << " "
                 << tmp_z << G4endl;
*/
                    fEventAction->AddEkin(aTrack->GetStep()->GetPreStepPoint()->GetKineticEnergy() /
                                          eV);
                    fEventAction->AddNeutronxLoc(tmp_neutronXpos);
                    fEventAction->AddNeutronyLoc(tmp_neutronYpos);
                    fEventAction->AddNeutronzLoc(tmp_neutronZpos);
                    fEventAction->AddNeutronxMom(tmp_neutronXmom);
                    fEventAction->AddNeutronyMom(tmp_neutronYmom);
                    fEventAction->AddNeutronzMom(tmp_neutronZmom);
                    fEventAction->AddNeutronTime(tmp_neutronTime);
                    fEventAction->WriteMostOuterRadius();
                    fEventAction->AddIDListOfGe77SiblingParticles(aTrack->GetTrackID());
                }
                else
                {
                    if(aTrack->GetStep()
                               ->GetSecondaryInCurrentStep()
                               ->at(i)
                               ->GetParticleDefinition()
                               ->GetPDGCharge() == 18)
                    {
                        fEventAction->AddnCAr_timing(
                                aTrack->GetStep()->GetPostStepPoint()->GetGlobalTime() / s);
                        fEventAction->AddnCAr_x(
                                aTrack->GetStep()->GetPostStepPoint()->GetPosition().getX() / m);
                        fEventAction->AddnCAr_y(
                                aTrack->GetStep()->GetPostStepPoint()->GetPosition().getY() / m);
                        fEventAction->AddnCAr_z(
                                aTrack->GetStep()->GetPostStepPoint()->GetPosition().getZ() / m);
                        fEventAction->AddnCAr_A(aTrack->GetStep()
                                                        ->GetSecondaryInCurrentStep()
                                                        ->at(i)
                                                        ->GetParticleDefinition()
                                                        ->GetAtomicMass());
                    }
                    else
                    {
                        if(aTrack->GetStep()
                                   ->GetSecondaryInCurrentStep()
                                   ->at(i)
                                   ->GetParticleDefinition()
                                   ->GetPDGCharge() == 64)
                        {
                            fEventAction->AddnCGd_timing(
                                    aTrack->GetStep()->GetPostStepPoint()->GetGlobalTime() / s);
                            fEventAction->AddnCGd_x(
                                    aTrack->GetStep()->GetPostStepPoint()->GetPosition().getX() / m);
                            fEventAction->AddnCGd_y(
                                    aTrack->GetStep()->GetPostStepPoint()->GetPosition().getY() / m);
                            fEventAction->AddnCGd_z(
                                    aTrack->GetStep()->GetPostStepPoint()->GetPosition().getZ() / m);
                            fEventAction->AddnCGd_A(aTrack->GetStep()
                                                            ->GetSecondaryInCurrentStep()
                                                            ->at(i)
                                                            ->GetParticleDefinition()
                                                            ->GetAtomicMass());
                            fEventAction->AddIDListOfGdSiblingParticles(aTrack->GetTrackID());
                        }
                        else
                        {
                            fEventAction->AddnCOther_timing(
                                    aTrack->GetStep()->GetPostStepPoint()->GetGlobalTime() / s);
                            fEventAction->AddnCOther_x(
                                    aTrack->GetStep()->GetPostStepPoint()->GetPosition().getX() / m);
                            fEventAction->AddnCOther_y(
                                    aTrack->GetStep()->GetPostStepPoint()->GetPosition().getY() / m);
                            fEventAction->AddnCOther_z(
                                    aTrack->GetStep()->GetPostStepPoint()->GetPosition().getZ() / m);
                            fEventAction->AddnCOther_A(aTrack->GetStep()
                                                               ->GetSecondaryInCurrentStep()
                                                               ->at(i)
                                                               ->GetParticleDefinition()
                                                               ->GetAtomicMass());
                            fEventAction->AddnCOther_ZC(aTrack->GetStep()
                                                                ->GetSecondaryInCurrentStep()
                                                                ->at(i)
                                                                ->GetParticleDefinition()
                                                                ->GetPDGCharge());
                        }
                    }
                }
            }
        }
    }
}
