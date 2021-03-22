#include "WLGDRunAction.hh"
#include "WLGDEventAction.hh"
#include "WLGDPrimaryGeneratorAction.hh"
#include "g4root.hh"

#include <iostream>
#include <fstream>
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
using namespace std;

WLGDRunAction::WLGDRunAction(WLGDEventAction* eventAction, G4String name, WLGDPrimaryGeneratorAction* primAction)
: G4UserRunAction()
, fEventAction(eventAction)
, fout(std::move(name))
{

  ofstream outputStream;
  fout2 = fout + "_NCrossing.txt";
  outputStream.open(fout2.c_str(),ios::trunc);
  outputStream.close();


  fout3 = fout + "_NCreationPosition.txt";
  outputStream_2.open(fout3.c_str(),ios::trunc);

  // Create analysis manager
  auto analysisManager = G4AnalysisManager::Instance();



  primAction->ChangeFileName("./musun_gs_100M.dat");

  // Create directories
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);

  // Creating ntuple with vector entries
  //
  analysisManager->CreateNtuple("Score", "Hits");
  analysisManager->CreateNtupleDColumn("Edep", fEventAction->GetHitEdep());
  analysisManager->CreateNtupleDColumn("Ekin", fEventAction->GetHitEkin());
  analysisManager->CreateNtupleDColumn("Time", fEventAction->GetHitTime());
  analysisManager->CreateNtupleDColumn("Weight", fEventAction->GetHitWeight());
  analysisManager->CreateNtupleDColumn("Hitxloc", fEventAction->GetHitxLoc());
  analysisManager->CreateNtupleDColumn("Hityloc", fEventAction->GetHityLoc());
  analysisManager->CreateNtupleDColumn("Hitzloc", fEventAction->GetHitzLoc());
  analysisManager->CreateNtupleDColumn("Neutronxloc", fEventAction->GetNeutronxLoc());
  analysisManager->CreateNtupleDColumn("Neutronyloc", fEventAction->GetNeutronyLoc());
  analysisManager->CreateNtupleDColumn("Neutronzloc", fEventAction->GetNeutronzLoc());
  analysisManager->CreateNtupleDColumn("Neutronxmom", fEventAction->GetNeutronxMom());
  analysisManager->CreateNtupleDColumn("Neutronymom", fEventAction->GetNeutronyMom());
  analysisManager->CreateNtupleDColumn("Neutronzmom", fEventAction->GetNeutronzMom());

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

  ofstream outputStream;
  outputStream.open(fout2.c_str(),ios::app);
  outputStream << fNumberOfCrossingNeutrons << "   " << fTotalNumberOfNeutronsInLAr << endl;
  outputStream.close();

  for(int i = 0; i < vector_x_dir.size(); i++)
  {
    outputStream_2 << vector_x_dir[i] << " " << vector_y_dir[i] << " " << vector_z_dir[i] << " " << vector_x_mom[i] << " " << vector_y_mom[i] << " " << vector_z_mom[i] << " " << vector_energy[i] << endl;
  }
  outputStream_2.close();
}
