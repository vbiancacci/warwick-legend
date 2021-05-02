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
  analysisManager->CreateNtupleDColumn("TotalEdepInLAr", fEventAction->GetLArEnergyDeposition());

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

