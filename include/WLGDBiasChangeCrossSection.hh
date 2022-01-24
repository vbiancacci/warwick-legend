#ifndef WLGDBiasChangeCrossSection_hh
#define WLGDBiasChangeCrossSection_hh 1

#include "G4VBiasingOperator.hh"
class G4BOptnChangeCrossSection;
class G4ParticleDefinition;
#include <map>

#include "G4BOptnChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "Randomize.hh"
#include "G4InteractionLawPhysical.hh"

class WLGDBiasChangeCrossSection : public G4VBiasingOperator
{
public:
  // ------------------------------------------------------------
  // -- Constructor: takes the name of the particle type to bias:
  // ------------------------------------------------------------
  WLGDBiasChangeCrossSection(G4String particleToBias, G4String name = "ChangeXS");
  virtual ~WLGDBiasChangeCrossSection();

  // -- method called at beginning of run:
  virtual void StartRun();
  void         SetNeutronFactor(G4double nf) { fNeutronBias = nf; }
  void         SetNeutronYieldFactor(G4double nf) { fNeutronYieldBias = nf; }
  void         SetMuonFactor(G4double mf) { fMuonBias = mf; }

private:
  // -----------------------------
  // -- Mandatory from base class:
  // -----------------------------
  // -- This method returns the biasing operation that will bias the physics process
  // occurence.
  virtual G4VBiasingOperation* ProposeOccurenceBiasingOperation(
    const G4Track* track, const G4BiasingProcessInterface* callingProcess);
  // -- Methods not used:
  virtual G4VBiasingOperation* ProposeFinalStateBiasingOperation(
    const G4Track*, const G4BiasingProcessInterface*)
  {
    return 0;
  }
  virtual G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(
    const G4Track*, const G4BiasingProcessInterface*)
  {
    return 0;
  }

private:
  // -- ("using" is avoid compiler complaining against (false) method shadowing.)
  using G4VBiasingOperator::OperationApplied;

  // -- Optionnal base class method implementation.
  // -- This method is called to inform the operator that a proposed operation has been
  // applied.
  // -- In the present case, it means that a physical interaction occured (interaction
  // at
  // -- PostStepDoIt level):
  virtual void OperationApplied(const G4BiasingProcessInterface* callingProcess,
                                G4BiasingAppliedCase             biasingCase,
                                G4VBiasingOperation*     occurenceOperationApplied,
                                G4double                 weightForOccurenceInteraction,
                                G4VBiasingOperation*     finalStateOperationApplied,
                                const G4VParticleChange* particleChangeProduced);

private:
  // -- List of associations between processes and biasing operations:
  std::map<const G4BiasingProcessInterface*, G4BOptnChangeCrossSection*>
                              fChangeCrossSectionOperations;
  G4bool                      fSetup;
  const G4ParticleDefinition* fParticleToBias;
  G4String                    fpname;

  // -- Scalar to manually adjust the cross-sections of the neutron capture in Ge
  G4double                    fNeutronBias      = 1.0;

  // -- Scalar to manually adjust the cross-sections of the muon
  G4double                    fMuonBias         = 1.0;
  
  // -- Experimental: Scalar to manually adjust all the cross-sections acting into the neutron yield
  G4double                    fNeutronYieldBias = 1.0;

};
#endif
