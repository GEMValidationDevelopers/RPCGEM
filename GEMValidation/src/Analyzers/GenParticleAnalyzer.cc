#include "GEMCode/GEMValidation/interface/Analyzers/GenParticleAnalyzer.h"

GenParticleAnalyzer::GenParticleAnalyzer(const edm::ParameterSet& conf, edm::ConsumesCollector&& iC)
{
  const auto& gen = conf.getParameter<edm::ParameterSet>("genParticle");
  verbose_ = gen.getParameter<int>("verbose");
  run_ = gen.getParameter<bool>("run");

  inputToken_ = iC.consumes<reco::GenParticleCollection>(gen.getParameter<edm::InputTag>("inputTag"));
}

void GenParticleAnalyzer::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iEvent.getByToken(inputToken_, genParticlesHandle_);
}

void GenParticleAnalyzer::setMatcher(const GenParticleMatcher& match_sh)
{
  match_.reset(new GenParticleMatcher(match_sh));
}

void GenParticleAnalyzer::analyze(TreeManager& tree)
{
  // genparticle properties
  tree.genParticle().pt = match_->getMatch()->pt();
  tree.genParticle().pz = match_->getMatch()->pz();
  tree.genParticle().phi = match_->getMatch()->phi();
  tree.genParticle().eta = match_->getMatch()->eta();
  tree.genParticle().charge = match_->getMatch()->charge();
  tree.genParticle().endcap = (tree.genParticle().eta > 0.) ? 1 : -1;
  tree.genParticle().pdgid = match_->getMatch()->pdgId();
}

void GenParticleAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup, TreeManager& tree)
{
  iEvent.getByToken(inputToken_, genParticlesHandle_);

  // fetch the collection
  const reco::GenParticleCollection& genParticles = *genParticlesHandle_.product();

  float minDeltaR = 0.6;
  for(auto iGenParticle = genParticles.begin();  iGenParticle != genParticles.end();  ++iGenParticle) {

    // require stable particle
    if (iGenParticle->status() != 1) continue;

    // genparticle properties
    tree.genParticle().gen_pt->push_back(iGenParticle->pt());
    tree.genParticle().gen_pz->push_back(iGenParticle->pz());
    tree.genParticle().gen_eta->push_back(iGenParticle->eta());
    tree.genParticle().gen_phi->push_back(iGenParticle->phi());
    tree.genParticle().gen_charge->push_back(iGenParticle->charge());
    tree.genParticle().gen_pdgid->push_back(iGenParticle->pdgId());

    // check if it was matched to a simtrack

  }
}
