#include "WLGDEventAction.hh"
#include "WLGDTrajectory.hh"
#include "g4root.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

#include "WLGDCrystalSD.hh"

#include "Randomize.hh"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <vector>

WLGDEventAction::WLGDEventAction(){
  DefineCommands();
}

WLGDCrystalHitsCollection* WLGDEventAction::GetHitsCollection(G4int hcID,
                                              const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<WLGDCrystalHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("WLGDEventAction::GetHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    


void WLGDEventAction::makeMap()
{
  // make a lookup table to translate Logical Volume name
  // to a unique int for storage in the ntuple.
  lookup["Cavern_log"]   = 0;
  lookup["Hall_log"]     = 1;
  lookup["Tank_log"]     = 2;
  lookup["Water_log"]    = 3;
  lookup["Cout_log"]     = 4;
  lookup["Cvac_log"]     = 5;
  lookup["Cinn_log"]     = 6;
  lookup["Lar_log"]      = 7;
  lookup["Lid_log"]      = 8;
  lookup["Bot_log"]      = 9;
  lookup["Copper_log"]   = 10;
  lookup["ULar_log"]     = 11;
  lookup["Ge_log"]       = 12;
  lookup["Pu_log"]       = 13;
  lookup["Membrane_log"] = 14;
}

G4int WLGDEventAction::GeomID(G4String name)
{
  auto it = lookup.find(name);

  if(it == lookup.end())
  {
    G4ExceptionDescription msg;
    msg << "Name  " << name << " Inot in look up table";
    G4Exception("WLGDEventAction::GeomID()", "MyCode0005", FatalException, msg);
  }
  return it->second;
}

void WLGDEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{
  nGe77.clear();
  edep.clear();
  ekin.clear();
  htrid.clear();
  thit.clear();
  whit.clear();
  xloc.clear();
  yloc.clear();
  zloc.clear();
  neutronxloc.clear();
  neutronyloc.clear();
  neutronzloc.clear();
  neutronxmom.clear();
  neutronymom.clear();
  neutronzmom.clear();
  // clear trajectory data
  trjpdg.clear();
  trjnpts.clear();
  nameid.clear();
  trjxvtx.clear();
  trjyvtx.clear();
  trjzvtx.clear();
  trjxpos.clear();
  trjypos.clear();
  trjzpos.clear();

  ReentranceTube.clear();

  Multiplicity_prompt.clear();
  Multiplicity_prompt.push_back(0);
  Multiplicity_prompt.push_back(0);
  Multiplicity_prompt.push_back(0);
  Multiplicity_prompt.push_back(0);

  Multiplicity_delayed.clear();
  Multiplicity_delayed.push_back(0);
  Multiplicity_delayed.push_back(0);
  Multiplicity_delayed.push_back(0);
  Multiplicity_delayed.push_back(0);

  v_EdepPerDetector_prompt.clear();
  v_EdepPerDetector_prompt.push_back(0);
  v_EdepPerDetector_prompt.push_back(0);
  v_EdepPerDetector_prompt.push_back(0);
  v_EdepPerDetector_prompt.push_back(0);

  v_EdepPerDetector_delayed.clear();
  v_EdepPerDetector_delayed.push_back(0);
  v_EdepPerDetector_delayed.push_back(0);
  v_EdepPerDetector_delayed.push_back(0);
  v_EdepPerDetector_delayed.push_back(0);

  NumberOfNeutronsProducedInEvent.clear();
  NumberOfNeutronsProducedInEvent.push_back(0);

  TotalEnergyDepositionInLAr_prompt.clear();
  TotalEnergyDepositionInLAr_prompt.push_back(0);
  TotalEnergyDepositionInLAr_prompt.push_back(0);
  TotalEnergyDepositionInLAr_prompt.push_back(0);
  TotalEnergyDepositionInLAr_prompt.push_back(0);

  TotalEnergyDepositionInGe_prompt.clear();
  TotalEnergyDepositionInGe_prompt.push_back(0);
  TotalEnergyDepositionInGe_prompt.push_back(0);
  TotalEnergyDepositionInGe_prompt.push_back(0);
  TotalEnergyDepositionInGe_prompt.push_back(0);

  TotalEnergyDepositionInLAr_delayed.clear();
  TotalEnergyDepositionInLAr_delayed.push_back(0);
  TotalEnergyDepositionInLAr_delayed.push_back(0);
  TotalEnergyDepositionInLAr_delayed.push_back(0);
  TotalEnergyDepositionInLAr_delayed.push_back(0);

  TotalEnergyDepositionInGe_delayed.clear();
  TotalEnergyDepositionInGe_delayed.push_back(0);
  TotalEnergyDepositionInGe_delayed.push_back(0);
  TotalEnergyDepositionInGe_delayed.push_back(0);
  TotalEnergyDepositionInGe_delayed.push_back(0);

  IndividualEnergyDeposition_Timing.clear();
  IndividualEnergyDeposition_LArOrGe.clear();
  IndividualEnergyDeposition_Energy.clear();
  IndividualEnergyDeposition_Position_x.clear();
  IndividualEnergyDeposition_Position_y.clear();
  IndividualEnergyDeposition_Position_z.clear();
  IndividualEnergyDeposition_ReentranceTube.clear();

  EdepPerDetector.clear();

  makeMap();
}

void WLGDEventAction::EndOfEventAction(const G4Event* event)
{
  // Get crystal hits collections IDs
  if(fHID < 0)
    fHID   = G4SDManager::GetSDMpointer()->GetCollectionID("CrystalHitsCollection");


  // Get entries from hits collections
  //
  auto CrysHC   = GetHitsCollection(fHID, event);

  nGe77.push_back(ekin.size());

  if(CrysHC->entries() <= 0 && fAllEvents==0)
  {
    return;  // no action on no hit
  }




  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int nofHits = CrysHC->entries();

  G4cout << "ReentranceTube: ";
  for ( G4int i=0; i<nofHits; i++ ) 
  {
    auto hh = (*CrysHC)[i];

    htrid.push_back(hh->GetTID());
    whit.push_back(hh->GetWeight());
    thit.push_back(hh->GetTime()  / G4Analysis::GetUnitValue("ns"));
    edep.push_back(hh->GetEdep()  / G4Analysis::GetUnitValue("MeV"));
    xloc.push_back((hh->GetPos()).x() / G4Analysis::GetUnitValue("m"));
    yloc.push_back((hh->GetPos()).y() / G4Analysis::GetUnitValue("m"));
    zloc.push_back((hh->GetPos()).z() / G4Analysis::GetUnitValue("m"));
    ReentranceTube.push_back(hh->GetWhichReentranceTube());
  }
  G4cout << G4endl;

  for(auto const& x : EdepPerDetector){
    int tmp_i = (int)(x.first/96);
    Multiplicity_prompt[tmp_i] += 1;
    v_NDetector_prompt.push_back(x.first);
    v_EdepPerDetector_prompt.push_back(x.second);
  }

  for(auto const& x : EdepPerDetector_delayed){
    int tmp_i = (int)(x.first/96);
    Multiplicity_delayed[tmp_i] += 1;
    v_NDetector_delayed.push_back(x.first);
    v_EdepPerDetector_delayed.push_back(x.second);
  }

  // fill trajectory data if available
  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int                  n_trajectories =
    (trajectoryContainer == nullptr) ? 0 : trajectoryContainer->entries();

  if(n_trajectories > 0)
  {
    // temporary full storage
    std::vector<G4int>    temptid, temppid, temppdg, tempnpts;
    std::vector<G4String> tempname;
    std::vector<G4double> tempxvtx, tempyvtx, tempzvtx;
    std::vector<G4double> tempxpos, tempypos, tempzpos;

    for(G4int i = 0; i < n_trajectories; i++)
    {
      WLGDTrajectory* trj = (WLGDTrajectory*) ((*(event->GetTrajectoryContainer()))[i]);
      temptid.push_back(trj->GetTrackID());
      temppid.push_back(trj->GetParentID());
      temppdg.push_back(trj->GetPDGEncoding());
      tempname.push_back(trj->GetVertexName());
      tempxvtx.push_back((trj->GetVertex()).x());
      tempyvtx.push_back((trj->GetVertex()).y());
      tempzvtx.push_back((trj->GetVertex()).z());
      tempnpts.push_back(trj->GetPointEntries());
      for(int nn = 0; nn < trj->GetPointEntries(); ++nn)
      {
        tempxpos.push_back((trj->GetPoint(nn)->GetPosition()).x());
        tempypos.push_back((trj->GetPoint(nn)->GetPosition()).y());
        tempzpos.push_back((trj->GetPoint(nn)->GetPosition()).z());
      }
    }

    // store filtered trajectories only
    for(const int& item : htrid)
    {
      std::vector<int> res = FilterTrajectories(item, temptid, temppid);
      for(int& idx : res)
      {
        trjpdg.push_back(temppdg.at(idx));
        nameid.push_back(GeomID(tempname.at(idx)));
        trjxvtx.push_back(tempxvtx.at(idx));
        trjyvtx.push_back(tempyvtx.at(idx));
        trjzvtx.push_back(tempzvtx.at(idx));
        trjnpts.push_back(tempnpts.at(idx));
        int start = std::accumulate(tempnpts.begin(), tempnpts.begin() + idx, 0);
        for(int i = start; i < (start + tempnpts.at(idx)); ++i)
        {
          trjxpos.push_back(tempxpos.at(i));
          trjypos.push_back(tempypos.at(i));
          trjzpos.push_back(tempzpos.at(i));
        }
      }
    }
    temptid.clear();
    temppid.clear();
    temppdg.clear();
    tempnpts.clear();
    tempname.clear();
    tempxvtx.clear();
    tempyvtx.clear();
    tempzvtx.clear();
    tempxpos.clear();
    tempypos.clear();
    tempzpos.clear();
  }
  // fill the ntuple
  analysisManager->AddNtupleRow();

  // printing
  G4int eventID = event->GetEventID();
 /* G4cout << ">>> Event: " << eventID << G4endl;
  G4cout << "    " << edep.size() << " hits stored in this event." << G4endl;
  G4cout << "    " << trjpdg.size() << " trajectories stored in this event." << G4endl;*/
}

void WLGDEventAction::SaveAllEvents(G4int answer){fAllEvents = answer;}

void WLGDEventAction::DefineCommands()
{
  // Define geometry command directory using generic messenger class
  fEventMessenger = new G4GenericMessenger(this, "/WLGD/event/",
                                              "Commands for controlling event action");

  // switch command
  fEventMessenger->DeclareMethod("saveAllEvents", &WLGDEventAction::SaveAllEvents)
    .SetGuidance("Set whether to save not only Ge77 but all events")
    .SetGuidance("0 = only Ge77 events")
    .SetGuidance("1 = all events")
    .SetCandidates("0 1")
    .SetDefaultValue("0");
}