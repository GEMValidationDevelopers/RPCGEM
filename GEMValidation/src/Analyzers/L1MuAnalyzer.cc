#include "GEMCode/GEMValidation/interface/Analyzers/L1MuAnalyzer.h"

L1MuAnalyzer::L1MuAnalyzer(const edm::ParameterSet& conf, edm::ConsumesCollector&& iC)
{
  const auto& emtfTrack = conf.getParameter<edm::ParameterSet>("emtfTrack");
  minBXEMTFTrack_ = emtfTrack.getParameter<int>("minBX");
  maxBXEMTFTrack_ = emtfTrack.getParameter<int>("maxBX");
  verboseEMTFTrack_ = emtfTrack.getParameter<int>("verbose");
  runEMTFTrack_ = emtfTrack.getParameter<bool>("run");

  const auto& emtfCand = conf.getParameter<edm::ParameterSet>("emtfCand");
  minBXEMTFCand_ = emtfCand.getParameter<int>("minBX");
  maxBXEMTFCand_ = emtfCand.getParameter<int>("maxBX");
  verboseEMTFCand_ = emtfCand.getParameter<int>("verbose");
  runEMTFCand_ = emtfCand.getParameter<bool>("run");

  const auto& muon = conf.getParameter<edm::ParameterSet>("muon");
  minBXGMT_ = muon.getParameter<int>("minBX");
  maxBXGMT_ = muon.getParameter<int>("maxBX");
  verboseGMT_ = muon.getParameter<int>("verbose");
  runGMT_ = muon.getParameter<bool>("run");

  emtfTrackToken_ = iC.consumes<l1t::EMTFTrackCollection>(emtfTrack.getParameter<edm::InputTag>("inputTag"));
  emtfCandToken_ = iC.consumes<l1t::RegionalMuonCandBxCollection>(emtfCand.getParameter<edm::InputTag>("inputTag"));
  muonToken_ = iC.consumes<l1t::MuonBxCollection>(muon.getParameter<edm::InputTag>("inputTag"));
}

void L1MuAnalyzer::setMatcher(const L1MuMatcher& match_sh)
{
  match_.reset(new L1MuMatcher(match_sh));
}

void L1MuAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup,
                           const MatcherSuperManager& manager, my::TreeManager& tree) {

  iEvent.getByToken(emtfTrackToken_, emtfTrackHandle_);
  iEvent.getByToken(emtfCandToken_, emtfCandHandle_);
  iEvent.getByToken(muonToken_, muonHandle_);

  const l1t::EMTFTrackCollection& emtfTracks = *emtfTrackHandle_.product();
  const l1t::RegionalMuonCandBxCollection& emtfCands = *emtfCandHandle_.product();
  const l1t::MuonBxCollection& gmtCands = *muonHandle_.product();

  auto& trkTree = tree.l1mu();

  if (verboseEMTFTrack_)
    std::cout << "Analyzing " << emtfTracks.size() << " EMTF tracks" << std::endl;

  for (const auto& trk : emtfTracks) {

    int bx = trk.BX();
    if ( bx < minBXEMTFTrack_ or bx > maxBXEMTFTrack_) continue;

    const gem::EMTFTrack& gemtrk(trk);

    if (verboseEMTFTrack_)
      std::cout << gemtrk << std::endl;

    int tpidfound = -1;
    // check if it was matched to a simtrack
    for (int tpid = 0; tpid < MAX_PARTICLES; tpid++) {

      // get the matcher
      const auto& matcher = manager.matcher(tpid);

      // stop processing when the first invalid matcher is found
      if (matcher->isInValid()) break;

      const auto& trackMatch = matcher->l1Muons()->emtfTrack();
      if (trackMatch) {

        if (verboseEMTFTrack_)
          std::cout << "Candidate match " << *trackMatch
                    << std::endl;

        //check if the same
        if (gemtrk == *trackMatch) {
          tpidfound = tpid;
          std::cout << "...matched! With index " << tpidfound << std::endl;
          break;
        }
      }
    }
    trkTree.emtftrack_pt->push_back(gemtrk.pt());
    trkTree.emtftrack_eta->push_back(gemtrk.eta());
    trkTree.emtftrack_phi->push_back(gemtrk.phi());
    trkTree.emtftrack_charge->push_back(gemtrk.charge());
    trkTree.emtftrack_bx->push_back(gemtrk.bx());
    trkTree.emtftrack_tpid->push_back(tpidfound);
  }

  if (verboseEMTFCand_)
    std::cout << "Analyzing " << int(emtfCands.end(0) - emtfCands.begin(0)) << " EMTF cands" << std::endl;

  for (int bx = emtfCands.getFirstBX(); bx <= emtfCands.getLastBX(); bx++ ){
    if ( bx < minBXEMTFCand_ or bx > maxBXEMTFCand_) continue;
    for (auto emtfCand = emtfCands.begin(bx); emtfCand != emtfCands.end(bx); ++emtfCand ){

      const gem::EMTFCand& gemtrk(*emtfCand);

      if (verboseEMTFCand_)
        std::cout << gemtrk << std::endl;

      int tpidfound = -1;
      // check if it was matched to a simtrack
      for (int tpid = 0; tpid < MAX_PARTICLES; tpid++) {

        // get the matcher
        const auto& matcher = manager.matcher(tpid);

        // stop processing when the first invalid matcher is found
        if (matcher->isInValid()) break;

        const auto& trackMatch = matcher->l1Muons()->emtfCand();
        if (trackMatch) {

          if (verboseEMTFCand_)
            std::cout << "Candidate match " << *trackMatch
                      << std::endl;

          //check if the same
          if (gemtrk == *trackMatch) {
            tpidfound = tpid;
            std::cout << "...matched! With index " << tpidfound << std::endl;
            break;
          }
        }
      }
      trkTree.emtfcand_pt->push_back(gemtrk.pt());
      trkTree.emtfcand_eta->push_back(gemtrk.eta());
      trkTree.emtfcand_phi->push_back(gemtrk.phi());
      trkTree.emtfcand_charge->push_back(gemtrk.charge());
      trkTree.emtfcand_bx->push_back(gemtrk.bx());
      trkTree.emtfcand_tpid->push_back(tpidfound);
    }
  }

  if (verboseGMT_)
    std::cout << "Analyzing " << int(gmtCands.end(0) - gmtCands.begin(0)) << " GMT cands" << std::endl;

  for (int bx = gmtCands.getFirstBX(); bx <= gmtCands.getLastBX(); bx++ ){
    if ( bx < minBXGMT_ or bx > maxBXGMT_) continue;
    for (auto emtfCand = gmtCands.begin(bx); emtfCand != gmtCands.end(bx); ++emtfCand ){

      const gem::EMTFCand& gemtrk(*emtfCand);

      if (verboseGMT_)
        std::cout << gemtrk << std::endl;

      int tpidfound = -1;
      // check if it was matched to a simtrack
      for (int tpid = 0; tpid < MAX_PARTICLES; tpid++) {

        // get the matcher
        const auto& matcher = manager.matcher(tpid);

        // stop processing when the first invalid matcher is found
        if (matcher->isInValid()) break;

        const auto& trackMatch = matcher->l1Muons()->muon();
        if (trackMatch) {

          if (verboseGMT_)
            std::cout << "Candidate match " << *trackMatch
                      << std::endl;

          //check if the same
          if (gemtrk == *trackMatch) {
            tpidfound = tpid;
            std::cout << "...matched! With index " << tpidfound << std::endl;
            break;
          }
        }
      }
      trkTree.l1mu_pt->push_back(gemtrk.pt());
      trkTree.l1mu_eta->push_back(gemtrk.eta());
      trkTree.l1mu_phi->push_back(gemtrk.phi());
      trkTree.l1mu_charge->push_back(gemtrk.charge());
      trkTree.l1mu_bx->push_back(gemtrk.bx());
      trkTree.l1mu_tpid->push_back(tpidfound);
    }
  }
}

  void L1MuAnalyzer::analyze(TreeManager& tree)
  {
  const auto& emtfTrack = match_->emtfTrack();
  const auto& muon = match_->muon();

  if (emtfTrack != nullptr) {
    tree.l1mu().has_emtfTrack = 1;
    tree.l1mu().emtf_pt = emtfTrack->pt();
    tree.l1mu().emtf_eta = emtfTrack->eta();
    tree.l1mu().emtf_phi = emtfTrack->phi();

    tree.l1mu().hasME1 = emtfTrack->hasStub(1);
    tree.l1mu().hasME2 = emtfTrack->hasStub(2);
    tree.l1mu().hasME3 = emtfTrack->hasStub(3);
    tree.l1mu().hasME4 = emtfTrack->hasStub(4);
    tree.l1mu().nstubs = emtfTrack->nStubs();
    tree.l1mu().deltaR = emtfTrack->dR();
    tree.l1mu().chargesign = emtfTrack->charge();
  }

  if (muon != nullptr) {
    tree.l1mu().has_gmtCand = 1;
    tree.l1mu().bestdRGmtCand = muon->dR();
    tree.l1mu().L1Mu_pt = muon->pt();
    tree.l1mu().L1Mu_eta = muon->eta();
    tree.l1mu().L1Mu_phi = muon->phi();
    tree.l1mu().L1Mu_charge = muon->charge();
    tree.l1mu().L1Mu_bx = muon->bx();
    tree.l1mu().L1Mu_quality = muon->quality();
  }
}
