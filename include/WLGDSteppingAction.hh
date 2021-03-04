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
  WLGDSteppingAction(WLGDRunAction* run){fRunAction = run; WLGDSteppingAction();}
  virtual ~WLGDSteppingAction() = default;

  virtual void UserSteppingAction(const G4Step*);

private:
  WLGDRunAction* fRunAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

