#include "GEMCode/GEMValidation/interface/AnalyzerManager.h"

AnalyzerManager::AnalyzerManager(const edm::ParameterSet& conf)
{
  gent_.reset(new GenParticleAnalyzer(conf));
  simt_.reset(new SimTrackAnalyzer(conf));
  gemsh_.reset(new GEMSimHitAnalyzer(conf));
  gemdg_.reset(new GEMDigiAnalyzer(conf));
  gemstub_.reset(new GEMStubAnalyzer(conf));
  cscsh_.reset(new CSCSimHitAnalyzer(conf));
  cscdg_.reset(new CSCDigiAnalyzer(conf));
  cscstub_.reset(new CSCStubAnalyzer(conf));
  l1mu_.reset(new L1MuAnalyzer(conf));
  // l1track_.reset(new L1TrackAnalyzer(conf));
  // recotrack_.reset(new RecoTrackAnalyzer(conf));
}

void AnalyzerManager::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  cscstub_->init(iEvent, iSetup);
}

void AnalyzerManager::setManager(const MatcherManager& manager)
{
  gent_->setMatcher(*manager.genParticles());
  gemsh_->setMatcher(*manager.gemSimHits());
  gemdg_->setMatcher(*manager.gemDigis());
  gemstub_->setMatcher(*manager.gemDigis());
  cscsh_->setMatcher(*manager.cscSimHits());
  cscdg_->setMatcher(*manager.cscDigis());
  cscstub_->setMatcher(*manager.cscStubs());
  l1mu_->setMatcher(*manager.l1Muons());;
  // l1track_->setMatcher(*manager.l1Tracks());;
  // recotrack_->setMatcher(*manager.recoTracks());;
}

void
AnalyzerManager::analyze(TreeManager& tree, const SimTrack& t, const SimVertex& v)
{
  gent_->analyze(tree);
  simt_->analyze(tree, t, v);
  gemsh_->analyze(tree);
  gemdg_->analyze(tree);
  gemstub_->analyze(tree);
  cscsh_->analyze(tree);
  cscdg_->analyze(tree);
  cscstub_->analyze(tree);
  l1mu_->analyze(tree);
}
