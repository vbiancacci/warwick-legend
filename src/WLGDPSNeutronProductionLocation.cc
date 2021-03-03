//
// Created by moritz on 3/3/21.
//

// WLGDPSNeutronProductionLocation
#include "WLGDPSNeutronProductionLocation.h"
#include "G4UnitsTable.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring the production position of the neutron producing the Ge77
//
///////////////////////////////////////////////////////////////////////////////

WLGDPSNeutronProductionLocation::WLGDPSNeutronProductionLocation(G4String name, G4int depth)
  : G4VPrimitiveScorer(std::move(name), depth)
  , HCID(-1)
  , NeuPosMap(nullptr)
{
  SetUnit("m");
}

WLGDPSNeutronProductionLocation::WLGDPSNeutronProductionLocation(G4String name, const G4String& unit, G4int depth)
  : G4VPrimitiveScorer(std::move(name), depth)
  , HCID(-1)
  , NeuPosMap(nullptr)
{
  SetUnit(unit);
}

WLGDPSNeutronProductionLocation::~WLGDPSNeutronProductionLocation() = default;

G4bool WLGDPSNeutronProductionLocation::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  if(aStep->GetTotalEnergyDeposit() <= 0.0)
    return false;

  G4int          index = GetIndex(aStep);
  G4TrackLogger& tlog  = fCellTrackLogger[index];
  if(tlog.FirstEnterance(aStep->GetTrack()->GetTrackID()))
  {
    G4StepPoint*  stepPoint = aStep->GetPreStepPoint();
    G4ThreeVector loc       = stepPoint->GetPosition();  // location at track creation

    NeuPosMap->add(index, loc);
  }
  return true;
}

void WLGDPSNeutronProductionLocation::Initialize(G4HCofThisEvent* HCE)
{
  NeuPosMap =
    new G4THitsMap<G4ThreeVector>(GetMultiFunctionalDetector()->GetName(), GetName());
  if(HCID < 0)
  {
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection(HCID, (G4VHitsCollection*) NeuPosMap);
}

void WLGDPSNeutronProductionLocation::EndOfEvent(G4HCofThisEvent* /*unused*/) { fCellTrackLogger.clear(); }

void WLGDPSNeutronProductionLocation::clear()
{
  fCellTrackLogger.clear();
  NeuPosMap->clear();
}

void WLGDPSNeutronProductionLocation::DrawAll() { ; }

void WLGDPSNeutronProductionLocation::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << NeuPosMap->entries() << G4endl;
  auto itr = NeuPosMap->GetMap()->begin();
  for(; itr != NeuPosMap->GetMap()->end(); itr++)
  {
    G4cout << "  key: " << itr->first << "  energy deposit at: ("
           << (*(itr->second)).x() / GetUnitValue() << ", "
           << (*(itr->second)).y() / GetUnitValue() << ", "
           << (*(itr->second)).z() / GetUnitValue() << ")"
           << " [" << GetUnit() << "]" << G4endl;
  }
}

void WLGDPSNeutronProductionLocation::SetUnit(const G4String& unit) { CheckAndSetUnit(unit, "Length"); }
