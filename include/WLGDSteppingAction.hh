//
// Created by moritz on 3/4/21.
//

#ifndef WARWICK_LEGEND_WLGDSTEPPINGACTION_HH
#define WARWICK_LEGEND_WLGDSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class WLGDTrackingAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class WLGDSteppingAction : public G4UserSteppingAction
{
public:
  WLGDSteppingAction() = default;
  WLGDSteppingAction(WLGDTrackingAction* track){fTrackingAction = track; WLGDSteppingAction();}
  ~WLGDSteppingAction() = default;

  virtual void UserSteppingAction(const G4Step*);

private:
  WLGDTrackingAction* fTrackingAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

