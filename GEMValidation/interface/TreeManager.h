#ifndef GEMCode_GEMValidation_TreeManager_h
#define GEMCode_GEMValidation_TreeManager_h

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "GEMCode/GEMValidation/interface/Structs/SimTrackStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/CSCSimHitStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/GEMSimHitStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/L1MuStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/RecoTrackStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/CSCDigiStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/GEMDigiStruct.h"
#include "GEMCode/GEMValidation/interface/Structs/CSCStubStruct.h"

#include "TTree.h"
#include <vector>
#include <string>

class TreeManager
{
 public:
  TreeManager() {}

  ~TreeManager() {}

  void book();

  void init();

  void fill();

  gem::SimTrackStruct& simTrack() { return simTrackSt_; }
  gem::GEMSimHitStruct& gemSimHit() { return gemSimHitSt_; }
  gem::CSCSimHitStruct& cscSimHit() { return cscSimHitSt_; }
  gem::GEMDigiStruct& gemDigi() { return gemDigiSt_; }
  gem::CSCDigiStruct& cscDigi() { return cscDigiSt_; }
  gem::CSCStubStruct& cscStub() { return cscStubSt_; }
  gem::L1MuStruct& l1mu() { return l1MuSt_; }
  gem::RecoTrackStruct& recoTrack() { return recoTrackSt_; }

 private:

  TTree* simTrackTree_;
  TTree* gemSimHitTree_;
  TTree* cscSimHitTree_;
  TTree* gemDigiTree_;
  TTree* cscDigiTree_;
  TTree* cscStubTree_;
  TTree* l1MuTree_;
  TTree* recoTrackTree_;

  gem::SimTrackStruct simTrackSt_;
  gem::GEMSimHitStruct gemSimHitSt_;
  gem::CSCSimHitStruct cscSimHitSt_;
  gem::GEMDigiStruct gemDigiSt_;
  gem::CSCDigiStruct cscDigiSt_;
  gem::CSCStubStruct cscStubSt_;
  gem::L1MuStruct l1MuSt_;
  gem::RecoTrackStruct recoTrackSt_;
};

#endif
