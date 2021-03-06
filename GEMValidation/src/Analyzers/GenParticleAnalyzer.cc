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

void GenParticleAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup, const MatcherSuperManager& manager, my::TreeManager& tree)
{
  iEvent.getByToken(inputToken_, genParticlesHandle_);

  // fetch the collection
  const reco::GenParticleCollection& genParticles = *genParticlesHandle_.product();

  auto& simTree = tree.simTrack();
  auto& genTree = tree.genParticle();

  int index = 0;
  for(auto iGenParticle = genParticles.begin();  iGenParticle != genParticles.end();  ++iGenParticle) {

    // require stable particle
    if (iGenParticle->status() != 1) continue;

    // add a few more selections
    if (iGenParticle->pt() < 2) continue;
    if (std::abs(iGenParticle->eta()) > 2.4) continue;

    // require muons
    if (std::abs(iGenParticle->pdgId()) != 13) continue;

    int tpidfound = -1;
    // check if it was matched to a simtrack
    for (int tpid = 0; tpid < MAX_PARTICLES; tpid++) {
      const auto& genMatch = manager.matcher(tpid)->genParticles()->getMatch();
      if (genMatch) {
        // check if the same
        if (verbose_)
          std::cout << genMatch->p4() <<  " " << iGenParticle->p4() << std::endl;
        if (genMatch->p4() == iGenParticle->p4()) {
          tpidfound =  tpid;
          break;
        }
      }
    }

    // genparticle properties
    genTree.gen_pt->push_back(iGenParticle->pt());
    genTree.gen_pz->push_back(iGenParticle->pz());
    genTree.gen_eta->push_back(iGenParticle->eta());
    genTree.gen_phi->push_back(iGenParticle->phi());
    genTree.gen_charge->push_back(iGenParticle->charge());
    genTree.gen_pdgid->push_back(iGenParticle->pdgId());
    genTree.gen_tpid->push_back(tpidfound);
    if (verbose_)
      std::cout << "tpidfound " << tpidfound << std::endl;

    if (tpidfound != -1)
      (*simTree.sim_id_gen)[tpidfound] = index;

    index++;
  }
}
