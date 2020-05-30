#include "GEMCode/GEMValidation/interface/Analyzers/CSCDigiAnalyzer.h"

CSCDigiAnalyzer::CSCDigiAnalyzer(const edm::ParameterSet& conf)
{
  minNHitsChamber_ = conf.getParameter<int>("minNHitsChamberCSCDigi");
}

void CSCDigiAnalyzer::init(const CSCDigiMatcher& match_sh)
{
  match_.reset(new CSCDigiMatcher(match_sh));
}

void CSCDigiAnalyzer::analyze(TreeManager& tree)
{
  /*
  // CSC strip digis
  for(const auto& d: match_->chamberIdsStrip(0)) {
    CSCDetId id(d);

    cout << "test digi analysis" << endl;

    const int st(gem::detIdToMEStation(id.station(),id.ring()));
    cout << "use this station? " << st << " "  << endl;

    const int nlayers(match_->nLayersWithStripInChamber(d));
    cout << "use this station " << st << " " << stt << " " << nlayers << " " << minNHitsChamber_ << endl;

    if (nlayers < minNHitsChamber_) continue;

    cout << "use this station " << st << " " << stt << " " << nlayers << " " << minNHitsChamber_ << endl;
    cout << "has strip digis " << endl;

    const bool odd(id.chamber()%2==1);

    if (odd) track[stt].has_csc_strips_odd = true;
    else     track[stt].has_csc_strips_even = true;

    if (odd) track[stt].nlayers_st_dg_odd = nlayers;
    else     track[stt].nlayers_st_dg_even = nlayers;

    // case ME11
    if (st==2 or st==3){
      if (odd) track[1].has_csc_strips_odd = true;
      else     track[1].has_csc_strips_even = true;

      if (odd) track[1].nlayers_st_dg_odd = nlayers;
      else     track[1].nlayers_st_dg_even = nlayers;
    }
  }

  // CSC wire digis
  for(const auto& d: match_->chamberIdsWire(0)) {
    CSCDetId id(d);
    const int st(gem::detIdToMEStation(id.station(),id.ring()));
    if (std::find(stations_to_use_.begin(), stations_to_use_.end(), st) == stations_to_use_.end()) continue;

    const int stt( std::find(stations_to_use_.begin(), stations_to_use_.end(), st) - stations_to_use_.begin());

    const int nlayers(match_->nLayersWithWireInChamber(d));
    if (nlayers < minNHitsChamber_) continue;

    const bool odd(id.chamber()%2==1);

    if (odd) track[stt].has_csc_wires_odd = true;
    else track[stt].has_csc_wires_even = true;

    if (odd) track[stt].nlayers_wg_dg_odd = nlayers;
    else track[stt].nlayers_wg_dg_even = nlayers;

    // case ME11
    if (st==2 or st==3){
      if (odd) track[1].has_csc_wires_odd = true;
      else track[1].has_csc_wires_even = true;

      if (odd) track[1].nlayers_wg_dg_odd = nlayers;
      else track[1].nlayers_wg_dg_even = nlayers;
    }
  }
  */
}
