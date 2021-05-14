#include "WLGDRunAction.hh"
#include "WLGDEventAction.hh"
#include "g4root.hh"

#include <iostream>
#include <fstream>
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
using namespace std;

WLGDRunAction::WLGDRunAction(WLGDEventAction* eventAction, G4String name)
: G4UserRunAction()
, fEventAction(eventAction)
, fout(std::move(name))
{

  DefineCommands();

  // Create analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);

  // Creating ntuple with vector entries
  //
  analysisManager->CreateNtuple("Score", "Hits");
  analysisManager->CreateNtupleIColumn("NGe77", fEventAction->GetNGe77());
  analysisManager->CreateNtupleIColumn("ReentranceTube", fEventAction->GetReentranceTube());
  analysisManager->CreateNtupleIColumn("HitID", fEventAction->GetHitTID());
  analysisManager->CreateNtupleDColumn("Edep", fEventAction->GetHitEdep());
  analysisManager->CreateNtupleDColumn("Time", fEventAction->GetHitTime());
  analysisManager->CreateNtupleDColumn("Weight", fEventAction->GetHitWeight());
  analysisManager->CreateNtupleDColumn("Hitxloc", fEventAction->GetHitxLoc());
  analysisManager->CreateNtupleDColumn("Hityloc", fEventAction->GetHityLoc());
  analysisManager->CreateNtupleDColumn("Hitzloc", fEventAction->GetHitzLoc());

  // Edit: 2021/04/07 by Moritz Neuberger
  // Adding additional outputs to further investigate situations in which Ge-77 is produced
  analysisManager->CreateNtupleDColumn("Ekin", fEventAction->GetHitEkin());

  analysisManager->CreateNtupleDColumn("Neutronxloc", fEventAction->GetNeutronxLoc());
  analysisManager->CreateNtupleDColumn("Neutronyloc", fEventAction->GetNeutronyLoc());
  analysisManager->CreateNtupleDColumn("Neutronzloc", fEventAction->GetNeutronzLoc());
  analysisManager->CreateNtupleDColumn("Neutronxmom", fEventAction->GetNeutronxMom());
  analysisManager->CreateNtupleDColumn("Neutronymom", fEventAction->GetNeutronyMom());
  analysisManager->CreateNtupleDColumn("Neutronzmom", fEventAction->GetNeutronzMom());
  analysisManager->CreateNtupleIColumn("NeutronsInEvent", fEventAction->GetNumberOfNeutronsInEvent());

  analysisManager->CreateNtupleDColumn("LArEnergyDeposition", fEventAction->GetLArEnergyDeposition());
  analysisManager->CreateNtupleDColumn("GeEnergyDeposition", fEventAction->GetGeEnergyDeposition());
  analysisManager->CreateNtupleDColumn("LArEnergyDeposition_delayed", fEventAction->GetLArEnergyDeposition_delayed());
  analysisManager->CreateNtupleDColumn("GeEnergyDeposition_delayed", fEventAction->GetGeEnergyDeposition_delayed());
  analysisManager->CreateNtupleDColumn("LArEnergyDeposition_delayed_long", fEventAction->GetLArEnergyDeposition_delayed_long());
  analysisManager->CreateNtupleDColumn("GeEnergyDeposition_delayed_long", fEventAction->GetGeEnergyDeposition_delayed_long());
  analysisManager->CreateNtupleDColumn("LArEnergyDeposition_after_delayed", fEventAction->GetLArEnergyDeposition_after_delayed());
  analysisManager->CreateNtupleDColumn("GeEnergyDeposition_after_delayed", fEventAction->GetGeEnergyDeposition_after_delayed());

  analysisManager->CreateNtupleDColumn("IndividualEnergyDeposition_Timing", fEventAction->GetIndividualEnergyDeposition_Timing());
  analysisManager->CreateNtupleDColumn("IndividualEnergyDeposition_Energy", fEventAction->GetIndividualEnergyDeposition_Energy());
  analysisManager->CreateNtupleDColumn("IndividualEnergyDeposition_Position_x", fEventAction->GetIndividualEnergyDeposition_Position_x());
  analysisManager->CreateNtupleDColumn("IndividualEnergyDeposition_Position_y", fEventAction->GetIndividualEnergyDeposition_Position_y());
  analysisManager->CreateNtupleDColumn("IndividualEnergyDeposition_Position_z", fEventAction->GetIndividualEnergyDeposition_Position_z());
  analysisManager->CreateNtupleIColumn("IndividualEnergyDeposition_ReentranceTube", fEventAction->GetIndividualEnergyDeposition_ReentranceTube());
  analysisManager->CreateNtupleIColumn("IndividualEnergyDeposition_LArOrGe", fEventAction->GetIndividualEnergyDeposition_LArOrGe());

  analysisManager->CreateNtupleIColumn("Multiplicity_prompt", fEventAction->GetMultiplicity_prompt());
  analysisManager->CreateNtupleIColumn("Multiplicity_delayed", fEventAction->GetMultiplicity_delayed());
  analysisManager->CreateNtupleIColumn("Multiplicity_delayed_long", fEventAction->GetMultiplicity_delayed_long());
  analysisManager->CreateNtupleDColumn("EdepPerDetector_prompt", fEventAction->GetEdepPerDetector_prompt());
  analysisManager->CreateNtupleDColumn("EdepPerDetector_delayed", fEventAction->GetEdepPerDetector_delayed());
  analysisManager->CreateNtupleDColumn("CopyNDetector_prompt", fEventAction->GetNDetector_prompt());
  analysisManager->CreateNtupleDColumn("CopyNDetector_delayed", fEventAction->GetNDetector_delayed());

  analysisManager->CreateNtupleIColumn("Trjpdg", fEventAction->GetTrjPDG());
  analysisManager->CreateNtupleIColumn("Trjentries", fEventAction->GetTrjEntries());
  analysisManager->CreateNtupleIColumn("VtxName", fEventAction->GetNameID());
  analysisManager->CreateNtupleDColumn("TrjXVtx", fEventAction->GetTrjXVtx());
  analysisManager->CreateNtupleDColumn("TrjYVtx", fEventAction->GetTrjYVtx());
  analysisManager->CreateNtupleDColumn("TrjZVtx", fEventAction->GetTrjZVtx());
  analysisManager->CreateNtupleDColumn("TrjXPos", fEventAction->GetTrjXPos());
  analysisManager->CreateNtupleDColumn("TrjYPos", fEventAction->GetTrjYPos());
  analysisManager->CreateNtupleDColumn("TrjZPos", fEventAction->GetTrjZPos());

  analysisManager->FinishNtuple();
}

WLGDRunAction::~WLGDRunAction() { delete G4AnalysisManager::Instance(); }

void WLGDRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  // Open an output file
  //
  analysisManager->OpenFile(fout);

  fNumberOfCrossingNeutrons = 0;
  fTotalNumberOfNeutronsInLAr = 0;

  ofstream outputStream;
  if(fWriteOutGeneralNeutronInfo == 1)
  {
    fout2 = fout + "_NCrossing.txt";
    outputStream.open(fout2.c_str(), ios::trunc);
    outputStream.close();
  }

  if(fWriteOutNeutronProductionInfo == 1)
  {
    fout3 = fout + "_NCreationPosition.txt";
    outputStream_2.open(fout3.c_str(), ios::trunc);
  }
}

void WLGDRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  G4cout << "NumberOfNeutronCrossings: " << fNumberOfCrossingNeutrons << G4endl;
  G4cout << "TotalNumberOfNeutronInLAr: " << fTotalNumberOfNeutronsInLAr << G4endl;

  // save ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();

  // Edit: 2021/03/12 by Moritz Neuberger
  // Adding output for number of neutrons crossing the detectors and total produced in LAr
  if(fWriteOutGeneralNeutronInfo == 1)
  {
    ofstream outputStream;
    outputStream.open(fout2.c_str(), ios::app);
    outputStream << fNumberOfCrossingNeutrons << "   " << fTotalNumberOfNeutronsInLAr
                 << endl;
    outputStream.close();
  }

  // Edit: 2021/03/12 by Moritz Neuberger
  // Adding detail output for neutron production information
  if(fWriteOutNeutronProductionInfo == 1){
    for(int i = 0; i < vector_x_dir.size(); i++)
    {
      outputStream_2 << vector_x_dir[i] << " " << vector_y_dir[i] << " " << vector_z_dir[i] << " " << vector_x_mom[i] << " " << vector_y_mom[i] << " " << vector_z_mom[i] << " " << vector_energy[i] << " " << vector_parentParticleType[i] << endl;
    }
    outputStream_2.close();
  }
}

void WLGDRunAction::SetWriteOutNeutronProductionInfo(G4int answer){ fWriteOutNeutronProductionInfo = answer; }

void WLGDRunAction::SetWriteOutGeneralNeutronInfo(G4int answer){ fWriteOutGeneralNeutronInfo = answer; }

void WLGDRunAction::DefineCommands()
{
  // Define /WLGD/generator command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/WLGD/runaction/", "Run Action control");

  fMessenger
    ->DeclareMethod("WriteOutNeutronProductionInfo", &WLGDRunAction::SetWriteOutNeutronProductionInfo)
    .SetGuidance("Set whether to write out Neutron Production Info")
    .SetGuidance("0 = without write out")
    .SetGuidance("1 = with write out")
    .SetCandidates("0 1")
    .SetDefaultValue("0");

  fMessenger
    ->DeclareMethod("WriteOutGeneralNeutronInfo", &WLGDRunAction::SetWriteOutGeneralNeutronInfo)
    .SetGuidance("Set whether to write out General Neutron Info")
    .SetGuidance("0 = without write out")
    .SetGuidance("1 = with write out")
    .SetCandidates("0 1")
    .SetDefaultValue("0");

}

