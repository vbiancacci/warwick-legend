//
// Created by moritz on 3/4/21.
//

#ifndef WARWICK_LEGEND_WLGDSTEPPINGACTION_HH
#define WARWICK_LEGEND_WLGDSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "WLGDRunAction.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

class WLGDTrackingAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class WLGDSteppingAction : public G4UserSteppingAction
{
public:
  WLGDSteppingAction() = default;
  WLGDSteppingAction(WLGDEventAction* event,WLGDRunAction* run){fEventAction = event; fRunAction = run; WLGDSteppingAction(); DefineCommands();}
  virtual ~WLGDSteppingAction() = default;

  virtual void UserSteppingAction(const G4Step*);
  void GetDepositionInfo(G4int answer);

private:
  WLGDRunAction* fRunAction;
  WLGDEventAction* fEventAction;
  G4GenericMessenger*        fStepMessenger;
  void DefineCommands();
  G4int fDepositionInfo = 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

