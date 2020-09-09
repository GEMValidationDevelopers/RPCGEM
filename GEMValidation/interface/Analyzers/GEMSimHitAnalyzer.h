#ifndef GEMCode_GEMValidation_GEMSimHitAnalyzer_h
#define GEMCode_GEMValidation_GEMSimHitAnalyzer_h

#include "GEMCode/GEMValidation/interface/Helpers.h"
#include "GEMCode/GEMValidation/interface/MatcherManager.h"
#include "GEMCode/GEMValidation/interface/TreeManager.h"

class GEMSimHitAnalyzer
{
public:

  // constructor
  GEMSimHitAnalyzer(const edm::ParameterSet& conf, edm::ConsumesCollector&& iC);

  // destructor
  ~GEMSimHitAnalyzer() {}

  void setMatcher(const GEMSimHitMatcher& match_sh);

  // initialize the event
  void analyze(TreeManager& tree);

 private:
  std::shared_ptr<GEMSimHitMatcher> match_;
  int minNHitsChamber_;
};

#endif
