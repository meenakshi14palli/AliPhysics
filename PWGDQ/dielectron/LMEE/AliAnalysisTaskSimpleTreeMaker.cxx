/*************** Tree Maker Class **********************
*                                                      *
* Created: 05.10.2016                                  *
* Authors: Aaron Capon      (aaron.capon@cern.ch)      *
*          Sebastian Lehner (sebastian.lehner@cern.ch) *
*                                                      *
*******************************************************/
/**********************************************************************************
*This analysis task is designed to create simple flat structured TTrees which     *
*can then be worked on locally. The motivation for this was to create TTrees that *
*could be easily used within TMVA.                                                *
*                                                                                 *
*The default track cuts are relatively loose, and the PID selects out electrons   *
*within +-4 nSigma in the TPC. This value, as well as cuts on the other detector  *
*response values, can be turned on and off via their relavent setter functions.   *
*                                                                                 *
*The GRID PID number for each job is stored with each event along with a simple   *
*event counter ID, so that after merging each event still has a unique label.     *
*                                                                                 *
*By default, the class will return a TTree focused on selecting electrons from    *
*primary decays. There are setter functions which can be used to instead create a *
*TTree filled with tracks originating from V0 decays. In this case the DCA cuts   *
*are removed.                                                                     *
**********************************************************************************/
#include "Riostream.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"

#include "AliInputEventHandler.h"
#include "AliPIDResponse.h"

#include "AliESDInputHandler.h"
#include "AliAODInputHandler.h"

#include "AliVEvent.h"

#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"

#include "AliMCEventHandler.h"
#include "AliMCEvent.h"

#include "AliAODMCParticle.h"

#include "TParticle.h"
#include "AliTrackReference.h"
#include "AliHeader.h"
#include "AliGenEventHeader.h"
#include "AliGenHijingEventHeader.h"
#include "AliGenCocktailEventHeader.h"

#include "AliVVertex.h"

#include "AliESDv0.h"
#include "AliDielectronTrackCuts.h"
#include "AliDielectronVarManager.h"
#include "AliDielectronV0Cuts.h"

#include "TClonesArray.h"
#include "AliCentrality.h"
#include "AliMultSelection.h"

#include "TSystem.h"
#include <string>
#include "AliAnalysisTaskSimpleTreeMaker.h"



ClassImp(AliAnalysisTaskSimpleTreeMaker)

Int_t eventNum = 0;

AliAnalysisTaskSimpleTreeMaker::AliAnalysisTaskSimpleTreeMaker():
    AliAnalysisTaskSE(),
		hasMC(kFALSE),
    fESDtrackCuts(0),
    fPIDResponse(0),
    fTree(0x0),
		eventCuts(0),
		eventFilter(0),
		varCuts(0),
		trackCuts(0),
		pidCuts(0),
		cuts(0),
		trackFilter(0),
		varManager(0),
		primaryVertex{0,0,0},
		multiplicityV0A(0),
		multiplicityV0C(0),
		multiplicityCL1(0),
		runNumber(0),
		event(0),
		pt(0),
		eta(0),
		phi(0),
		nTPCclusters(0),
		nTPCcrossed(0),
		fTPCcrossOverFind(0),
		nTPCfindable(0),
		tpcSharedMap(0),
		nTPCshared(0),
		chi2TPC(0),
		DCA{0,0},
		nITS(0),
		chi2ITS(0),
		fITSshared(0),
		SPDfirst(0),
		charge(0),
		EnSigmaITS(0),
		EnSigmaTPC(0),
		EnSigmaTPCcorr(0),
		EnSigmaTOF(0),
		PnSigmaTPC(0),
		PnSigmaITS(0),
		PnSigmaTOF(0),
		KnSigmaITS(0),
		KnSigmaTPC(0),
		KnSigmaTOF(0),
		ITSsignal(0),
		TPCsignal(0),
		TOFsignal(0),
		goldenChi2(0),
		mcEta(0),
		mcPhi(0),
		mcPt(0),
		mcVert{0,0,0},
		iPdg(0),
		iPdgMother(0),
		HasMother(0),
		motherLabel(0),
		pointingAngle(0),
		daughtersDCA(0),
		decayLength(0),
		v0mass(0),
		ptArm(0),
		alpha(0),
    fQAhist(0),
    fCentralityPercentileMin(0),
    fCentralityPercentileMax(80), 
    fPtMin(0.2),
    fPtMax(10),
    fEtaMin(-0.8),
    fEtaMax(0.8),
    fESigITSMin(-3.),
    fESigITSMax(3.),
    fESigTPCMin(-4.),
    fESigTPCMax(4.),
    fESigTOFMin(-3.),
    fESigTOFMax(3.),
    fPIDcutITS(kFALSE),
    fPIDcutTOF(kFALSE),
    fPionPIDcutTPC(kFALSE),
    fPSigTPCMin(-99.),
    fPSigTPCMax(-3.),
    fHasSDD(kTRUE),
    fIsV0tree(kFALSE),
    fArmPlot(0),
    fIsAOD(kTRUE),
    fFilterBit(16),
    fIsGRIDanalysis(kTRUE),
    fGridPID(-1),
		fUseTPCcorr(kFALSE),
		fWidth(0),
		fMean(0)
{

}

AliAnalysisTaskSimpleTreeMaker::AliAnalysisTaskSimpleTreeMaker(const char *name) :
    AliAnalysisTaskSE(name),
		hasMC(kFALSE),
   	fESDtrackCuts(0),
    fPIDResponse(0),
    fTree(0),
		eventCuts(0),
		eventFilter(0),
		varCuts(0),
		trackCuts(0),
		pidCuts(0),
		cuts(0),
		trackFilter(0),
		varManager(0),
		primaryVertex{0,0,0},
		multiplicityV0A(0),
		multiplicityV0C(0),
		multiplicityCL1(0),
		runNumber(0),
		event(0),
		pt(0),
		eta(0),
		phi(0),
		nTPCclusters(0),
		nTPCcrossed(0),
		fTPCcrossOverFind(0),
		nTPCfindable(0),
		tpcSharedMap(0),
		nTPCshared(0),
		chi2TPC(0),
		DCA{0,0},
		nITS(0),
		chi2ITS(0),
		fITSshared(0),
		SPDfirst(0),
		charge(0),
		EnSigmaITS(0),
		EnSigmaTPC(0),
		EnSigmaTPCcorr(0),
		EnSigmaTOF(0),
		PnSigmaTPC(0),
		PnSigmaITS(0),
		PnSigmaTOF(0),
		KnSigmaITS(0),
		KnSigmaTPC(0),
		KnSigmaTOF(0),
		ITSsignal(0),
		TPCsignal(0),
		TOFsignal(0),
		goldenChi2(0),
		mcEta(0),
		mcPhi(0),
		mcPt(0),
		mcVert{0,0,0},
		iPdg(0),
		iPdgMother(0),
		HasMother(0),
		motherLabel(0),
		pointingAngle(0),
		daughtersDCA(0),
		decayLength(0),
		v0mass(0),
		ptArm(0),
		alpha(0),
    fQAhist(0),
    fCentralityPercentileMin(0),
    fCentralityPercentileMax(80), 
    fPtMin(0.2),
    fPtMax(10),
    fEtaMin(-0.8),
    fEtaMax(0.8),
    fESigITSMin(-3.),
    fESigITSMax(3.),
    fESigTPCMin(-4.),
    fESigTPCMax(4.),
    fESigTOFMin(-3.),
    fESigTOFMax(3.),
    fPIDcutITS(kFALSE),
    fPIDcutTOF(kFALSE),
    fPionPIDcutTPC(kFALSE),
		fPSigTPCMin(-99.),
    fPSigTPCMax(-3.),
    fHasSDD(kTRUE),
    fIsV0tree(kFALSE),
    fArmPlot(0),
    fIsAOD(kTRUE),
    fFilterBit(16),
    fIsGRIDanalysis(kTRUE),
    fGridPID(0),
		fUseTPCcorr(kFALSE),
		fWidth(0),
		fMean(0)

{
    if(!fIsV0tree){
        fESDtrackCuts = AliESDtrackCuts::GetStandardITSTPCTrackCuts2011(kFALSE,1);
    }
    else{
        fESDtrackCuts = AliESDtrackCuts::GetStandardV0DaughterCuts();
    }

    // Input slot #0 works with a TChain
    DefineInput(0, TChain::Class());
    DefineOutput(1, TTree::Class()); //will be connected to fTree
    DefineOutput(2, TH1F::Class());
    DefineOutput(3, TH2F::Class());
}

//________________________________________________________________________

AliAnalysisTaskSimpleTreeMaker::~AliAnalysisTaskSimpleTreeMaker(){

  delete eventCuts;
  delete eventFilter;
  
  delete varCuts;
  delete trackCuts;
  delete pidCuts;
  delete cuts;
  delete trackFilter; 

}


//________________________________________________________________________

void AliAnalysisTaskSimpleTreeMaker::UserCreateOutputObjects(){
  
    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    AliInputEventHandler* inputHandler = dynamic_cast<AliInputEventHandler*>(man->GetInputEventHandler());
    inputHandler->SetNeedField();
     
    fPIDResponse = inputHandler->GetPIDResponse();
    if(!fPIDResponse){
        AliFatal("This task needs the PID response attached to the inputHandler");
      return;
    } 

    fTree = new TTree("tracks", "tracks");
		//Common branches to all variants of class

		//Track variables
		fTree->Branch("pt",                &pt);
		fTree->Branch("eta",               &eta);
		fTree->Branch("phi",               &phi);
		fTree->Branch("nTPCclusters",      &nTPCclusters);
		fTree->Branch("nTPCcrossed",       &nTPCcrossed);
		fTree->Branch("nTPCfindable",      &nTPCfindable);
		fTree->Branch("nTPCshared",        &nTPCshared);
		fTree->Branch("fTPCcrossOverFind", &fTPCcrossOverFind);
		fTree->Branch("chi2TPC",           &chi2TPC);
		fTree->Branch("nITS",              &nITS);
		fTree->Branch("fITSshared",        &fITSshared);
		fTree->Branch("chi2ITS",           &chi2ITS);
		fTree->Branch("SPDfirst",          &SPDfirst);
		fTree->Branch("DCAxy",             &DCA[0]);
		fTree->Branch("DCAz",              &DCA[1]);
		fTree->Branch("goldenChi2",        &goldenChi2);
		fTree->Branch("charge",            &charge);
		fTree->Branch("EsigITS",           &EnSigmaITS);
		fTree->Branch("EsigTPC",           &EnSigmaTPC);
		if(fUseTPCcorr){
			fTree->Branch("EsigTPCcorr",       &EnSigmaTPCcorr);
		}
		fTree->Branch("EsigTOF",           &EnSigmaTOF);
		fTree->Branch("PsigITS",           &PnSigmaITS);
		fTree->Branch("PsigTPC",           &PnSigmaTPC);
		fTree->Branch("PsigTOF",           &PnSigmaTOF);
		fTree->Branch("KsigITS",           &KnSigmaITS);
		fTree->Branch("KsigTPC",           &KnSigmaTPC);
		fTree->Branch("KsigTOF",           &KnSigmaTOF);
		fTree->Branch("ITSsignal",         &ITSsignal);
		fTree->Branch("TPCsignal",         &TPCsignal);
		fTree->Branch("TOFsignal",         &TOFsignal);
		//V0 tree variables
		if(fIsV0tree){
				fTree->Branch("pointingAngle", &pointingAngle);
				fTree->Branch("daughtersDCA",  &daughtersDCA);
				fTree->Branch("decayLength",   &decayLength);
				fTree->Branch("v0mass",        &v0mass);
				fTree->Branch("ptArm",         &ptArm);
				fTree->Branch("alpha",         &alpha);
		}
		//MC variables
		if(hasMC){
			fTree->Branch("mcPt",        &mcPt);
			fTree->Branch("mcEta",       &mcEta);
			fTree->Branch("mcPhi",       &mcPhi);
			fTree->Branch("mcVertx",     &mcVert[0]);
			fTree->Branch("mcVerty",     &mcVert[1]);
			fTree->Branch("mcVertz",     &mcVert[2]);
			fTree->Branch("pdg",         &iPdg);
			fTree->Branch("pdgMother",   &iPdgMother);
			fTree->Branch("HasMother",   &HasMother);
			fTree->Branch("motherLabel", &motherLabel);
		}
		//Event variables
		fTree->Branch("vertexX",         &primaryVertex[0]);
		fTree->Branch("vertexY",         &primaryVertex[1]);
		fTree->Branch("vertexZ",         &primaryVertex[2]);
		fTree->Branch("runNumber",       &runNumber);
		fTree->Branch("eventNum",        &eventNum);
		fTree->Branch("multiplicityV0A", &multiplicityV0A);
		fTree->Branch("multiplicityV0C", &multiplicityV0C);
		fTree->Branch("multiplicityCL1", &multiplicityCL1);
		fTree->Branch("gridPID",         &fGridPID);

    //Get grid PID which can be used later to assign unique event numbers
    if(fIsGRIDanalysis){
        const char* gridIDchar = gSystem->Getenv("ALIEN_PROC_ID");
        std::string str(gridIDchar);
        SetGridPID(str);
    }
    else{ 
        fGridPID = -1;
    }


    //Create TH2F for armenteros plot. Filled if creating v0 tree
		//NOTE: Class designed to study electrons with weak EsigTPC cuts
		//applied. Therefore, the number of tracks will not necessarily be twice the
		//number of V0 particles (as one might expect). 
    fArmPlot = new TH2F("ArmPlot", "Armenteros Plot", 100, -1, 1, 100, 0, 0.4);
    if(fIsV0tree){
        fArmPlot->GetXaxis()->SetTitle("#alpha = (p^{+}-p^{-})/(p^{+}+p^{-})");
        fArmPlot->GetYaxis()->SetTitle("p_{T}");
    }  

    fQAhist = new TH1F("h1", "Event and track QA", 8, 0, 8);
		//Fill each bin with nothing to ensure correct ordering
		fQAhist->Fill("Events_check",0);
		fQAhist->Fill("Events_accepted",0);
		fQAhist->Fill("Events_MCcheck",0);
		fQAhist->Fill("Tracks_all",0);
		fQAhist->Fill("Tracks_MCcheck",0);
		fQAhist->Fill("Tracks_KineCuts",0);
		fQAhist->Fill("Tracks_TrackCuts",0);
		fQAhist->Fill("Tracks_PIDcuts",0);
		    
		PostData(1, fTree);
    PostData(2, fQAhist);
    PostData(3, fArmPlot);
}

//________________________________________________________________________

void AliAnalysisTaskSimpleTreeMaker::UserExec(Option_t *){
		
	//Main loop
	//Called for each event
	AliVEvent* event = 0x0;
	AliESDEvent* esdEvent = 0x0;
	if(!fIsV0tree){
		event = dynamic_cast<AliVEvent*>(InputEvent());
	}else{
		esdEvent = dynamic_cast<AliESDEvent*>(InputEvent());
		//event = dynamic_cast<AliESDEvent*>(InputEvent());
		event = esdEvent;
	}
	if(!event){
		AliError("No event");
		return;
	} 
	if(!fIsV0tree){
		fQAhist->Fill("Events_check",1);
	}

	// check event cuts
	if(IsEventAccepted(event) == 0){ 
		return;
	}
	if(!fIsV0tree){
		fQAhist->Fill("Events_accepted",1);
	}

	//Check if running on MC files
	AliMCEvent* mcEvent = MCEvent();
	if(mcEvent){
		hasMC = kTRUE;
		if(!fIsV0tree){
			fQAhist->Fill("Events_MCcheck",1);
		}
	}
	else{
		hasMC = kFALSE;
	}

	eventNum += 1;

    
	//PID Response task active?
	fPIDResponse = (dynamic_cast<AliInputEventHandler*>((AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler())))->GetPIDResponse();

	if(!fPIDResponse){ AliFatal("This task needs the PID response attached to the inputHandler"); }

	AliVVertex* vertex = const_cast<AliVVertex*>(event->GetPrimaryVertex());

	primaryVertex[0] = vertex->GetX();
	primaryVertex[1] = vertex->GetY();
	primaryVertex[2] = vertex->GetZ(); 


	//Get Multiplicity
	AliMultSelection* multSelection = dynamic_cast<AliMultSelection*>(event->FindListObject("MultSelection"));
	if(!multSelection){
		AliWarning("AliMultSelection object not found");
	}
	else{
		multiplicityV0A = multSelection->GetMultiplicityPercentile("V0A");
		multiplicityV0C = multSelection->GetMultiplicityPercentile("V0C");
		multiplicityCL1 = multSelection->GetMultiplicityPercentile("CL1");
	}

	//Check if ESD or AOD analysis
	//Get ClassName from file and set appropriate ESD or AOD flag
	TString className = static_cast<TString>(event->ClassName());
	if(className.Contains("AOD")){
		fIsAOD = kTRUE;
	}
	else if(className.Contains("ESD")){
		fIsAOD = kFALSE;
	}
	else{
		AliError("-----!! Analysis type unknown !!--------");
		return;
	}
    

	runNumber         = event->GetRunNumber();
	Int_t eventTracks = event->GetNumberOfTracks();
	Int_t numV0s      = event->GetNumberOfV0s();

	AliVParticle* mcTrack = 0x0;
	AliVParticle* motherMCtrack = 0x0;
	
	//Setup TPC correction maps
	if(fUseTPCcorr){
		AliDielectronPID::SetCentroidCorrFunction( (TH1*)fMean->Clone() );
		AliDielectronPID::SetWidthCorrFunction( (TH1*)fWidth->Clone() );
		::Info("AliAnalysisTaskSimpleTreeMaker::UserExec","Setting Correction Histos");
	}

	//Needed by the dielectron framework
  varManager->SetPIDResponse(fPIDResponse);

	//Loop over tracks for event
	if(!fIsV0tree){
		for(Int_t iTrack = 0; iTrack < eventTracks; iTrack++){ 

			AliVTrack* track = dynamic_cast<AliVTrack*>(event->GetTrack(iTrack));
			if(!track){
				AliError(Form("Could not receive track %d", iTrack));
				continue;
			}

			fQAhist->Fill("Tracks_all",1);

			//Set MC features to dummy values
			mcEta       = -99;
			mcPhi       = -99;
			mcPt        = -99;
			mcVert[0]   = {-99};
			mcVert[1]   = {-99};
			mcVert[2]   = {-99};
			iPdg        = -9999;
			iPdgMother  = -9999;
			HasMother   = kFALSE;
			motherLabel = -9999; //Needed to determine whether tracks have same mother
			//Bool_t IsEnhanced = kFALSE;

			//Get MC information
			if(hasMC){
				if(fIsAOD){
					mcTrack = dynamic_cast<AliAODMCParticle*>(mcEvent->GetTrack(TMath::Abs(track->GetLabel())));

					//Check valid pointer has been returned. If not, disregard track. 
					if(!mcTrack){
						continue;
					}
				}else{
					mcTrack = dynamic_cast<AliMCParticle*>(mcEvent->GetTrack(TMath::Abs(track->GetLabel())));

					//Check valid pointer has been returned. If not, disregard track. 
					if(!mcTrack){
						continue;
					}
				}
					
				fQAhist->Fill("Tracks_MCcheck", 1);

				iPdg  = mcTrack->PdgCode();
				mcEta = mcTrack->Eta();
				mcPhi = mcTrack->Phi();
				mcPt  = mcTrack->Pt();
				mcTrack->XvYvZv(mcVert);

				Int_t gMotherIndex = mcTrack->GetMother();
				
				if(!(gMotherIndex < 0)){
					if(fIsAOD){
						motherMCtrack = dynamic_cast<AliAODMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}else{
						motherMCtrack = dynamic_cast<AliMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
                    }
						HasMother = kTRUE;
            iPdgMother = motherMCtrack->PdgCode();
            motherLabel = TMath::Abs(motherMCtrack->GetLabel());
				}
					
				//Currently no injected MC productions for Run 2. Hence commented out
				//Now determine whether track comes from an injected MC sample or not
				/*Int_t mcTrackIndex = -9999;
				while(!(mcTrack->GetMother() < 0)){ 
						mcTrackIndex = mcTrack->GetMother();
						mcTrack = dynamic_cast<AliAODMCParticle*>(mcEvent->GetTrack(mcTrack->GetMother()));
				}
				if(!(mcEvent->IsFromBGEvent(TMath::Abs(mcTrackIndex)))){
						IsEnchanced = kTRUE;
				}else{
						IsEnhanced = kFALSE;
				}*/
      }

			//Apply global track trackFilter
			if(!fIsAOD){
				if(!(fESDtrackCuts->AcceptTrack(dynamic_cast<const AliESDtrack*>(track)))){ 
						continue; 
				}
			}
			else{
				UInt_t selectedMask = (1<<trackFilter->GetCuts()->GetEntries())-1;
				if( selectedMask != (trackFilter->IsSelected((AliVParticle*)track)) ){
					continue;
				}
			}

			fQAhist->Fill("Tracks_Cuts", 1);
			
			pt  = track->Pt();
			eta = track->Eta();
			phi = track->Phi();

			//Get PID response of track without TPC calibration
			EnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kElectron);
			if(fUseTPCcorr){
				EnSigmaTPCcorr = EnSigmaTPC;
				EnSigmaTPCcorr -= AliDielectronPID::GetCntrdCorr(track);
				EnSigmaTPCcorr /= AliDielectronPID::GetWdthCorr(track);
			}
				
			EnSigmaITS = -999;
			if(fHasSDD){
				EnSigmaITS = fPIDResponse->NumberOfSigmasITS(track,(AliPID::EParticleType)AliPID::kElectron);
			}
			EnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(track,(AliPID::EParticleType)AliPID::kElectron);

			PnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kPion);
		
			//Get rest of nSigma values for pion and kaon
			PnSigmaITS = fPIDResponse->NumberOfSigmasITS(track,(AliPID::EParticleType)AliPID::kPion);
			PnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(track,(AliPID::EParticleType)AliPID::kPion);

			KnSigmaITS = fPIDResponse->NumberOfSigmasITS(track,(AliPID::EParticleType)AliPID::kKaon);
			KnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kKaon);
			KnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(track,(AliPID::EParticleType)AliPID::kKaon);

			//Get TPC information
			//kNclsTPC
			nTPCclusters = track->GetTPCNcls(); 
			
			//kNFclsTPCr
			nTPCcrossed = track->GetTPCClusterInfo(2,1);

			fTPCcrossOverFind = 0;
			nTPCfindable = track->GetTPCNclsF();
			//kNFclsTPCfCross
			if(nTPCfindable > 0){
				fTPCcrossOverFind = nTPCcrossed/nTPCfindable;
			}

			tpcSharedMap = 0;
			if(fIsAOD){
				tpcSharedMap = (dynamic_cast<AliAODTrack*>(track))->GetTPCSharedMap();
			}else{
				tpcSharedMap = (dynamic_cast<AliESDtrack*>(track))->GetTPCSharedMap();
			}

			nTPCshared = -1;
			if(fIsAOD){
				nTPCshared = tpcSharedMap.CountBits(0) - tpcSharedMap.CountBits(159);
			}else{
				nTPCshared = (dynamic_cast<AliESDtrack*>(track))->GetTPCnclsS();
			}

			chi2TPC = track->GetTPCchi2(); //Function only implemented in ESDs. Returns dumym value for AODs
			
			//DCA values
			Float_t  DCAesd[2] = {0.0, 0.0};
			Double_t DCAaod[2] = {0.0, 0.0};
			Double_t DCAcov[2] = {0.0, 0.0};
			if(!fIsAOD){
				//Arguments: xy, z
				track->GetImpactParameters( &DCAesd[0], &DCAesd[1]);
			}
			else{
				GetDCA(const_cast<const AliVEvent*>(event), dynamic_cast<const AliAODTrack*>(track), DCAaod, DCAcov);
			}
			//Final DCA values stored here 
			if(!fIsAOD){
				DCA[0] = static_cast<Double_t>(DCAesd[0]);
				DCA[1] = static_cast<Double_t>(DCAesd[1]);
			}
			else{
				DCA[0] = static_cast<Double_t>(DCAaod[0]);
				DCA[1] = static_cast<Double_t>(DCAaod[1]);
			}

			//Get ITS information
			//kNclsITS
			nITS = track->GetNcls(0);
			chi2ITS = track->GetITSchi2();
			
			fITSshared = 0.;
			for(Int_t d = 0; d < 6; d++){
				fITSshared += static_cast<Double_t>(track->HasSharedPointOnITSLayer(d));
			}
			fITSshared /= nITS;

			if(fIsAOD){
				SPDfirst = (dynamic_cast<AliAODTrack*>(track))->HasPointOnITSLayer(0); 
			}else{
				SPDfirst = (dynamic_cast<AliESDtrack*>(track))->HasPointOnITSLayer(0);
			}

			//Get ITS and TPC signals
			ITSsignal = track->GetITSsignal();
			TPCsignal = track->GetTPCsignal();
			TOFsignal = track->GetTOFsignal();

			Int_t fCutMaxChi2TPCConstrainedVsGlobalVertexType = fESDtrackCuts->kVertexTracks | fESDtrackCuts->kVertexSPD;

			const AliVVertex* vertex = 0;

			if(fCutMaxChi2TPCConstrainedVsGlobalVertexType & fESDtrackCuts->kVertexTracks){
				vertex = track->GetEvent()->GetPrimaryVertexTracks();
			}

			if((!vertex || !vertex->GetStatus()) && fCutMaxChi2TPCConstrainedVsGlobalVertexType & fESDtrackCuts->kVertexSPD){
				vertex = track->GetEvent()->GetPrimaryVertexSPD();
			}

			if((!vertex || !vertex->GetStatus()) && fCutMaxChi2TPCConstrainedVsGlobalVertexType & fESDtrackCuts->kVertexTPC){
				vertex = track->GetEvent()->GetPrimaryVertexTPC();
			}
			
			//Get golden Chi2
			goldenChi2 = -1;
			if(vertex->GetStatus()){
				if(fIsAOD){
					goldenChi2 = dynamic_cast<AliAODTrack*>(track)->GetChi2TPCConstrainedVsGlobal();
				}
				else{
					goldenChi2 = dynamic_cast<AliESDtrack*>(track)->GetChi2TPCConstrainedVsGlobal(dynamic_cast<const AliESDVertex*>(vertex));
				}
			}

			charge = track->Charge();

			fTree->Fill();
    } //End loop over tracks
  }//End of "normal" TTree creation
  else if(fIsV0tree){
		for(Int_t iV0 = 0; iV0 < numV0s; iV0++){

			AliESDv0* V0vertex = esdEvent->GetV0(iV0);
	
			if(!V0vertex){
				AliError(Form("Could not receive V0 track %d", iV0));
				continue;
			}

			//fQAhist->Fill("Tracks_all",1);

			//Get V0 daughter tracks
			AliESDtrack* negTrack = esdEvent->GetTrack(V0vertex->GetIndex(0));
			AliESDtrack* posTrack = esdEvent->GetTrack(V0vertex->GetIndex(1));
			if(!negTrack || !posTrack){
				Printf("Daughter track of v0 not found: %p - %p \n",negTrack, posTrack);
				continue;
			}
			//Check for like-sign V0 candidates
			if(negTrack->Charge() == posTrack->Charge()){ continue; }

			//Apply kinematic and PID cuts to both legs 
			if(isV0daughterAccepted(negTrack) != kTRUE){ continue; }
			if(isV0daughterAccepted(posTrack) != kTRUE){ continue; }

			pointingAngle = V0vertex->GetV0CosineOfPointingAngle();
			daughtersDCA  = V0vertex->GetDcaV0Daughters();
			decayLength   = V0vertex->GetRr();
			v0mass        = V0vertex->M();
			//Super loose cuts on V0 topological qualities(stored in Tree to be cut on later)
			if( pointingAngle < 0.8 || daughtersDCA < 0.05 || decayLength < 0.01 ){ continue; }
			
			//fQAhist->Fill("Arm. cuts",1);

			ptArm = V0vertex->PtArmV0();
			alpha = V0vertex->AlphaV0();
			//Armentors plots is not filled until MC checks completed

		

			//TODO: Improve efficiency of MC section
			//Currently: checks neg particle, then pos particle.
			//If both pass, write pos particle features, then get neg features
			//again!
			Int_t label = -9999;
			if(hasMC){
				//-------- Check negative charge particle --------------
				//Only want to check validity of track
				label = negTrack->GetLabel();

				mcTrack = dynamic_cast<AliMCParticle*>(mcEvent->GetTrack(TMath::Abs(label)));
				if(!mcTrack){
					continue;
				}
				
				Int_t gMotherIndex = mcTrack->GetMother();
		
				if(!(gMotherIndex < 0)){
					if(fIsAOD){
						motherMCtrack = dynamic_cast<AliAODMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}else{
						motherMCtrack = dynamic_cast<AliMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}
				}//End loop over motherIndex
				//--------- End negative particle check	 -------------

				//--------- Get postive particle features
				//Set features for writing into TTree
				 
				label = posTrack->GetLabel();

				mcTrack = dynamic_cast<AliMCParticle*>(mcEvent->GetTrack(TMath::Abs(label)));
				if(!mcTrack){
					continue;
				}

				//Fill MC check
				//fQAhist->Fill("Tracks_MCcheck", 1);

				iPdg = mcTrack->PdgCode();
				
				mcEta = mcTrack->Eta();
				mcPhi = mcTrack->Phi();
				mcPt  = mcTrack->Pt();
				mcTrack->XvYvZv(mcVert);

				gMotherIndex = mcTrack->GetMother();
		
				if(!(gMotherIndex < 0)){
					if(fIsAOD){
						motherMCtrack = dynamic_cast<AliAODMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}else{
						motherMCtrack = dynamic_cast<AliMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}
					HasMother   = kTRUE;
					iPdgMother  = motherMCtrack->PdgCode();
					motherLabel = TMath::Abs(motherMCtrack->GetLabel());
				}//End loop over motherIndex
			}//End ifMC section
	
			fArmPlot->Fill(alpha, ptArm);

			//Get positive particle obsevables
			pt  = posTrack->Pt();
			eta = posTrack->Eta();
			phi = posTrack->Phi();

			EnSigmaITS = -999;
			PnSigmaITS = -999;
			KnSigmaITS = -999;

			if(fHasSDD){
				EnSigmaITS = fPIDResponse->NumberOfSigmasITS(posTrack,(AliPID::EParticleType)AliPID::kElectron);
				PnSigmaITS = fPIDResponse->NumberOfSigmasITS(posTrack,(AliPID::EParticleType)AliPID::kPion);
				KnSigmaITS = fPIDResponse->NumberOfSigmasITS(posTrack,(AliPID::EParticleType)AliPID::kKaon);
			}
			EnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(posTrack,(AliPID::EParticleType)AliPID::kElectron);
			EnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(posTrack,(AliPID::EParticleType)AliPID::kElectron);
			PnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(posTrack,(AliPID::EParticleType)AliPID::kPion);
			PnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(posTrack,(AliPID::EParticleType)AliPID::kPion);
			KnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(posTrack,(AliPID::EParticleType)AliPID::kKaon);
			KnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(posTrack,(AliPID::EParticleType)AliPID::kKaon);
	
			
			//DCA values
			//Initialized here incase goto is invoked
			Float_t DCAesd[2]  = {0.0,0.0};
			Double_t DCAaod[2] = {0.0,0.0};
			Double_t DCAcov[2] = {0.0, 0.0};

			if(TMath::Abs(EnSigmaTPC) > fESigTPCMax){ 
				goto negativeTrack;
			}

			//Get TPC information
			nTPCclusters = posTrack->GetTPCNcls(); 
			if(nTPCclusters < 70){ continue;}
			
			nTPCcrossed = posTrack->GetTPCClusterInfo(2,1);
			if(nTPCcrossed < 60){ continue;}

			fTPCcrossOverFind = 0;

			nTPCfindable = posTrack->GetTPCNclsF();
			if(nTPCfindable > 0){
				fTPCcrossOverFind = nTPCcrossed/nTPCfindable;
			}

			if(fTPCcrossOverFind < 0.3 || fTPCcrossOverFind > 1.1){ continue;}

			tpcSharedMap = 0;
			if(fIsAOD){
				tpcSharedMap = (dynamic_cast<AliAODTrack*>(posTrack))->GetTPCSharedMap();
			}else{
				tpcSharedMap = (dynamic_cast<AliESDtrack*>(posTrack))->GetTPCSharedMap();
			}

			nTPCshared = -1;
			if(fIsAOD){
				nTPCshared = tpcSharedMap.CountBits(0) - tpcSharedMap.CountBits(159);
			}else{
				nTPCshared = (dynamic_cast<AliESDtrack*>(posTrack))->GetTPCnclsS();
			}

			chi2TPC = posTrack->GetTPCchi2(); //Function only implemented in ESDs. Returns dumym value for AODs
			
			//Check for refits 
			if((posTrack->GetStatus() & AliVTrack::kITSrefit) <= 0){ continue;}
			if((posTrack->GetStatus() & AliVTrack::kTPCrefit) <= 0){ continue;}

			if(!fIsAOD){
				posTrack->GetImpactParameters( &DCAesd[0], &DCAesd[1]);
			}
			else{
				GetDCA(const_cast<const AliVEvent*>(event), dynamic_cast<const AliAODTrack*>(posTrack), DCAaod, DCAcov);
			}
			//Final DCA values stored here 
			if(!fIsAOD){
				DCA[0] = static_cast<Double_t>(DCAesd[0]);
				DCA[1] = static_cast<Double_t>(DCAesd[1]);
			}
			else{
				DCA[0] = static_cast<Double_t>(DCAaod[0]);
				DCA[1] = static_cast<Double_t>(DCAaod[1]);
			}

			//ITS clusters and shared clusters
			nITS = posTrack->GetNumberOfITSClusters();
			fITSshared = 0.;
			for(Int_t d = 0; d < 6; d++){
				fITSshared += static_cast<Double_t>(posTrack->HasSharedPointOnITSLayer(d));
			}
			fITSshared /= nITS;

			SPDfirst = (dynamic_cast<AliESDtrack*>(posTrack))->HasPointOnITSLayer(0);

			charge = posTrack->Charge();

			//Fill with feature from positive track
			fTree->Fill();

			//Skip to this point if positive track failed PID cut
			negativeTrack:

			//Get negative particle obsevables
			pt  = negTrack->Pt();
			eta = negTrack->Eta();
			phi = negTrack->Phi();

			if(fHasSDD){
				EnSigmaITS = fPIDResponse->NumberOfSigmasITS(negTrack,(AliPID::EParticleType)AliPID::kElectron);
				PnSigmaITS = fPIDResponse->NumberOfSigmasITS(negTrack,(AliPID::EParticleType)AliPID::kPion);
				KnSigmaITS = fPIDResponse->NumberOfSigmasITS(negTrack,(AliPID::EParticleType)AliPID::kKaon);
			}
			EnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(negTrack,(AliPID::EParticleType)AliPID::kElectron);
			EnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(negTrack,(AliPID::EParticleType)AliPID::kElectron);
			PnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(negTrack,(AliPID::EParticleType)AliPID::kPion);
			PnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(negTrack,(AliPID::EParticleType)AliPID::kPion);
			KnSigmaTPC = fPIDResponse->NumberOfSigmasTPC(negTrack,(AliPID::EParticleType)AliPID::kKaon);
			KnSigmaTOF = fPIDResponse->NumberOfSigmasTOF(negTrack,(AliPID::EParticleType)AliPID::kKaon);

			if(TMath::Abs(EnSigmaTPC) > fESigTPCMax){ 
				continue;
			}

			//Get TPC information
			nTPCclusters = negTrack->GetTPCNcls(); 
			if(nTPCclusters < 70){ continue;}
			
			nTPCcrossed = negTrack->GetTPCClusterInfo(2,1);
			if(nTPCcrossed < 60){ continue;}

			fTPCcrossOverFind = 0;

			nTPCfindable = negTrack->GetTPCNclsF();
			if(nTPCfindable > 0){
				fTPCcrossOverFind = nTPCcrossed/nTPCfindable;
			}

			if(fTPCcrossOverFind < 0.3 || fTPCcrossOverFind > 1.1){ continue;}

			tpcSharedMap = 0;
			if(fIsAOD){
				tpcSharedMap = (dynamic_cast<AliAODTrack*>(negTrack))->GetTPCSharedMap();
			}else{
				tpcSharedMap = (dynamic_cast<AliESDtrack*>(negTrack))->GetTPCSharedMap();
			}

			nTPCshared = -1;
			if(fIsAOD){
				nTPCshared = tpcSharedMap.CountBits(0) - tpcSharedMap.CountBits(159);
			}else{
				nTPCshared = (dynamic_cast<AliESDtrack*>(negTrack))->GetTPCnclsS();
			}

			chi2TPC = negTrack->GetTPCchi2(); //Function only implemented in ESDs. Returns dumym value for AODs
			
			//Check for refits 
			if((negTrack->GetStatus() & AliVTrack::kITSrefit) <= 0){ continue;}
			if((negTrack->GetStatus() & AliVTrack::kTPCrefit) <= 0){ continue;}

			//DCA values
			if(!fIsAOD){
				negTrack->GetImpactParameters( &DCAesd[0], &DCAesd[1]);
			}
			else{
				GetDCA(const_cast<const AliVEvent*>(event), dynamic_cast<const AliAODTrack*>(negTrack), DCAaod, DCAcov);
			}
			//Final DCA values stored here 
			if(!fIsAOD){
				DCA[0] = static_cast<Double_t>(DCAesd[0]);
				DCA[1] = static_cast<Double_t>(DCAesd[1]);
			}
			else{
				DCA[0] = static_cast<Double_t>(DCAaod[0]);
				DCA[1] = static_cast<Double_t>(DCAaod[1]);
			}

			//ITS clusters and shared clusters
			nITS = negTrack->GetNumberOfITSClusters();
			fITSshared = 0.;
			for(Int_t d = 0; d < 6; d++){
				fITSshared += static_cast<Double_t>(negTrack->HasSharedPointOnITSLayer(d));
			}
			fITSshared /= nITS;

			SPDfirst = (dynamic_cast<AliESDtrack*>(negTrack))->HasPointOnITSLayer(0);

			charge = negTrack->Charge(); 
			//Write negative observales to tree (v0 information written twice. Filter by looking at only pos or neg charge)
			if(hasMC){
				label = negTrack->GetLabel();

				mcTrack = dynamic_cast<AliMCParticle*>(mcEvent->GetTrack(TMath::Abs(label)));
				//Redundant?
				if(!mcTrack){
					continue;
				}

				//Fill MC check
				//fQAhist->Fill("Tracks_MCcheck", 1);

				iPdg = mcTrack->PdgCode();
				
				mcEta = mcTrack->Eta();
				mcPhi = mcTrack->Phi();
				mcPt  = mcTrack->Pt();
				mcTrack->XvYvZv(mcVert);

				Int_t gMotherIndex = mcTrack->GetMother();
	
				if(!(gMotherIndex < 0)){
					if(fIsAOD){
						motherMCtrack = dynamic_cast<AliAODMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}else{
						motherMCtrack = dynamic_cast<AliMCParticle*>((mcEvent->GetTrack(gMotherIndex)));
						//Check for mother particle. 
						if(!motherMCtrack){
							continue;
						}
					}
					HasMother = kTRUE;
					iPdgMother = motherMCtrack->PdgCode();
					motherLabel = TMath::Abs(motherMCtrack->GetLabel());
				}//End loop over motherIndex
			}

			
			//Fill with features from negative track
			fTree->Fill();
		}//End loop over v0's for this event
  }//End V0 code

}

//~ //________________________________________________________________________

void  AliAnalysisTaskSimpleTreeMaker::FinishTaskOutput(){
    // Finish task output

    // not implemented ...

}
//~ 

//~ //________________________________________________________________________

void AliAnalysisTaskSimpleTreeMaker::Terminate(Option_t *){
    // Draw result to the screen

    // Called once at the end of the query

    // not implemented ...

}
//~ 


//________________________________________________________________________

Int_t AliAnalysisTaskSimpleTreeMaker::IsEventAccepted(AliVEvent *event){
    
    if(!fIsV0tree){
			UInt_t selectedMask = (1<<eventFilter->GetCuts()->GetEntries())-1;
			varManager->SetEvent(event);
			if(selectedMask == (eventFilter->IsSelected(event))){
				return 1;
			}
    }
    else{
			if(TMath::Abs(event->GetPrimaryVertexSPD()->GetZ()) < 10){
				return 1;
			}
    }
    return 0;
}

Bool_t AliAnalysisTaskSimpleTreeMaker::isV0daughterAccepted(AliVTrack* track){
    
    Bool_t answer = kFALSE;
    //Kinematic cuts
    Double_t pt   = track->Pt();
    if( pt < fPtMin || pt > fPtMax ){ return answer; }
    Double_t eta  = track->Eta();
    if( eta < fEtaMin || eta > fEtaMax ){ return answer; } 

    if(!fIsAOD){
        if(!(fESDtrackCuts->AcceptTrack(dynamic_cast<AliESDtrack*>(track)))){
            return answer;
        }
    }
    else{
        if(!(dynamic_cast<AliAODTrack*>(track))->TestFilterBit(fFilterBit)){
            return answer;
        }
    }
    //fQAhist->Fill("Tracks_KineCuts", 1);
		//Do not apply PID cuts
    //fQAhist->Fill("Tracks_PIDcuts",1); 

    answer = kTRUE;
    return answer;
}

Bool_t AliAnalysisTaskSimpleTreeMaker::GetDCA(const AliVEvent* event, const AliAODTrack* track, Double_t* d0z0, Double_t* covd0z0){
// this is a copy of the AliDielectronVarManager

  if(track->TestBit(AliAODTrack::kIsDCA)){
    d0z0[0]=track->DCA();
    d0z0[1]=track->ZAtDCA();
    // the covariance matrix is not stored in case of AliAODTrack::kIsDCA
    return kTRUE;
  }

  Bool_t ok = kFALSE;
  if(event){
    AliExternalTrackParam etp; 
		etp.CopyFromVTrack(track);

    Float_t xstart = etp.GetX();
    if(xstart>3.){
      d0z0[0] = -999.;
      d0z0[1] = -999.;
      return kFALSE;
    }

    const AliAODVertex *vtx =dynamic_cast<const AliAODVertex*>((event->GetPrimaryVertex()));
    Double_t fBzkG = event->GetMagneticField(); // z componenent of field in kG
    ok = etp.PropagateToDCA(vtx,fBzkG,kVeryBig,d0z0,covd0z0);
  }

  if(!ok){
    d0z0[0] = -999.;
    d0z0[1] = -999.;
  }
  return ok;
}

//
void AliAnalysisTaskSimpleTreeMaker::SetupTrackCuts(AliDielectronCutGroup* cuts){

	//Initialise track cut object and add to DielectronCutGroup
	//Track cuts include kinematic cuts, track cuts and PID cuts
	trackFilter = new AliAnalysisFilter("TrackFilter", "trackCuts");
	trackFilter->AddCuts(cuts);
}

void AliAnalysisTaskSimpleTreeMaker::SetupEventCuts(AliDielectronEventCuts* cuts){

	//Initiliase the event trackFilter object and add event cuts
	eventFilter = new AliAnalysisFilter("eventFilter", "eventFilter");
	eventFilter->AddCuts(cuts);
}
