#ifndef WLGDRunAction_h
#define WLGDRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class WLGDEventAction;
class G4Run;

#include <fstream>
#include <vector>
#include <iostream>
using namespace std;
/// Run action class
///

class WLGDRunAction : public G4UserRunAction
{
public:
  WLGDRunAction(WLGDEventAction* eventAction, G4String name);
  virtual ~WLGDRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  void increaseNumberOfCrossingNeutrons(){fNumberOfCrossingNeutrons++;}
  void increaseTotalNumberOfNeutronsInLAr(){fTotalNumberOfNeutronsInLAr++;}
  G4int getNumberOfNeutronsInLAr(){return fTotalNumberOfNeutronsInLAr;}
  void addCoordinatsToFile(G4double x, G4double y, G4double z){vector_x_dir.push_back(x);vector_y_dir.push_back(y);vector_z_dir.push_back(z);}//{outputStream_2 << x << " " << y << " " << z << endl;}
  void addMomentumToFile(G4double x, G4double y, G4double z){vector_x_mom.push_back(x);vector_y_mom.push_back(y);vector_z_mom.push_back(z);}//{outputStream_2 << x << " " << y << " " << z << endl;}
  void addEnergyToFile(G4double ekin){vector_energy.push_back(ekin);}//{outputStream_2 << x << " " << y << " " << z << endl;}
  void addParentParticleType(G4int PDG_code){vector_parentParticleType.push_back(PDG_code);}

private:
  WLGDEventAction* fEventAction;  // have event information for run
  G4String         fout;          // output file name
  G4int            fNumberOfCrossingNeutrons;
  G4int            fTotalNumberOfNeutronsInLAr;
  G4String         fout2, fout3;
  ofstream         outputStream_2;
  std::vector<G4double> vector_x_dir;
  std::vector<G4double> vector_y_dir;
  std::vector<G4double> vector_z_dir;
  std::vector<G4double> vector_x_mom;
  std::vector<G4double> vector_y_mom;
  std::vector<G4double> vector_z_mom;
  std::vector<G4double> vector_energy;
  std::vector<G4int> vector_parentParticleType;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
