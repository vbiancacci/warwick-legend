#ifndef WLGDTrackingAction_h
#define WLGDTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "WLGDEventAction.hh"

class WLGDTrackingAction : public G4UserTrackingAction
{
public:
  WLGDTrackingAction();
  WLGDTrackingAction(WLGDEventAction* event){fEventAction = event; WLGDTrackingAction();}
  virtual ~WLGDTrackingAction(){};

  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);
private:
  double tmp_neutronXpos, tmp_neutronYpos, tmp_neutronZpos;
  WLGDEventAction* fEventAction;
};

#endif
