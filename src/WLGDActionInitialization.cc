#include "WLGDActionInitialization.hh"
#include "WLGDEventAction.hh"
#include "WLGDPrimaryGeneratorAction.hh"
#include "WLGDRunAction.hh"
#include "WLGDStackingAction.hh"
#include "WLGDSteppingAction.hh"
#include "WLGDTrackingAction.hh"

WLGDActionInitialization::WLGDActionInitialization(WLGDDetectorConstruction* det,
                                                   G4String                  name)
: G4VUserActionInitialization()
, fDet(det)
, foutname(std::move(name))
{}

WLGDActionInitialization::~WLGDActionInitialization() = default;

void WLGDActionInitialization::BuildForMaster() const
{
  auto event = new WLGDEventAction;
  auto prim = new WLGDPrimaryGeneratorAction(fDet);
  SetUserAction(new WLGDRunAction(event, foutname, prim));
}

void WLGDActionInitialization::Build() const
{
  // forward detector
  auto prim = new WLGDPrimaryGeneratorAction(fDet);
  SetUserAction(prim);
  auto event = new WLGDEventAction();
  SetUserAction(event);
  auto run = new WLGDRunAction(event, foutname, prim);
  SetUserAction(run);
  SetUserAction(new WLGDTrackingAction(event,run));
  SetUserAction(new WLGDSteppingAction(run));
  SetUserAction(new WLGDStackingAction);
}
