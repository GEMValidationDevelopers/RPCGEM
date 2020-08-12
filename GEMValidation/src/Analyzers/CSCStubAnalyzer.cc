#include "GEMCode/GEMValidation/interface/Analyzers/CSCStubAnalyzer.h"
#include "GEMCode/GEMValidation/interface/Helpers.h"
#include "L1Trigger/CSCCommonTrigger/interface/CSCPatternLUT.h"
#include "L1Trigger/CSCTriggerPrimitives/interface/CSCComparatorCodeLUT.h"

CSCStubAnalyzer::CSCStubAnalyzer(const edm::ParameterSet& conf)
{
  minNHitsChamber_ = conf.getParameter<int>("minNHitsChamberCSCStub");
}

void CSCStubAnalyzer::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iSetup.get<MuonGeometryRecord>().get(csc_geom_);
  if (csc_geom_.isValid()) {
  cscGeometry_ = &*csc_geom_;
  } else {
    std::cout << "+++ Info: CSC geometry is unavailable. +++\n";
  }

  iSetup.get<MuonGeometryRecord>().get(gem_geom_);
  if (gem_geom_.isValid()) {
  gemGeometry_ = &*gem_geom_;
  } else {
    std::cout << "+++ Info: GEM geometry is unavailable. +++\n";
  }
}

void CSCStubAnalyzer::setMatcher(const CSCStubMatcher& match_sh)
{
  match_.reset(new CSCStubMatcher(match_sh));
}

void CSCStubAnalyzer::analyze(TreeManager& tree)
{
  // CSC CLCTs
  for(const auto& d: match_->chamberIdsCLCT(0)) {
    CSCDetId id(d);

    const int st(gem::detIdToMEStation(id.station(),id.ring()));

    const bool odd(id.chamber()%2==1);
    const auto& clct = match_->bestClctInChamber(d);

    if (!clct.isValid()) continue;

    const float slope(getSlope(clct));
    std::cout << id << " " << clct << std::endl;

    int deltaStrip = 0;
    if (id.station() == 1 and id.ring() == 4 and clct.getKeyStrip() > CSCConstants::MAX_HALF_STRIP_ME1B)
      deltaStrip = CSCConstants::MAX_NUM_STRIPS_ME1B;

    auto fill = [clct, odd, slope, tree, deltaStrip](gem::CSCStubStruct& cscStubTree, int st) mutable {
      if (odd) {
        cscStubTree.has_clct_odd[st] = true;
        cscStubTree.quality_clct_odd[st] = clct.getQuality();
        cscStubTree.bx_clct_odd[st] = clct.getBX();
        cscStubTree.hs_clct_odd[st] = clct.getKeyStrip();
        cscStubTree.qs_clct_odd[st] = clct.getKeyStrip(4);
        cscStubTree.es_clct_odd[st] = clct.getKeyStrip(8);
        cscStubTree.slope_clct_odd[st] = slope;
        cscStubTree.fhs_clct_odd[st] = clct.getFractionalStrip(2);
        cscStubTree.fqs_clct_odd[st] = clct.getFractionalStrip(4);
        cscStubTree.fes_clct_odd[st] = clct.getFractionalStrip(8);
        // deltas
        cscStubTree.delta_fhs_clct_odd[st] = cscStubTree.fhs_clct_odd[st] - deltaStrip - tree.cscSimHit().strip_csc_sh_odd[st];
        cscStubTree.delta_fqs_clct_odd[st] = cscStubTree.fqs_clct_odd[st] - deltaStrip - tree.cscSimHit().strip_csc_sh_odd[st];
        cscStubTree.delta_fes_clct_odd[st] = cscStubTree.fes_clct_odd[st] - deltaStrip - tree.cscSimHit().strip_csc_sh_odd[st];
        std::cout << "delta hs " << cscStubTree.delta_fhs_clct_odd[st] << " hs " << cscStubTree.fhs_clct_odd[st] - deltaStrip << " true " << tree.cscSimHit().strip_csc_sh_odd[st] << std::endl;
        std::cout << "delta qs " << cscStubTree.delta_fqs_clct_odd[st] << " qs " << cscStubTree.fqs_clct_odd[st] - deltaStrip << " true " << tree.cscSimHit().strip_csc_sh_odd[st] << std::endl;
        std::cout << "delta es " << cscStubTree.delta_fes_clct_odd[st] << " es " << cscStubTree.fes_clct_odd[st] - deltaStrip << " true " << tree.cscSimHit().strip_csc_sh_odd[st] << std::endl;
      }
      else {
        cscStubTree.has_clct_even[st] = true;
        cscStubTree.quality_clct_even[st] = clct.getQuality();
        cscStubTree.bx_clct_even[st] = clct.getBX();
        cscStubTree.hs_clct_even[st] = clct.getKeyStrip();
        cscStubTree.qs_clct_even[st] = clct.getKeyStrip(4);
        cscStubTree.es_clct_even[st] = clct.getKeyStrip(8);
        cscStubTree.slope_clct_even[st] = slope;
        cscStubTree.fhs_clct_even[st] = clct.getFractionalStrip(2);
        cscStubTree.fqs_clct_even[st] = clct.getFractionalStrip(4);
        cscStubTree.fes_clct_even[st] = clct.getFractionalStrip(8);
        // deltas
        cscStubTree.delta_fhs_clct_even[st] = cscStubTree.fhs_clct_even[st] - deltaStrip - tree.cscSimHit().strip_csc_sh_even[st];
        cscStubTree.delta_fqs_clct_even[st] = cscStubTree.fqs_clct_even[st] - deltaStrip - tree.cscSimHit().strip_csc_sh_even[st];
        cscStubTree.delta_fes_clct_even[st] = cscStubTree.fes_clct_even[st] - deltaStrip - tree.cscSimHit().strip_csc_sh_even[st];
        std::cout << "delta hs " << cscStubTree.delta_fhs_clct_even[st] << " hs " << cscStubTree.fhs_clct_even[st] - deltaStrip << " true " << tree.cscSimHit().strip_csc_sh_even[st] << std::endl;
        std::cout << "delta qs " << cscStubTree.delta_fqs_clct_even[st] << " qs " << cscStubTree.fqs_clct_even[st] - deltaStrip << " true " << tree.cscSimHit().strip_csc_sh_even[st] << std::endl;
        std::cout << "delta es " << cscStubTree.delta_fes_clct_even[st] << " es " << cscStubTree.fes_clct_even[st] - deltaStrip << " true " << tree.cscSimHit().strip_csc_sh_even[st] << std::endl;
      }
    };

    fill(tree.cscStub(), st);
    // case ME11
    if (st==1 or st==2) {
      fill(tree.cscStub(), 0);
    }
  }

  // CSC ALCTs
  for(const auto& d: match_->chamberIdsALCT(0)) {
    CSCDetId id(d);
    const int st(gem::detIdToMEStation(id.station(),id.ring()));

    const bool odd(id.chamber()%2==1);
    const auto& alct = match_->bestAlctInChamber(d);

    if (!alct.isValid()) continue;

    auto fill = [alct, odd](gem::CSCStubStruct& cscStubTree, int st) mutable {
      if (odd) {
        cscStubTree.has_alct_odd[st] = true;
        cscStubTree.wg_alct_odd[st] = alct.getKeyWG();
        cscStubTree.quality_alct_odd[st] = alct.getQuality();
        cscStubTree.bx_alct_odd[st] = alct.getBX();
      }
      else {
        cscStubTree.has_alct_even[st] = true;
        cscStubTree.wg_alct_even[st] = alct.getKeyWG();
        cscStubTree.quality_alct_even[st] = alct.getQuality();
        cscStubTree.bx_alct_even[st] = alct.getBX();
      }
    };

    fill(tree.cscStub(), st);
    // case ME11
    if (st==1 or st==2){
      fill(tree.cscStub(), 0);
    }
  }

  // CSC LCTs
  for(const auto& d: match_->chamberIdsLCT(0)) {
    CSCDetId id(d);
    const int st(gem::detIdToMEStation(id.station(),id.ring()));
    const int gemstation(id.station());

    const auto& lct = match_->bestLctInChamber(d);

    if (!lct.isValid()) continue;

    const GlobalPoint& gp = match_->getGlobalPosition(d, lct);

    const bool odd(id.chamber()%2==1);

    auto fill = [lct, gp, odd](gem::CSCStubStruct& cscStubTree, int st) mutable {
      if (odd) {
        cscStubTree.has_lct_odd[st] = true;
        cscStubTree.bend_lct_odd[st] = lct.getPattern();
        cscStubTree.phi_lct_odd[st] = gp.phi();
        cscStubTree.eta_lct_odd[st] = gp.eta();
        cscStubTree.perp_lct_odd[st] = gp.perp();
        cscStubTree.bx_lct_odd[st] = lct.getBX();
        cscStubTree.hs_lct_odd[st] = lct.getStrip();
        cscStubTree.qs_lct_odd[st] = lct.getStrip(4);
        cscStubTree.es_lct_odd[st] = lct.getStrip(8);
        cscStubTree.wg_lct_odd[st] = lct.getKeyWG();
        cscStubTree.quality_lct_odd[st] = lct.getQuality();
      }
      else {
        cscStubTree.has_lct_even[st] = true;
        cscStubTree.bend_lct_even[st] = lct.getPattern();
        cscStubTree.phi_lct_even[st] = gp.phi();
        cscStubTree.eta_lct_even[st] = gp.eta();
        cscStubTree.perp_lct_even[st] = gp.perp();
        cscStubTree.bx_lct_even[st] = lct.getBX();
        cscStubTree.hs_lct_even[st] = lct.getStrip();
        cscStubTree.qs_lct_odd[st] = lct.getStrip(4);
        cscStubTree.es_lct_odd[st] = lct.getStrip(8);
        cscStubTree.wg_lct_even[st] = lct.getKeyWG();
        cscStubTree.quality_lct_even[st] = lct.getQuality();
      }
    };

    fill(tree.cscStub(), st);
    // case ME11
    if (st==1 or st==2){
      fill(tree.cscStub(), 0);
    }

    // only for ME1/1 and ME2/1
    if (st==1 or st==2 or st==5) continue;

    // require at least one GEM pad present...
    if (! (tree.gemStub().has_gem_pad_even[gemstation] or
           tree.gemStub().has_gem_pad_odd[gemstation] ) ) continue;

    if (odd) {
      tree.gemStub().dphi_lct_pad1_odd[gemstation]
        = reco::deltaPhi(float(tree.gemStub().phi_pad1_odd[st]),
                         float(tree.cscStub().phi_lct_odd[gemstation]));
      tree.gemStub().dphi_lct_pad2_odd[gemstation]
        = reco::deltaPhi(float(tree.gemStub().phi_pad2_odd[st]),
                         float(tree.cscStub().phi_lct_odd[gemstation]));
    } else {
      tree.gemStub().dphi_lct_pad1_even[gemstation]
        = reco::deltaPhi(float(tree.gemStub().phi_pad1_even[st]),
                         float(tree.cscStub().phi_lct_even[gemstation]));
      tree.gemStub().dphi_lct_pad2_even[gemstation]
        = reco::deltaPhi(float(tree.gemStub().phi_pad2_even[st]),
                         float(tree.cscStub().phi_lct_even[gemstation]));
    }

    // require at least one GEM pad present...
    if (! (tree.gemStub().has_gem_copad_even[gemstation] or
           tree.gemStub().has_gem_copad_odd[gemstation] ) ) continue;

    if (odd) {
      tree.gemStub().dphi_lct_copad_odd[gemstation]
        = reco::deltaPhi(float(tree.gemStub().phi_copad_odd[st]),
                         float(tree.cscStub().phi_lct_odd[gemstation]));
    } else {
      tree.gemStub().dphi_lct_copad_even[gemstation]
        = reco::deltaPhi(float(tree.gemStub().phi_copad_even[st]),
                         float(tree.cscStub().phi_lct_even[gemstation]));
    }
  }
}

float CSCStubAnalyzer::getSlope(const CSCCLCTDigi& lct) const
{
  return getSlope(lct.getPattern(), lct.getCompCode());
}

float CSCStubAnalyzer::getSlope(int pattern, int compCode) const
{
  if (compCode == -1) return getAverageSlopeLegacy(pattern);
  else return getSlopeRun3(pattern, compCode);
}

float CSCStubAnalyzer::getMaxSlopeLegacy(int pattern) const
{
  // slope in number of strips/layer
  int slope[CSCConstants::NUM_CLCT_PATTERNS] = {
    0, 0, 5, -5, 4, -4, 3, -3, 2, -2, 1};
  return float(slope[pattern]/5.);
}

float CSCStubAnalyzer::getMinSlopeLegacy(int pattern) const
{
  // slope in number of strips/layer
  int slope[CSCConstants::NUM_CLCT_PATTERNS] = {
    0, 0, 3, -3, 2, -2, 1, -1, 0, 0, -1};
  return float(slope[pattern]/5.);
}

float CSCStubAnalyzer::getAverageSlopeLegacy(int pattern) const
{
  // slope in number of strips/layer
  int slope[CSCConstants::NUM_CLCT_PATTERNS] = {
    0, 0, 4, -4, 3, -3, 2, -2, 1, -1, 0};
  return float(slope[pattern]/5.);
}

float CSCStubAnalyzer::getSlopeRun3(int pattern, int compCode) const
{
  // need to access the LUTs in CMSSW!
  std::string lutstring("L1Trigger/CSCTriggerPrimitives/data/CSCComparatorCodeSlopeLUT_pat" + std::to_string(pattern) + "_v1.txt");
  std::unique_ptr<CSCComparatorCodeLUT> lut(new CSCComparatorCodeLUT(lutstring));
  return lut->lookup(compCode);
}

std::pair<GEMDigi, GlobalPoint>
CSCStubAnalyzer::bestGEMDigi(const GEMDetId& gemId,
                             const GEMDigiContainer& gem_digis,
                             const GlobalPoint& csc_gp) const
{
  GlobalPoint gp;
  GEMDigi best_digi;
  auto emptyValue = make_pair(best_digi, gp);

  // no valid GEM digis
  if (gem_digis.empty()) return emptyValue;

  // invalid CSC stub
  if (std::abs(csc_gp.z()) < 0.001) return emptyValue;

  float bestDR2 = 999.;
  for (const auto& d: gem_digis) {
    const LocalPoint& lp = gemGeometry_->etaPartition(gemId)->centreOfStrip(d.strip());
    const GlobalPoint& gem_gp = gemGeometry_->idToDet(gemId)->surface().toGlobal(lp);

    // in deltaR calculation, give x20 larger weight to deltaPhi to make them comparable
    // but with slight bias towards dphi:
    float dphi = 20. * reco::deltaPhi(float(csc_gp.phi()), float(gem_gp.phi()));
    float deta = csc_gp.eta() - gem_gp.eta();
    float dR2 = dphi*dphi + deta*deta;
    // current gp is closer in phi then the previous
    if (dR2 < bestDR2) {
      gp = gem_gp;
      best_digi = d;
      bestDR2 = dR2;
    }
  }
  return emptyValue;
}


std::pair<GEMPadDigi, GlobalPoint>
CSCStubAnalyzer::bestGEMPadDigi(const GEMDetId& gemId,
                                const GEMPadDigiContainer& gem_digis,
                                const GlobalPoint& csc_gp) const
{
  GlobalPoint gp;
  GEMPadDigi best_digi;
  auto emptyValue = make_pair(best_digi, gp);

  // no valid GEM digis
  if (gem_digis.empty()) return emptyValue;

  // invalid CSC stub
  if (std::abs(csc_gp.z()) < 0.001) return emptyValue;

  float bestDR2 = 999.;
  for (const auto& d: gem_digis) {
    const LocalPoint& lp = gemGeometry_->etaPartition(gemId)->centreOfPad(d.pad());
    const GlobalPoint& gem_gp = gemGeometry_->idToDet(gemId)->surface().toGlobal(lp);

    // in deltaR calculation, give x20 larger weight to deltaPhi to make them comparable
    // but with slight bias towards dphi:
    float dphi = 20. * reco::deltaPhi(float(csc_gp.phi()), float(gem_gp.phi()));
    float deta = csc_gp.eta() - gem_gp.eta();
    float dR2 = dphi*dphi + deta*deta;
    // current gp is closer in phi then the previous
    if (dR2 < bestDR2) {
      gp = gem_gp;
      best_digi = d;
      bestDR2 = dR2;
    }
  }
  return emptyValue;
}


std::pair<GEMCoPadDigi, GlobalPoint>
CSCStubAnalyzer::bestGEMCoPadDigi(const GEMDetId& gemId,
                                  const GEMCoPadDigiContainer& gem_digis,
                                  const GlobalPoint& csc_gp) const
{
  GlobalPoint gp;
  GEMCoPadDigi best_digi;
  auto emptyValue = make_pair(best_digi, gp);

  // no valid GEM digis
  if (gem_digis.empty()) return emptyValue;

  // invalid CSC stub
  if (std::abs(csc_gp.z()) < 0.001) return emptyValue;

  float bestDR2 = 999.;
  for (const auto& d: gem_digis) {
    const LocalPoint& lp = gemGeometry_->etaPartition(gemId)->centreOfPad(d.pad(1));
    const GlobalPoint& gem_gp = gemGeometry_->idToDet(gemId)->surface().toGlobal(lp);

    // in deltaR calculation, give x20 larger weight to deltaPhi to make them comparable
    // but with slight bias towards dphi:
    float dphi = 20. * reco::deltaPhi(float(csc_gp.phi()), float(gem_gp.phi()));
    float deta = csc_gp.eta() - gem_gp.eta();
    float dR2 = dphi*dphi + deta*deta;
    // current gp is closer in phi then the previous
    if (dR2 < bestDR2) {
      gp = gem_gp;
      best_digi = d;
      bestDR2 = dR2;
    }
  }
  return emptyValue;
}
