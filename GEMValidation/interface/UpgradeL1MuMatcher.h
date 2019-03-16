#ifndef GEMCode_GEMValidation_UpgradeL1MuMatcher_h
#define GEMCode_GEMValidation_UpgradeL1MuMatcher_h

/**\class UpgradeL1MuMatcher

 Description: Matching of tracks to SimTrack

 Original Author:  "Sven Dildick"
*/
#include "GEMCode/GEMValidation/interface/TFTrack.h"
#include "GEMCode/GEMValidation/interface/TFCand.h"

#include "GEMCode/GEMValidation/interface/CSCStubMatcher.h"
#include "GEMCode/GEMValidation/interface/SimHitMatcher.h"
#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "DataFormats/L1TMuon/interface/EMTFTrack.h"
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
#include "L1Trigger/L1TMuonEndCap/interface/TrackTools.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

class UpgradeL1MuMatcher : public BaseMatcher
{
 public:
  /// constructor
  UpgradeL1MuMatcher(CSCStubMatcher&,
                     edm::EDGetTokenT<l1t::EMTFTrackCollection> &,
                     edm::EDGetTokenT< BXVector<l1t::RegionalMuonCand> > &,
			edm::EDGetTokenT< BXVector<l1t::Muon> > &);
  /// destructor
  ~UpgradeL1MuMatcher();

  std::vector<TFTrack*> tfTracks() const  {return tfTracks_;}
  TFTrack* bestTFTrack() const { return bestTrack; }
  TFCand* bestTFCand() const { return bestRegMuCand; }
  TFCand* bestGMTCand() const { return bestGMT; }
  std::vector<TFCand*> gmts() const { return gmts_; }

 private:

  void clear();

  float simPt;
  float simEta;
  float simPhi;
  float simE;
  float simCharge;

  void matchEmtfTrackToSimTrack(const l1t::EMTFTrackCollection&);
  void matchRegionalMuonCandToSimTrack(const BXVector<l1t::RegionalMuonCand>&);
  void matchGMTToSimTrack(const BXVector<l1t::Muon>&);

  float mindREMTFTrack = 10;
  TFTrack* bestTrack;

  float mindRRegMuCand = 10;
  TFCand* bestRegMuCand;

  float mindRGMT = 10;
  TFCand* bestGMT;

  const CSCStubMatcher* csc_stub_matcher_;

  int minBXEMTFTrack_, maxBXEMTFTrack_;
  int verboseEMTFTrack_;
  double deltaREMTFTrack_;
  bool runEMTFTrack_;

  int minBXRegMuCand_, maxBXRegMuCand_;
  int verboseRegMuCand_;
  double deltaRRegMuCand_;
  bool runRegMuCand_;

  int minBXGMT_, maxBXGMT_;
  int verboseGMT_;
  double deltaRGMT_;
  bool runGMT_;

  //l1t::EMTFTrackCollection tfTracks_;
  std::vector<TFTrack*> tfTracks_;
  std::vector<TFCand*> regMuCands_;
  std::vector<TFCand*> gmts_;
};

#endif
