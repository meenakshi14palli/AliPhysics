#ifndef ALIANALYSISTASKGAMMACONVERSION_H
#define ALIANALYSISTASKGAMMACONVERSION_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//--------------------------------------------- 
// Class used to do analysis on conversion pairs
//---------------------------------------------
////////////////////////////////////////////////
 
#include "AliAnalysisTaskSE.h"
#include <vector>
#include "AliV0Reader.h"
#include "AliGammaConversionAODObject.h"

class TNtuple;
class AliGammaConversionHistograms;
class AliESDv0;
class AliKFParticle;
class AliESDInputHandler;
class AliESDEvent;
class AliAODEvent;
class TList;
class AliStack;
class AliESDtrackCuts;


class AliAnalysisTaskGammaConversion : public AliAnalysisTaskSE
{
 public:
  AliAnalysisTaskGammaConversion();
  AliAnalysisTaskGammaConversion(const char* name);
  virtual ~AliAnalysisTaskGammaConversion() ;// virtual destructor
 
  // Implementation of interface methods
  virtual void UserCreateOutputObjects();
  virtual void Init();
  virtual void LocalInit() {Init();}
  virtual void Exec(Option_t *option);
  virtual void Terminate(Option_t *option);
  virtual void ConnectInputData(Option_t *);
	
  void ProcessMCData();
  void ProcessV0sNoCut();
  void ProcessV0s();
  void ProcessGammasForNeutralMesonAnalysis();

  // AOD
  TString GetAODBranchName() const {return  fAODBranchName;}
  void SetAODBranchName(TString name)  {fAODBranchName = name ;}	
  void FillAODWithConversionGammas();
  // end AOD


 // for GammaJetAnalysis
  void ProcessGammasForGammaJetAnalysis();
  void CreateListOfChargedParticles();
  Double_t GetMinimumDistanceToCharge(Int_t indexHighestPtGamma);
  void CalculateJetCone(Int_t gammaIndex);
  Int_t GetIndexHighestPtGamma();
  void SetESDtrackCuts();
  // end of Gamma Jet

  void SetMinPtForGammaJet(Double_t minPtForGammaJet){fMinPtForGammaJet=minPtForGammaJet;}
  void SetMinIsoConeSize(Double_t minIsoConeSize){fMinIsoConeSize=minIsoConeSize;}
  void SetMinPtIsoCone(Double_t minPtIsoCone){fMinPtIsoCone=minPtIsoCone;}
  void SetMinPtGamChargedCorr(Double_t minPtGamChargedCorr){fMinPtGamChargedCorr=minPtGamChargedCorr;}
  void SetMinPtJetCone(Double_t minPtJetCone){fMinPtJetCone=minPtJetCone;}

  void SetHistograms(AliGammaConversionHistograms *const histograms){fHistograms=histograms;}
  void SetDoMCTruth(Bool_t flag){fDoMCTruth=flag;}
  void SetDoNeutralMeson(Bool_t flag){fDoNeutralMeson=flag;}
  void SetDoJet(Bool_t flag){fDoJet=flag;}
  void SetDoChic(Bool_t flag){fDoChic=flag;}

  void SetElectronMass(Double_t electronMass){fElectronMass = electronMass;}
  void SetGammaMass(Double_t gammaMass){fGammaMass = gammaMass;}
  void SetGammaWidth(Double_t gammaWidth){fGammaWidth = gammaWidth;}
  void SetPi0Mass(Double_t pi0Mass){fPi0Mass = pi0Mass;}
  void SetPi0Width(Double_t pi0Width){fPi0Width = pi0Width;}
  void SetEtaMass(Double_t etaMass){fEtaMass = etaMass;}
  void SetEtaWidth(Double_t etaWidth){fEtaWidth = etaWidth;}
  void SetMinOpeningAngleGhostCut(Double_t ghostCut){fMinOpeningAngleGhostCut = ghostCut;}
  void SetV0Reader(AliV0Reader* const reader){fV0Reader=reader;}
  void SetCalculateBackground(Bool_t bg){fCalculateBackground=bg;}
  void CalculateBackground();
  void SetWriteNtuple(Bool_t writeNtuple){fWriteNtuple = writeNtuple;}
  void FillNtuple();
  Double_t GetMCOpeningAngle(TParticle* const daughter0, TParticle* const daughter1) const;
  void CheckV0Efficiency();


  //////////////////Chi_c Analysis////////////////////////////
  void GetPID(AliESDtrack *track, Stat_t &pid, Stat_t &weight);	
  double GetSigmaToVertex(AliESDtrack* t);
  void ElectronBackground(TString hBg, TClonesArray e);
  void FillAngle(TString histoName,TClonesArray const tlVeNeg, TClonesArray const tlVePos);
  void FillElectronInvMass(TString histoName, TClonesArray const negativeElectron, TClonesArray const positiveElectron);
  void FillGammaElectronInvMass(TString histoMass,TString histoDiff, TClonesArray const fKFGammas, TClonesArray const tlVeNeg,TClonesArray const tlVePos);
  void CleanWithAngleCuts(TClonesArray const negativeElectrons, TClonesArray const positiveElectrons, TClonesArray const gammas);
  TClonesArray GetTLorentzVector(TClonesArray* esdTrack);	
  void ProcessGammaElectronsForChicAnalysis();
  ///////////////////////////////////////////////////////////////


 private:
  AliAnalysisTaskGammaConversion(const AliAnalysisTaskGammaConversion&); // Not implemented
  AliAnalysisTaskGammaConversion& operator=(const AliAnalysisTaskGammaConversion&); // Not implemented

  AliV0Reader* fV0Reader; // The V0 reader object 

  AliStack * fStack; // pointer to the MC particle stack
  AliESDEvent* fESDEvent; //pointer to the ESDEvent
  TList * fOutputContainer ; // Histogram container

  AliGammaConversionHistograms *fHistograms; // Pointer to the histogram handling class

  Bool_t fDoMCTruth; // Flag to switch on/off MC truth 
  Bool_t fDoNeutralMeson; // flag
  Bool_t fDoJet; // flag
  Bool_t fDoChic; // flag

  TClonesArray * fKFReconstructedGammasTClone; //! transient
  TClonesArray * fCurrentEventPosElectronTClone; //! transient
  TClonesArray * fCurrentEventNegElectronTClone; //! transient
  TClonesArray * fKFReconstructedGammasCutTClone; //! transient
  TClonesArray * fPreviousEventTLVNegElectronTClone; //! transient
  TClonesArray * fPreviousEventTLVPosElectronTClone; //! transient
  
  //  vector<AliKFParticle> fKFReconstructedGammas; // vector containing all reconstructed gammas
  vector<Int_t> fElectronv1; // vector containing index of electron 1
  vector<Int_t> fElectronv2; // vector containing index of electron 2

  ///////Chi_c Analysis///////////////////////////
  //  vector<AliESDtrack*> fCurrentEventPosElectron;       // comment here
  //  vector<AliESDtrack*> fCurrentEventNegElectron;       // comment here
  //  vector<AliKFParticle> fKFReconstructedGammasCut;     // comment here
  //  vector<TLorentzVector> fPreviousEventTLVNegElectron; // comment here
  //  vector<TLorentzVector> fPreviousEventTLVPosElectron; // comment here
  //////////////////////////////////////////////////	

  //mass defines
  Double_t fElectronMass; //electron mass
  Double_t fGammaMass;    //gamma mass
  Double_t fPi0Mass;      //pi0mass
  Double_t fEtaMass;      //eta mass

  // width defines
  Double_t fGammaWidth; //gamma width cut
  Double_t fPi0Width;   // pi0 width cut
  Double_t fEtaWidth;   // eta width cut

  Double_t fMinOpeningAngleGhostCut; // minimum angle cut

  AliESDtrackCuts* fEsdTrackCuts;           // Object containing the parameters of the esd track cuts

  Bool_t fCalculateBackground; //flag to set backgrount calculation on/off
  Bool_t fWriteNtuple;         // flag to set if writing to ntuple on/off
  TNtuple *fGammaNtuple;       // Ntuple for gamma values
  TNtuple *fNeutralMesonNtuple;// NTuple for mesons

  Int_t fTotalNumberOfAddedNtupleEntries; // number of added ntuple entries

  TClonesArray* fChargedParticles;  //! transient
  vector<Int_t> fChargedParticlesId;  //! transient

  Double_t fGammaPtHighest;  //! transient
  Double_t fMinPtForGammaJet;  //! transient
  Double_t fMinIsoConeSize; //! transient
  Double_t fMinPtIsoCone; //! transient
  Double_t fMinPtGamChargedCorr; //! transient
  Double_t fMinPtJetCone; //! transient
  Int_t    fLeadingChargedIndex; //! transient

  TClonesArray* fAODBranch ;        //! selected particles branch
  TString fAODBranchName; // New AOD branch name
  
  //  TClonesArray *fAODObjects;

  ClassDef(AliAnalysisTaskGammaConversion, 4); // Analysis task for gamma conversions
};
 
#endif //ALIANALYSISTASKGAMMA_H
