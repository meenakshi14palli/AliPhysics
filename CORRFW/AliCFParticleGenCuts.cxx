/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

////////////////////////////////////////////////////////////////////////////
//          ----   CORRECTION FRAMEWORK   ----
// class AliCFParticleGenCuts implementation
// Using this class a user may define selections relative to 
// MC particle (AliMCParticle) using generation-level information.
////////////////////////////////////////////////////////////////////////////
// author : R. Vernet (renaud.vernet@cern.ch)
////////////////////////////////////////////////////////////////////////////

#include "AliLog.h"
#include "AliCFParticleGenCuts.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "AliMCEvent.h"
#include "TObject.h"
#include "AliStack.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TBits.h"
#include "TList.h"

ClassImp(AliCFParticleGenCuts)

//______________________________
AliCFParticleGenCuts::AliCFParticleGenCuts() : 
  AliCFCutBase(),
  fMCInfo(0x0),
  fRequireIsCharged(0),
  fRequireIsNeutral(0),
  fRequireIsPrimary(0),
  fRequireIsSecondary(0),
  fRequirePdgCode(0),
  fPdgCode(0),
  fProdVtxXMin (-1.e+09),
  fProdVtxYMin (-1.e+09),
  fProdVtxZMin (-1.e+09),
  fProdVtxXMax ( 1.e+09),
  fProdVtxYMax ( 1.e+09),
  fProdVtxZMax ( 1.e+09),
  fDecayVtxXMin(-1.e+09),
  fDecayVtxYMin(-1.e+09),
  fDecayVtxZMin(-1.e+09),
  fDecayVtxXMax( 1.e+09),
  fDecayVtxYMax( 1.e+09),
  fDecayVtxZMax( 1.e+09),
  fDecayLengthMin(-1.),
  fDecayLengthMax(1.e+09),
  fDecayRxyMin(-1),
  fDecayRxyMax(1.e+09),
  fhCutStatistics(0x0),
  fhCutCorrelation(0x0),
  fBitmap(new TBits(0))
{
  //
  //ctor
  //
  for (int i=0; i<kNCuts; i++) 
    for (int j=0; j<kNStepQA; j++) 
      fhQA[i][j]=0x0;
}

//______________________________
AliCFParticleGenCuts::AliCFParticleGenCuts(const Char_t* name, const Char_t* title) : 
  AliCFCutBase(name,title),
  fMCInfo(0x0),
  fRequireIsCharged(0),
  fRequireIsNeutral(0),
  fRequireIsPrimary(0),
  fRequireIsSecondary(0),
  fRequirePdgCode(0),
  fPdgCode(0),
  fProdVtxXMin (-1.e+09),
  fProdVtxYMin (-1.e+09),
  fProdVtxZMin (-1.e+09),
  fProdVtxXMax ( 1.e+09),
  fProdVtxYMax ( 1.e+09),
  fProdVtxZMax ( 1.e+09),
  fDecayVtxXMin(-1.e+09),
  fDecayVtxYMin(-1.e+09),
  fDecayVtxZMin(-1.e+09),
  fDecayVtxXMax( 1.e+09),
  fDecayVtxYMax( 1.e+09),
  fDecayVtxZMax( 1.e+09),
  fDecayLengthMin(-1.),
  fDecayLengthMax(1.e+09),
  fDecayRxyMin(-1.),
  fDecayRxyMax(1.e+09),
  fhCutStatistics(0x0),
  fhCutCorrelation(0x0),
  fBitmap(new TBits(0))
{
  //
  //ctor
  //
  for (int i=0; i<kNCuts; i++) 
    for (int j=0; j<kNStepQA; j++) 
      fhQA[i][j]=0x0;
}

//______________________________
AliCFParticleGenCuts::AliCFParticleGenCuts(const AliCFParticleGenCuts& c) : 
  AliCFCutBase(c),
  fMCInfo(c.fMCInfo),
  fRequireIsCharged(c.fRequireIsCharged),
  fRequireIsNeutral(c.fRequireIsNeutral),
  fRequireIsPrimary(c.fRequireIsPrimary),
  fRequireIsSecondary(c.fRequireIsSecondary),
  fRequirePdgCode(c.fRequirePdgCode),
  fPdgCode(c.fPdgCode),
  fProdVtxXMin (c.fProdVtxXMin),
  fProdVtxYMin (c.fProdVtxYMin),
  fProdVtxZMin (c.fProdVtxZMin),
  fProdVtxXMax (c.fProdVtxXMax),
  fProdVtxYMax (c.fProdVtxYMax),
  fProdVtxZMax (c.fProdVtxZMax),
  fDecayVtxXMin(c.fDecayVtxXMin),
  fDecayVtxYMin(c.fDecayVtxYMin),
  fDecayVtxZMin(c.fDecayVtxZMin),
  fDecayVtxXMax(c.fDecayVtxXMax),
  fDecayVtxYMax(c.fDecayVtxYMax),
  fDecayVtxZMax(c.fDecayVtxZMax),
  fDecayLengthMin(c.fDecayLengthMin),
  fDecayLengthMax(c.fDecayLengthMin),
  fDecayRxyMin(c.fDecayLengthMin),
  fDecayRxyMax(c.fDecayLengthMin),
  fhCutStatistics(new TH1F(*c.fhCutStatistics)),
  fhCutCorrelation(new TH2F(*c.fhCutCorrelation)),
  fBitmap(new TBits(*c.fBitmap))
{
  //
  //copy ctor
  //
  if (c.fhCutStatistics)  fhCutStatistics  = new TH1F(*c.fhCutStatistics)  ;
  if (c.fhCutCorrelation) fhCutCorrelation = new TH2F(*c.fhCutCorrelation) ;

  for (int i=0; i<kNCuts; i++)
    for (int j=0; j<kNStepQA; j++)
      fhQA[i][j]=(TH1F*)c.fhQA[i][j]->Clone();
}

//______________________________
AliCFParticleGenCuts& AliCFParticleGenCuts::operator=(const AliCFParticleGenCuts& c)
{
  //
  // Assignment operator
  //
  if (this != &c) {
    AliCFCutBase::operator=(c) ;
    fMCInfo=c.fMCInfo;
    fRequireIsCharged=c.fRequireIsCharged;
    fRequireIsNeutral=c.fRequireIsNeutral;
    fRequireIsPrimary=c.fRequireIsPrimary;
    fRequireIsSecondary=c.fRequireIsSecondary;
    fRequirePdgCode=c.fRequirePdgCode;
    fPdgCode=c.fPdgCode;
    fProdVtxXMin=c.fProdVtxXMin;
    fProdVtxYMin=c.fProdVtxYMin;
    fProdVtxZMin=c.fProdVtxZMin;
    fProdVtxXMax=c.fProdVtxXMax;
    fProdVtxYMax=c.fProdVtxYMax;
    fProdVtxZMax=c.fProdVtxZMax;
    fDecayVtxXMin=c.fDecayVtxXMin;
    fDecayVtxYMin=c.fDecayVtxYMin;
    fDecayVtxZMin=c.fDecayVtxZMin;
    fDecayVtxXMax=c.fDecayVtxXMax;
    fDecayVtxYMax=c.fDecayVtxYMax;
    fDecayVtxZMax=c.fDecayVtxZMax;      
    fDecayLengthMin=c.fDecayVtxZMax;
    fDecayLengthMax=c.fDecayLengthMax;
    fDecayRxyMin=c.fDecayRxyMin;
    fDecayRxyMax=c.fDecayRxyMax;
    fBitmap=new TBits(*c.fBitmap);
    
    if (fhCutStatistics)  fhCutStatistics =new TH1F(*c.fhCutStatistics) ;
    if (fhCutCorrelation) fhCutCorrelation=new TH2F(*c.fhCutCorrelation);
    
    for (int i=0; i<kNCuts; i++)
      for (int j=0; j<kNStepQA; j++)
	fhQA[i][j]=(TH1F*)c.fhQA[i][j]->Clone();
  }
  return *this ;
}

//______________________________
Bool_t AliCFParticleGenCuts::IsSelected(TObject* obj) {
  //
  // check if selections on 'obj' are passed
  // 'obj' must be an AliMCParticle
  //
  
  SelectionBitMap(obj);

  if (fIsQAOn) FillHistograms(obj,0);

  for (UInt_t icut=0; icut<fBitmap->GetNbits();icut++)
    if (!fBitmap->TestBitNumber(icut)) return kFALSE ; 
  
  if (fIsQAOn) FillHistograms(obj,1);
  return kTRUE;
}

//__________________________________________________________________________________
void AliCFParticleGenCuts::SelectionBitMap(TObject* obj)
{
  //
  // test if the track passes the single cuts
  // and store the information in a bitmap
  //

  for (UInt_t i=0; i<kNCuts; i++) fBitmap->SetBitNumber(i,kFALSE);

  if (!obj) return  ;
  TString className(obj->ClassName());
  if (className.CompareTo("AliMCParticle") != 0) {
    AliError("argument must point to an AliMCParticle !");
    return ;
  }

  AliMCParticle* mcPart = (AliMCParticle*) obj ;
  TParticle* part = mcPart->Particle();
  AliStack*  stack = fMCInfo->Stack();

  Double32_t partVx=(Double32_t)part->Vx();
  Double32_t partVy=(Double32_t)part->Vy();
  Double32_t partVz=(Double32_t)part->Vz();

  TParticle* daughter=0x0;
  Double32_t decayVx=0.;
  Double32_t decayVy=0.;
  Double32_t decayVz=0.;
  Double32_t decayL=0.;
  Double32_t decayRxy=0.;

  if ( part->GetNDaughters() > 0 ) {
    daughter = stack->Particle(part->GetFirstDaughter()) ;
    decayVx=(Double32_t)daughter->Vx();
    decayVy=(Double32_t)daughter->Vy();
    decayVz=(Double32_t)daughter->Vz();
    decayL = TMath::Sqrt(TMath::Power(partVx-decayVx,2) + 
			 TMath::Power(partVy-decayVy,2) + 
			 TMath::Power(partVz-decayVz,2) ) ;
    decayRxy = TMath::Sqrt(TMath::Power(decayVx,2) + TMath::Power(decayVy,2) ) ;
  }


  Int_t iCutBit = 0;

  // cut on charge
  if ( fRequireIsCharged || fRequireIsNeutral ) {
    if(fRequireIsCharged &&  IsCharged(mcPart)) fBitmap->SetBitNumber(iCutBit,kTRUE);
    if(fRequireIsNeutral && !IsCharged(mcPart)) fBitmap->SetBitNumber(iCutBit,kTRUE);
  } 
  else fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  
  // cut on primary/secondary
  if ( fRequireIsPrimary || fRequireIsSecondary) {
    if (fRequireIsPrimary   &&  IsPrimary(mcPart,stack)) fBitmap->SetBitNumber(iCutBit,kTRUE);
    if (fRequireIsSecondary && !IsPrimary(mcPart,stack)) fBitmap->SetBitNumber(iCutBit,kTRUE);
  } 
  else fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;

  // cut on PDG code
  if ( fRequirePdgCode ){
    if (IsA(mcPart,fPdgCode,kTRUE)) fBitmap->SetBitNumber(iCutBit,kTRUE);
  }
  else fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;

  // production vertex cuts
  if ( partVx > fProdVtxXMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( partVx < fProdVtxXMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( partVy > fProdVtxYMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( partVy < fProdVtxYMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( partVz > fProdVtxZMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( partVz < fProdVtxZMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  
  // decay vertex cuts
  if ( decayVx > fDecayVtxXMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayVx < fDecayVtxXMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayVy > fDecayVtxYMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayVy < fDecayVtxYMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayVz > fDecayVtxZMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayVz < fDecayVtxZMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;

  // decay length cuts
  if ( decayL > fDecayLengthMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayL < fDecayLengthMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  
  // transverse decay length cuts
  if ( decayRxy > fDecayRxyMin ) fBitmap->SetBitNumber(iCutBit,kTRUE);
  iCutBit++;
  if ( decayRxy < fDecayRxyMax ) fBitmap->SetBitNumber(iCutBit,kTRUE);

}

//__________________________________________________________________________________
void AliCFParticleGenCuts::FillHistograms(TObject* obj, Bool_t afterCuts)
{
  //
  // fill the QA histograms
  //

  for (int iCutNumber = 0; iCutNumber < kNCuts; iCutNumber++) 
    fhQA[iCutNumber][afterCuts]->Fill(fBitmap->TestBitNumber(iCutNumber));

  // fill cut statistics and cut correlation histograms with information from the bitmap
  if (afterCuts) return;

  // Number of single cuts in this class
  UInt_t ncuts = fBitmap->GetNbits();
  for(UInt_t bit=0; bit<ncuts;bit++) {
   if (!fBitmap->TestBitNumber(bit)) {
      fhCutStatistics->Fill(bit+1);
      for (UInt_t bit2=bit; bit2<ncuts;bit2++) {
	if (!fBitmap->TestBitNumber(bit2)) 
	  fhCutCorrelation->Fill(bit+1,bit2+1);
      }
    }
  }
}

//__________________________________________________________________________________
void AliCFParticleGenCuts::AddQAHistograms(TList *qaList) {
  //
  // saves the histograms in a TList
  //

  DefineHistograms();

  qaList->Add(fhCutStatistics);
  qaList->Add(fhCutCorrelation);

  for (Int_t j=0; j<kNStepQA; j++) {
    for(Int_t i=0; i<kNCuts; i++)
      qaList->Add(fhQA[i][j]);
  }
}

//__________________________________________________________________________________
void AliCFParticleGenCuts::DefineHistograms() {
  //
  // histograms for cut variables, cut statistics and cut correlations
  //
  Int_t color = 2;

  // book cut statistics and cut correlation histograms
  fhCutStatistics = new TH1F(Form("%s_cut_statistics",GetName()),"",kNCuts,0.5,kNCuts+0.5);
  fhCutStatistics->SetLineWidth(2);
  int k = 1;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"charge")     ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"prim/sec")   ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"PDG")        ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"VtxXMin")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"VtxXMax")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"VtxYMin")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"VtxYMax")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"VtxZMin")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecZMax")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecXMin")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecXMax")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecYMin")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecYMax")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecZMin")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecZMax")    ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecLgthMin") ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecLgthMax") ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecRxyMin")  ; k++;
  fhCutStatistics->GetXaxis()->SetBinLabel(k,"DecRxyMax")  ; k++;


  fhCutCorrelation = new TH2F(Form("%s_cut_correlation",GetName()),"",kNCuts,0.5,kNCuts+0.5,kNCuts,0.5,kNCuts+0.5);
  fhCutCorrelation->SetLineWidth(2);
  for (k=1; k<=kNCuts; k++) {
    fhCutCorrelation->GetXaxis()->SetBinLabel(k,fhCutStatistics->GetXaxis()->GetBinLabel(k));
    fhCutCorrelation->GetYaxis()->SetBinLabel(k,fhCutStatistics->GetXaxis()->GetBinLabel(k));
  }

  Char_t str[256];
  for (int i=0; i<kNStepQA; i++) {
    if (i==0) sprintf(str," ");
    else sprintf(str,"_cut");
    fhQA[kCutCharge]     [i] = new TH1F(Form("%s_charge%s"      ,GetName(),str),"",2,-0.5,1.5);
    fhQA[kCutPrimSec]    [i] = new TH1F(Form("%s_primSec%s"     ,GetName(),str),"",2,-0.5,1.5);
    fhQA[kCutPDGCode]    [i] = new TH1F(Form("%s_pdgCode%s"     ,GetName(),str),"",10000,-5000,5000);
    fhQA[kCutProdVtxXMin][i] = new TH1F(Form("%s_prodVtxXMin%s" ,GetName(),str),"",100,0,10);
    fhQA[kCutProdVtxXMax][i] = new TH1F(Form("%s_prodVtxXMax%s" ,GetName(),str),"",100,0,10);
    fhQA[kCutProdVtxYMin][i] = new TH1F(Form("%s_prodVtxYMin%s" ,GetName(),str),"",100,0,10);
    fhQA[kCutProdVtxYMax][i] = new TH1F(Form("%s_prodVtxYMax%s" ,GetName(),str),"",100,0,10);
    fhQA[kCutProdVtxZMin][i] = new TH1F(Form("%s_prodVtxZMin%s" ,GetName(),str),"",100,0,10);
    fhQA[kCutProdVtxZMax][i] = new TH1F(Form("%s_prodVtxZMax%s" ,GetName(),str),"",100,0,10);
    fhQA[kCutDecVtxXMin] [i] = new TH1F(Form("%s_decVtxXMin%s"  ,GetName(),str),"",100,0,10);
    fhQA[kCutDecVtxXMax] [i] = new TH1F(Form("%s_decVtxXMax%s"  ,GetName(),str),"",100,0,10);
    fhQA[kCutDecVtxYMin] [i] = new TH1F(Form("%s_decVtxYMin%s"  ,GetName(),str),"",100,0,10);
    fhQA[kCutDecVtxYMax] [i] = new TH1F(Form("%s_decVtxYMax%s"  ,GetName(),str),"",100,0,10);
    fhQA[kCutDecVtxZMin] [i] = new TH1F(Form("%s_decVtxZMin%s"  ,GetName(),str),"",100,0,10);
    fhQA[kCutDecVtxZMax] [i] = new TH1F(Form("%s_decVtxZMax%s"  ,GetName(),str),"",100,0,10);
    fhQA[kCutDecLgthMin] [i] = new TH1F(Form("%s_decLengthMin%s",GetName(),str),"",100,0,10);
    fhQA[kCutDecLgthMax] [i] = new TH1F(Form("%s_decLengthMax%s",GetName(),str),"",100,0,10);
    fhQA[kCutDecRxyMin]  [i] = new TH1F(Form("%s_decRxyMin%s"   ,GetName(),str),"",100,0,10);
    fhQA[kCutDecRxyMax]  [i] = new TH1F(Form("%s_decRxyMax%s"   ,GetName(),str),"",100,0,10);
  }
  for(Int_t i=0; i<kNCuts; i++) fhQA[i][1]->SetLineColor(color);
}


//______________________________
Bool_t AliCFParticleGenCuts::IsCharged(AliMCParticle *mcPart) {
  //
  //check if particle is charged.
  //
  TParticle* part = mcPart->Particle();
  TParticlePDG* pdgPart = part->GetPDG();
  if(!pdgPart)return kFALSE;
  if (pdgPart->Charge() == 0) return kFALSE;
  return kTRUE;
}
//______________________________
Bool_t AliCFParticleGenCuts::IsPrimary(AliMCParticle *mcPart, AliStack *stack) {
  //
  //check if particle is primary (standard definition)
  //
  if (!stack->IsPhysicalPrimary(mcPart->Label())) return kFALSE ;
  return kTRUE;
}
//______________________________
Bool_t AliCFParticleGenCuts::IsPrimaryCharged(AliMCParticle *mcPart, AliStack *stack) {
  //
  //check if a charged particle is primary (standard definition)
  //
  if (!stack->IsPhysicalPrimary(mcPart->Label()) || !IsCharged(mcPart)) return kFALSE ;
  return kTRUE;
}
//______________________________
Bool_t AliCFParticleGenCuts::IsA(AliMCParticle *mcPart, Int_t pdg, Bool_t abs) {
  //
  //Check on the pdg code of the MC particle. if abs=kTRUE then check on the 
  //absolute value. By default is set to kFALSE.
  //
  TParticle* part = mcPart->Particle();
  Int_t pdgCode = part->GetPdgCode();
  if (abs) pdgCode = TMath::Abs(pdgCode);
  if (pdgCode != pdg ) return kFALSE;
  return kTRUE;
}
//______________________________
void AliCFParticleGenCuts::SetEvtInfo(TObject* mcEvent) {
  //
  // Sets pointer to MC event information (AliMCEvent)
  //

  if (!mcEvent) {
    AliError("Pointer to MC Event is null !");
    return;
  }
  
  TString className(mcEvent->ClassName());
  if (className.CompareTo("AliMCEvent") != 0) {
    AliError("argument must point to an AliMCEvent !");
    return ;
  }
  
  fMCInfo = (AliMCEvent*) mcEvent ;
}
