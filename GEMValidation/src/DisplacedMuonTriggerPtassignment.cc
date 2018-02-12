#ifndef GEMCode_GEMValidation_DisplacedMuonTriggerPtassignment_cc
#define GEMCode_GEMValidation_DisplacedMuonTriggerPtassignment_cc

/**\class DisplacedMuonTriggerPtassignment

  Displaced Muon Trigger Design based on Muon system

  Author: tao.huang@cern.ch, sven.dildick@cern.ch

*/
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
#include "GEMCode/GEMValidation/interface/DisplacedMuonTriggerPtassignment.h"
#include "GEMCode/GEMValidation/interface/PtassignmentHelper.h"
#include "GEMCode/GEMValidation/interface/CSCStubPatterns.h"

//endcap, we need LCTs and associated cscid, gempads and associated gemid, and all gemometry
//to get position from fitting, we also need all comparator digis
//step0 get LCTs and associated cscids, GEMPads and associated gemids, and geometry.
//step1 get fitting positions from fitting compara digis after assoicating comparator digis to LCTs
//step2 calculate all variables used pt in assignment, requires eta,phi,radius,Z
//step3 assgin L1 pt according to LUTs (in short future)

/*
DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(const CSCCorrelatedLCTDigiCollection* lcts, const edm::EventSetup& es, const edm::Event& ev)
  : ev_(ev), es_(es), verbose_(0)
{
  setupGeometry(es);
}

DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(const CSCCorrelatedLCTDigiContainer lcts, const CSCDetIdContainer cscids, const edm::EventSetup& es, const edm::Event& ev)
  : ev_(ev), es_(es), verbose_(0)
{
  setupGeometry(es);
}*/

//chamberid_lcts: LCTs matched to simmuon and their associated chamberid, detid_pads: gempads matched to simmuon and their associated detid_pads
DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(std::map<unsigned int, CSCCorrelatedLCTDigiContainer> chamberid_lcts,
								   //std::map<unsigned int, GEMPadDigiContainer> detid_pads,
								   edm::EDGetTokenT<GEMPadDigiCollection>& gemPadDigiInput_,
								   edm::EDGetTokenT<ME0SegmentCollection>& me0SegmentInput_,
								   const edm::ParameterSet& ps,
								   const edm::EventSetup& es,
								   const edm::Event& ev)
  : ps_(ps), ev_(ev), es_(es)
{
  setupGeometry(es);
  chamberid_lcts_ = chamberid_lcts;
  //detid_pads_ = detid_pads;
  ev.getByLabel("simMuonCSCDigis", "MuonCSCComparatorDigi", hCSCComparators);
  minGEMCSCdPhi_ = ps_.getParameter<double>("minGEMCSCdPhi");
  minGEMCSCdEta_ = ps_.getParameter<double>("minGEMCSCdEta");
  verbose_ = ps_.getParameter<int>("verbose");

  //me0SegmentInput_ = consumes<ME0SegmentCollection>(ps_.getParameter<edm::InputTag>("me0SegmentInput"));
  me0MinEta_ = ps_.getParameter<double>("me0MinEta");//eta range to use ME0

  initVariables();
  for (const auto& idlcts : chamberid_lcts_){
    CSCDetId chid(idlcts.first);
    for (const auto& stub: idlcts.second){
	if (stub.isValid()) hasStub_st[chid.station()-1] =true;
	else {
	    std::cout <<"chid "<< chid <<" stub " << stub <<" Not valid "<< std::endl;
	}
    }
    if (not hasStub_st[chid.station()-1]) {
	    std::cout <<" chid "<< chid <<"  number of lcts "<< idlcts.second.size() <<" NO valid stub "<< std::endl;
	    continue;
    }

    int endcap(chid.endcap() == 1 ? 1:-1);
    //check the ring number that muon is flying through, 2nd station as reference
    //later use this one to check whether we should use GE21 or ME21only
    if (chid.station()==2) meRing = chid.ring();
    //record ring number in station1
    if (chid.station()==1) meRing_st1 = chid.ring();
    //chamber parity in station3 should be the same as in station2,
    //if there is already qualified one, the skip stubs in station 3
    if (chid.station()==2 and hasStub_st[1] and hasStub_st[2] and
	   ((isEven[1] and isEven[2]) or (not(isEven[1]) and not(isEven[2]))))
	continue;
    if (chid.chamber()%2 == 0) isEven[chid.station()-1] = true;
    if (idlcts.second.size()>1 and verbose_>0)
      std::cout <<"more than one LCT  available in chamber id "<< chid <<" LCT size "<< idlcts.second.size()<<std::endl;
    globalPositionOfLCT(idlcts.second, chid);
    //find GEMPads
    if ((chid.station() == 1 and (chid.ring()==1 or chid.ring()==4) and fabs(gp_st_layer3[0].eta()) <= me0MinEta_)
	or (chid.station()==2 and chid.ring()==1)){

      /*for (const auto& idgempads : detid_pads){
        GEMDetId gemid(idgempads.first);
        if (endcap*gemid.region() > 0 and
	    (chid.station() == gemid.station())
            and chid.chamber() == gemid.chamber()){

	  if (idgempads.second.size() ==0 ) continue;
          //matching GEMPads to CSC stubs
          globalPositionOfGEMPad(idgempads.second, gemid);
          if (gemid.station() == 1 and fabs(dphi_gemcsc_st[0]) < minGEMCSCdPhi_) {
	      hasGEMPad_st1 = true;
	  }else if (gemid.station() == 2 and fabs(dphi_gemcsc_st[1]) < minGEMCSCdPhi_) {
	      hasGEMPad_st2 = true;
	  }else {
	      ;//do nothing here
	  }

        }
      }*/
      GEMDetId id(endcap, 1, chid.station(), 1, chid.chamber(), 0);//GEM superchamber
      edm::Handle<GEMPadDigiCollection> gem_pads;
      if (gemvalidation::getByToken(gemPadDigiInput_, gem_pads, ev_))
        matchGEMPadsToTrack(*gem_pads.product(), id);
      else
        if (verbose_>0) std::cout <<"error failed to readout GEMPad "<< std::endl;
      if (not hasGEMPad_st[chid.station() - 1]) std::cout <<"failed to find matched GEM Pad, station "<< chid.station() << std::endl;

    }

    if (chid.station() == 1 and (chid.ring()==1 or chid.ring()==4) and fabs(gp_st_layer3[0].eta()) > me0MinEta_-0.01){
      //ME0DetId id(endcap, 0, chid.chamber()/2, 0);//region, layer, chamber, roll
      edm::Handle<ME0SegmentCollection> me0_segments;
      if (gemvalidation::getByToken(me0SegmentInput_, me0_segments, ev_))
        //matchME0SegmentsToTrack(*me0_segments.product(), id);
        matchME0SegmentsToTrack(*me0_segments.product());
      else
        if (verbose_>0) std::cout <<"error failed to read out ME0Segment "<< std::endl;
      if (not hasME0)
        if (verbose_>0) std::cout <<"failed to find matched ME0 segment "<< std::endl;
    }

  }

  nstubs = 0;
  for (int i=0; i<4; i++)
  	if (hasStub_st[i]){
	    nstubs++;
	    radius_st_ME[i] = gp_st_layer3[i].perp();
	}



  //npar>=0 is the flag to do pt assignment
  if (hasStub_st[0] and hasStub_st[1] and hasStub_st[2]){
  	if (not(isEven[0]) and isEven[1] and isEven[2]) npar = 0;
    else if (not(isEven[0]) and not(isEven[1]) and not(isEven[2])) npar = 1;
    else if (isEven[0] and isEven[1] and isEven[2]) npar = 2;
    else if (isEven[0] and not(isEven[1]) and not(isEven[2])) npar = 3;
    else {
	    if (verbose_>0) std::cout <<" hasStub in station 1 2 3  but npar = -1 "<< std::endl;
	    npar= -1;
      for (const auto& idlcts : chamberid_lcts_){
        CSCDetId chid(idlcts.first);
        if (verbose_>0) std::cout <<"CSC id "<< chid <<" LCT "<< idlcts.second[0] << std::endl;
	    }
    }
  }else
      npar = -1;

  if (nstubs >= 3){
  	//find fitting radius
     fitTrackRadius(gp_st_layer3, radius_st_ME);
  	//reset gp_st_layer3
     for (int i=0; i<4; i++){
  	if (not(hasStub_st[i])) continue;
	float phi = gp_st_layer3[i].phi();
	float z = gp_st_layer3[i].z();
	gp_st_layer3[i] = GlobalPoint(GlobalPoint::Cylindrical(radius_st_ME[i], phi, z));
     }
  }

  //second station
  if (hasStub_st[1]) eta_st2 = gp_st_layer3[1].eta();

  if (hasStub_st[0] and hasStub_st[1])
	xfactor = (gp_st_layer3[1].perp()/gp_st_layer3[0].perp()-1.0)/fabs(gp_st_layer3[0].z()-gp_st_layer3[1].z());

}

DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(const l1t::EMTFTrack& tftrack,
                                                                   const l1t::EMTFTrackCollection& l1Tracks,
                                                                   const CSCCorrelatedLCTDigiCollection& CSCCorrelatedLCTs,
                                                                   bool doStubRecovery,
                                                                   bool matchGEMPads,
								   const edm::ParameterSet& ps,
                                                                   const edm::EventSetup& es,
                                                                   const edm::Event& ev)
  : ps_(ps), ev_(ev), es_(es)
{
  setupGeometry(es);
  initVariables();
  verbose_ = ps_.getParameter<int>("verbose");

  // first step: collect all stubs associated to the CSC TF Track
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> chamberid_lct;
  const auto& stubCollection = tftrack.Hits();
  for (const auto& hit: stubCollection) {
    if (not hit.Is_CSC()) continue;
    const CSCDetId& ch_id = hit.CSC_DetId()();
    // empty vector for stubs
    CSCCorrelatedLCTDigiContainer v;
    v.push_back(hit.CSC_LCTDigi());
    hasStub_st[ch_id.station()-1] = true;
    chamberid_lct[ch_id.rawId()] = v;
  }


  const int csctf_bx = tftrack.BX();
  const float csctf_eta = tftrack.Eta();
  // const float csctf_phi = normalizedPhi(tftrack.Phi_glob_rad());

  // second step: stub recovery
  bool stubMissingSt1 = not hasStub_st[0];
  bool stubMissingSt2 = not hasStub_st[1];
  bool stubMissingSt3 = not hasStub_st[2];
  bool stubMissingSt4 = not hasStub_st[3];
  bool atLeast1StubMissing = stubMissingSt1 or stubMissingSt2 or stubMissingSt3 or stubMissingSt4;

  if (doStubRecovery and atLeast1StubMissing){
    int triggerSector = tftrack.Sector();

    for (int endcap=1; endcap<=2; endcap++){
      // do not consider stubs in the wrong endcap
      int zendcap(endcap!=1 ? -1 : +1 );
      if (zendcap * csctf_eta < 0) continue;
      for (int station=1; station<=4; station++){

        // ignore station where a L1Mu stub is present!
        if (not stubMissingSt1 and station==1) continue;
        if (not stubMissingSt2 and station==2) continue;
        if (not stubMissingSt3 and station==3) continue;
        if (not stubMissingSt4 and station==4) continue;
        // if(verbose) std::cout << "Recovered stubs in station: " << station << std::endl;
        // temp storage of candidate stubs per station and ring
        CSCCorrelatedLCTDigiId bestMatchingStub;
        int iStub = 0;
        for (int ring=1; ring<=3; ring++){
          if (station!=1 and ring==3) continue;
          //std::cout << "Analyzing ring " << ring << std::endl;

          for (int chamber=1; chamber<=36; chamber++){
            // do not consider invalid detids
            if ( (station==2 or station==3 or station==4) and
                 (ring==1) and chamber>18) continue;
            //std::cout << "Analyzing chamber " << chamber << std::endl;
            // create the detid
            CSCDetId ch_id(endcap, station, ring, chamber);
            //std::cout << "ch_id " <<  ch_id << std::endl;
            // get the stubs in this detid
            const auto& range = CSCCorrelatedLCTs.get(ch_id);
            for (auto digiItr = range.first; digiItr != range.second; ++digiItr){
              iStub++;

              const auto& stub(*digiItr);

              // check that this stub is not already part of a CSC TF track
              if (stubInCSCTFTracks(stub, l1Tracks)) continue;

              // trigger sector must be the same
              if (triggerSector != ch_id.triggerSector()) continue;

              // BXs have to match
              int deltaBX = std::abs(stub.getBX() - (6 + csctf_bx));
              if (deltaBX > 1) continue;

              if(verbose()>2) std::cout << ch_id << std::endl;
              if(verbose()>2) std::cout<<"Candidate " << stub << std::endl;
               // bestMatchingStub = pickBestMatchingStub(allxs[ch_id.station()-1], allys[ch_id.station()-1],
               //                                         bestMatchingStub, std::make_pair(ch_id, stub), 6 + csctf_bx);
             }
             // consider the case ME1a
             if (station==1 and ring==1){
               CSCDetId me1a_id(endcap, station, 4, chamber);
               const auto& range = CSCCorrelatedLCTs.get(me1a_id);
               for (auto digiItr = range.first; digiItr != range.second; ++digiItr){
                 iStub++;
                 const auto& stub(*digiItr);

                 // check that this stub is not already part of a CSC TF track
                 if (stubInCSCTFTracks(stub, l1Tracks)) continue;

                 // trigger sector must be the same
                 if (triggerSector != me1a_id.triggerSector()) continue;

                 // BXs have to match
                 int deltaBX = std::abs(stub.getBX() - (6 + csctf_bx));
                 if (deltaBX > 1) continue;

                 if(verbose()>2) std::cout << me1a_id << std::endl;
                 if(verbose()>2) std::cout<<"Candidate " << stub << std::endl;
                 // bestMatchingStub = pickBestMatchingStub(allxs[me1a_id.station()-1], allys[me1a_id.station()-1],
                 //                                         bestMatchingStub, std::make_pair(me1a_id, stub), 6 + csctf_bx);
               }
             }
          }
        }
      }
    }
  }

  // third step: add GEM pads
  if (matchGEMPads) {

    // GEMPadDigiId bestPad_GE11_L1;
    // GEMPadDigiId bestPad_GE11_L2;
    // GEMPadDigiId bestPad_GE21_L1;
    // GEMPadDigiId bestPad_GE21_L2;

    // FIXME
  }
}


DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(GlobalPoint gp1,
								   GlobalPoint gp2,
								   GlobalPoint gp3,
								   GlobalPoint gp4,
								   GlobalPoint gp5,
								   GlobalPoint gp6,
								   int npar_in,
								   const edm::ParameterSet& ps,
								   const edm::EventSetup& es,
								   const edm::Event& ev)
: ps_(ps), ev_(ev), es_(es)
{ //sim level

  setupGeometry(es);
  initVariables();
  verbose_ = ps_.getParameter<int>("verbose");


  gp_st_layer3[0] = GlobalPoint(gp1);
  //use z>100 to make sure this is valid globalpoint
  if (fabs(gp1.z())>100) hasStub_st[0] = true;
  else if (verbose_>0)
      std::cout <<" gp_st1 x "<< gp1.x()<<" y "<< gp1.y()<<" z "<< gp1.z()<< " eta "<< gp1.eta()<<" phi "<< gp1.phi()<< std::endl;
  gp_st_layer3[1] = GlobalPoint(gp2);
  if (fabs(gp2.z())>100) hasStub_st[1] = true;
  else if (verbose_>0)
      std::cout <<" gp_st2 x "<< gp2.x()<<" y "<< gp2.y()<<" z "<< gp2.z()<< " eta "<< gp2.eta()<<" phi "<< gp2.phi()<< std::endl;
  gp_st_layer3[2] = GlobalPoint(gp3);
  if (fabs(gp3.z())>100) hasStub_st[2] = true;
  else if (verbose_>0)
      std::cout <<" gp_st3 x "<< gp3.x()<<" y "<< gp3.y()<<" z "<< gp3.z()<< " eta "<< gp3.eta()<<" phi "<< gp3.phi()<< std::endl;
  gp_st_layer3[3] = GlobalPoint(gp4);
  if (fabs(gp4.z())>100) hasStub_st[3] = true;

  gp_ge[0] = GlobalPoint(gp5);
  if (fabs(gp_ge[0].z())>100) hasGEMPad_st[0] = true;
  else if (verbose_>0)
      std::cout <<" gp_ge11 x "<< gp_ge[0].x()<<" y "<< gp_ge[0].y()<<" z "<< gp_ge[0].z()<< " eta "<< gp_ge[0].eta()<<" phi "<< gp_ge[0].phi()<< std::endl;
  gp_ge[1] = GlobalPoint(gp6);
  if (fabs(gp_ge[1].z())>100) hasGEMPad_st[1] = true;
  else if (verbose_>0)
      std::cout <<" gp_ge21 x "<< gp_ge[1].x()<<" y "<< gp_ge[1].y()<<" z "<< gp_ge[1].z()<< " eta "<< gp_ge[1].eta()<<" phi "<< gp_ge[1].phi()<< std::endl;

  if (hasStub_st[0] and hasStub_st[1])
  	npar = npar_in;
  else
      npar = -1;

  if (hasGEMPad_st[0])
  	phi_gem[0] = gp_ge[0].phi();
  if (hasGEMPad_st[1])
  	phi_gem[1] = gp_ge[1].phi();

  if (hasStub_st[1] and fabs(gp2.eta())<1.6 and fabs(gp2.eta())>=1.2 and not(hasGEMPad_st[1]))
      meRing =2;
  else if (hasStub_st[1] and fabs(gp2.eta())<=2.4 and fabs(gp2.eta())>=1.6)
      meRing =1;

  if (hasStub_st[0] and hasStub_st[1])
	xfactor = (gp_st_layer3[1].perp()/gp_st_layer3[0].perp()-1.0)/fabs(gp_st_layer3[0].z()-gp_st_layer3[1].z());
  //second station
  if (hasStub_st[1]) eta_st2 = gp_st_layer3[1].eta();

}


//DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(){ //test constructor
//}

DisplacedMuonTriggerPtassignment::DisplacedMuonTriggerPtassignment(const L1MuDTTrackSegPhiContainer& tracks,
								   const edm::ParameterSet& ps,
                                                                   const edm::EventSetup& es,
                                                                   const edm::Event& ev)
  : ps_(ps), ev_(ev), es_(es)
{
  setupGeometry(es);
  initVariables();
  verbose_ = ps_.getParameter<int>("verbose");
}


DisplacedMuonTriggerPtassignment::~DisplacedMuonTriggerPtassignment(){


}

void DisplacedMuonTriggerPtassignment::initVariables()
{
  eta_st2 = -9;
  dphi_me0_st1 = -9;
  hasME0 = false;
  hasGEMPad_st[0] = false;
  hasGEMPad_st[1] = false;

  npar = -1;
  meRing = -1;
  //position-based
  ddY123 = -99;
  deltaY12 = -99;
  deltaY23 = -99;

  //direction-based
  phiM_st1 = -9;
  phiM_st2 = -9;
  phiM_st12 = -9;
  phiM_st23 = -9;
  dPhi_dir_st1_st2 = -9;
  dPhi_dir_st1_st12 = -9;
  dPhi_dir_st2_st23 = -9;
  dPhi_dir_st12_st23 = -9;

  position_pt = 0.0;
  direction_pt = 0.0;
  hybrid_pt = 0.0;
  /// barrel
  has_stub_mb1 = false;
  has_stub_mb2 = false;
  has_stub_mb3 = false;
  has_stub_mb4 = false;
  phi_mb1 = -9;
  phi_mb2 = -9;
  phi_mb3 = -9;
  phi_mb4 = -9;
  phib_mb1 = -9;
  phib_mb2 = -9;
  phib_mb3 = -9;
  phib_mb4 = -9;
}


void DisplacedMuonTriggerPtassignment::setupGeometry(const edm::EventSetup& es)
{

  /// get the geometry
  hasGEMGeometry_ = true;
  hasRPCGeometry_ = true;
  hasCSCGeometry_ = true;
  hasME0Geometry_ = true;
  hasDTGeometry_ = true;

  es.get<MuonGeometryRecord>().get(gem_geom_);
  if (gem_geom_.isValid()) {
    gemGeometry_ = &*gem_geom_;
  } else {
    hasGEMGeometry_ = false;
    std::cout << "+++ Info: GEM geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(me0_geom_);
  if (me0_geom_.isValid()) {
    me0Geometry_ = &*me0_geom_;
  } else {
    hasME0Geometry_ = false;
    std::cout << "+++ Info: ME0 geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(rpc_geom_);
  if (rpc_geom_.isValid()) {
    rpcGeometry_ = &*rpc_geom_;
  } else {
    hasRPCGeometry_ = false;
    std::cout << "+++ Info: RPC geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(dt_geom_);
  if (dt_geom_.isValid()) {
    dtGeometry_ = &*dt_geom_;
  } else {
    hasDTGeometry_ = false;
    std::cout << "+++ Info: DT geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(csc_geom_);
  if (csc_geom_.isValid()) {
    cscGeometry_ = &*csc_geom_;
  } else {
    hasCSCGeometry_ = false;
    std::cout << "+++ Info: CSC geometry is unavailable. +++\n";
  }
}


//void DisplacedMuonTriggerPtassignment::fitComparatorsLCT(const CSCComparatorDigiCollection& hCSCComparators, const CSCCorrelatedLCTDigi& stub,
//	                                  CSCDetId ch_id, float& fit_phi_layer1, float& fit_phi_layer3, float& fit_phi_layer6,
//					  float& fit_z_layer1, float& fit_z_layer3, float& fit_z_layer6, float& perp)
void DisplacedMuonTriggerPtassignment::fitComparatorsLCT(const CSCComparatorDigiCollection& hCSCComparators, const CSCCorrelatedLCTDigi& stub,
	                                  CSCDetId ch_id, float* fit_phi_layers, float* fit_z_layers, float& perp)
{

  const auto& cscChamber = cscGeometry_->chamber(ch_id);

  // fetch the CSC comparator digis in this chamber
  CSCComparatorDigiContainerIds compDigisIds;
  for (int iLayer=1; iLayer<=6; ++iLayer){
    CSCDetId layerId(ch_id.endcap(), ch_id.station(), ch_id.ring(), ch_id.chamber(), iLayer);
    // get the digis per layer
    const auto& compRange = hCSCComparators.get(layerId);
    CSCComparatorDigiContainer compDigis;
    for (auto compDigiItr = compRange.first; compDigiItr != compRange.second; compDigiItr++) {
      const auto& compDigi = *compDigiItr;
      //if (stub.getTimeBin() < 4 or stub.getTimeBin() > 8) continue;
      int stubHalfStrip(getHalfStrip(compDigi));
      // these comparator digis never fit the pattern anyway!
      if (std::abs(stubHalfStrip-stub.getStrip())>5) continue;
      // check if this comparator digi fits the pattern
      //if(verbose) std::cout << "Comparator digi L1Mu " << layerId << " " << compDigi << " HS " << stubHalfStrip << " stubKeyHS " << stub.getStrip() << std::endl;
      if (comparatorInLCTPattern(stub.getStrip(), stub.getPattern(), iLayer, stubHalfStrip)) {
        //if(verbose) std::cout<<"\tACCEPT"<<std::endl;
        compDigis.push_back(compDigi);
      }
      // else{
      //   if(verbose) std::cout<<"\tDECLINE!"<<std::endl;
      // }
    }
    // if(verbose) if (compDigis.size() > 2) std::cout << ">>> INFO: " << compDigis.size() << " matched comp digis in this layer!" << std::endl;
    compDigisIds.push_back(std::make_pair(layerId, compDigis));
  }

  // get the z and phi positions
  perp = 0.0;
  std::vector<float> phis;
  std::vector<float> zs;
  std::vector<float> ephis;
  std::vector<float> ezs;
  std::vector<float> status;
  for (const auto& p: compDigisIds){
    const auto& detId = p.first;
    float phi_tmp = 0.0;
    float perp_tmp = 0.0;
    float z_tmp = 0.0;
    if (p.second.size()==0) continue;
    for (const auto& hit: p.second){
      float fractional_strip = getFractionalStrip(hit);
      const auto& layer_geo = cscChamber->layer(detId.layer())->geometry();
      float wire = layer_geo->middleWireOfGroup(stub.getKeyWG() + 1);
      const LocalPoint& csc_intersect = layer_geo->intersectionOfStripAndWire(fractional_strip, wire);
      const GlobalPoint& csc_gp = cscGeometry_->idToDet(detId)->surface().toGlobal(csc_intersect);
      float gpphi = csc_gp.phi();

      if (phis.size()>0 and gpphi>0 and phis[0]<0 and  (gpphi-phis[0])>3.1416)
        phi_tmp += (gpphi-2*3.1415926);
      else if (phis.size()>0 and gpphi<0 and phis[0]>0 and (gpphi-phis[0])<-3.1416)
        phi_tmp += (gpphi+2*3.1415926);
      else
        phi_tmp += (csc_gp.phi());

      z_tmp = csc_gp.z();
      perp_tmp += csc_gp.perp();
    }
    //in case there are more than one comparator digis in one layer
    perp_tmp = perp_tmp/(p.second).size();
    phi_tmp = phi_tmp/(p.second).size();
    if (verbose_>0)
    	std::cout <<"detid "<< detId <<" perp "<< perp_tmp <<" phi "<< phi_tmp <<" z "<< z_tmp << std::endl;
    perp += perp_tmp;
    phis.push_back(phi_tmp);
    zs.push_back(z_tmp);
    ezs.push_back(0);
    // phis.push_back(csc_gp.phi());
    ephis.push_back(gemvalidation::cscHalfStripWidth(detId)/sqrt(12));
  }


  CSCDetId key_id(ch_id.endcap(), ch_id.station(), ch_id.ring(), ch_id.chamber(), CSCConstants::KEY_CLCT_LAYER);
  float fractional_strip = 0.5 * (stub.getStrip() + 1) - 0.25;
  const auto& layer_geo = cscChamber->layer(CSCConstants::KEY_CLCT_LAYER)->geometry();
  // LCT::getKeyWG() also starts from 0
  float wire = layer_geo->middleWireOfGroup(stub.getKeyWG() + 1);
  const LocalPoint& csc_intersect = layer_geo->intersectionOfStripAndWire(fractional_strip, wire);
  const GlobalPoint& csc_gp = cscGeometry_->idToDet(key_id)->surface().toGlobal(csc_intersect);
  perp = csc_gp.perp();
  // use average perp
  //perp = perp/phis.size();
  // do a fit to the comparator digis
  float alpha = -99., beta = 0.;
  PtassignmentHelper::calculateAlphaBeta(zs, phis, ezs, ephis, status, alpha, beta);
  if (phis.size() <= 2 or fabs(alpha)>=99){
      if (verbose_>0)
      	std::cout <<"warning, falied to fit comparator digis,num of digis: "<< phis.size()<<" alpha "<< alpha <<" beta "<< beta << std::endl;
      alpha = csc_gp.phi();
      beta = 0.0;
  }
  if (verbose_>0)
  std::cout <<"fitting results: alpha "<< alpha <<" beta "<< beta << std::endl;
  for (int i=0; i<6; i++){
      fit_z_layers[i] = cscChamber->layer(i+1)->centerOfStrip(20).z();
      fit_phi_layers[i] = PtassignmentHelper::normalizePhi(alpha + beta * fit_z_layers[i]);
      if (verbose_>0)
      	std::cout <<"i "<< i <<" fit_z "<< fit_z_layers[i]<< " fit_phi "<< fit_phi_layers[i]<<" perp "<< perp << std::endl;
  }

}


void DisplacedMuonTriggerPtassignment::fitTrackRadius(GlobalPoint* gps, float* radius)
{

  std::vector<float> gps_r;
  std::vector<float> gps_z;
  std::vector<float> gps_er;
  std::vector<float> gps_ez;
  std::vector<float> status;
  int nstubs = 0;
  for (int i=0; i<4; i++){
  	if (not(hasStub_st[i])) continue;
	nstubs ++;
	gps_r.push_back(gps[i].perp());
	gps_z.push_back(gps[i].z());
	gps_er.push_back(1.5);//how to set error on r?
	gps_ez.push_back(0.);

  }

  if (nstubs < 3){
    if (verbose_>0) std::cout <<"error!!! only "<< nstubs <<" stubs are found, need at least 3 "<< std::endl;
    return ;
  }

  float alpha = 0., beta = 0.;
  PtassignmentHelper::calculateAlphaBeta(gps_z, gps_r, gps_ez, gps_er, status, alpha, beta);
  for (int i=0; i<4; i++){
  	if (hasStub_st[i])
	    radius[i] = alpha + beta*gps[i].z();
  	else
	    radius[i] = 0.0;
	if (fabs(radius[i]-gps[i].perp())>=.02*gps[i].perp() and hasStub_st[i]){
	    if (verbose_>0) std::cout <<" warning!!! difference bewteen before fitting and after fitting is large "<< std::endl;
	//if (verbose_>=0)
	    if (verbose_>0) std::cout <<"station "<< i+1 <<" z "<< gps[i].z() <<" radius from gp "<< gps[i].perp()<<" from fit "<< radius[i]<< std::endl;
	}
  }


}

void DisplacedMuonTriggerPtassignment::globalPositionOfLCT(const CSCCorrelatedLCTDigi stub, CSCDetId chid)
{
  float perp;
  int st = chid.station();
  if (verbose_) std::cout <<"To find globalPositionOfLCT detid "<< chid <<" stub "<< stub << std::endl;
  if (not stub.isValid()) return;

  fitComparatorsLCT(*hCSCComparators.product(), stub, chid, phi_st_layers[st-1], z_st_layers[st-1], perp);
  //gp calculated here can have negative Z!!! later use fabs() to get distance

  gp_st_layer1[st-1] = GlobalPoint(GlobalPoint::Cylindrical(perp, phi_st_layers[st-1][0], z_st_layers[st-1][0]));
  gp_st_layer3[st-1] = GlobalPoint(GlobalPoint::Cylindrical(perp, phi_st_layers[st-1][2], z_st_layers[st-1][2]));
  gp_st_layer6[st-1] = GlobalPoint(GlobalPoint::Cylindrical(perp, phi_st_layers[st-1][5], z_st_layers[st-1][5]));
  if (verbose_>0)
      std::cout <<"LCT position, chid "<< chid<<" hs "<<stub.getStrip()+1<<" wg "<< stub.getKeyWG()+1 <<" gp eta "<< gp_st_layer3[st-1].eta()<<" phi "<<gp_st_layer3[st-1].phi()<<" perp "<< gp_st_layer3[st-1].perp() << std::endl;

}


void DisplacedMuonTriggerPtassignment::globalPositionOfLCT(CSCCorrelatedLCTDigiContainer stubs, CSCDetId chid)
{
  float dR = 99;
  int st = chid.station();
  GlobalPoint gp_ref;
  bool hasRefStub = false;
  unsigned int beststub=0;
  //find closest stub as ref
  for (int i=st-1; i>0; i--)
    if (hasStub_st[i-1]){
      gp_ref = GlobalPoint(gp_st_layer3[i-1]);
      hasRefStub = true;
      break;
  	}
  if (hasRefStub and stubs.size()>1){
    unsigned int istub = -1;
  	for (const auto& stub : stubs){
      istub++;
      if (not stub.isValid()) continue;
  		globalPositionOfLCT(stub, chid);
      //assign higher weight to phi comparison
      float dphi = 10.*deltaPhi(float(gp_st_layer3[st-1].phi()), float(gp_ref.phi()));
      float deta = gp_st_layer3[st-1].eta() - gp_ref.eta();
      float curr_dr2 = dphi*dphi + deta*deta;
      if (curr_dr2<dR){
        dR = curr_dr2;
		    beststub = istub;
      }
  	}
  }
  if (beststub >= stubs.size() or dR >= 99)
    if (verbose_>0) std::cout <<"error beststub >= stubs.size() , beststub "<< beststub <<" stubs size "<< stubs.size() <<" dR "<< dR << std::endl;

  globalPositionOfLCT(stubs[beststub], chid);
  if (verbose_>0)
    std::cout <<"LCT position, chid "<< chid<<" hs "<<stubs[beststub].getStrip()+1<<" wg "<< stubs[beststub].getKeyWG()+1 <<" gp eta "<< gp_st_layer3[st-1].eta()<<" phi "<<gp_st_layer3[st-1].phi()<<" perp "<< gp_st_layer3[st-1].perp() << std::endl;

}


void DisplacedMuonTriggerPtassignment::globalPositionOfGEMPad(const GEMPadDigi gempad, GEMDetId gemid)
{
  GEMDetId ch_id(gemid.region(), gemid.ring(), gemid.station(), gemid.layer(), gemid.chamber(), 0);
  const GEMChamber* gemChamber(gemGeometry_->chamber(ch_id.rawId()));
  const auto& gemRoll(gemChamber->etaPartition(gemid.roll()));//any roll
  /*ignore this since Geometry of GE21 is wrong(should be 2strip-pad)?
  const int nGEMPads(gemRoll->npads());
  if (gempad.pad() > nGEMPads or gempad.pad() < 0){
      std::cout <<" gempad.pad() is within pad range gempad "<< gempad <<" npad "<< nGEMPads << std::endl;
      return;
  }*/

  if (gemid.station() == 1){
  	const LocalPoint& lpGEM(gemRoll->centreOfPad(gempad.pad()));
  	gp_ge[0] = GlobalPoint(gemRoll->toGlobal(lpGEM));
    phi_gem[0] = gp_ge[0].phi();
    if (verbose_>0) std::cout <<" gempad in GE11 id " << gemid <<" gp eta "<< gp_ge[0].eta()<<" phi "<< gp_ge[0].phi()<<" pad "<<gempad.pad()<< std::endl;
  }else if (gemid.station() == 2){
  	const LocalPoint& lpGEM(gemRoll->centreOfStrip(float(gempad.pad()*2.0-1.0)));
  	gp_ge[1] = GlobalPoint(gemRoll->toGlobal(lpGEM));
    phi_gem[1] = gp_ge[1].phi();
    if (verbose_>0) std::cout <<" gempad in GE21 id "<< gemid <<" gp eta "<< gp_ge[1].eta()<<" phi "<< gp_ge[1].phi()<<" pad "<<gempad.pad()<< std::endl;
  }else if (verbose_>0)
    std::cout <<" gemid "<< gemid  <<" not in station 1 or 3" << std::endl;

}


void DisplacedMuonTriggerPtassignment::globalPositionOfGEMPad(GEMPadDigiContainer gempads, GEMDetId gemid)
{
  GEMDetId ch_id(gemid.region(), gemid.ring(), gemid.station(), gemid.layer(), gemid.chamber(), 0);
  const GEMChamber* gemChamber(gemGeometry_->chamber(ch_id.rawId()));
  const auto& gemRoll(gemChamber->etaPartition(gemid.roll()));//any roll
  const int nGEMPads(gemRoll->npads());
  int st = gemid.station();
  for (const auto& gempad : gempads){
  	if (gempad.pad() > nGEMPads or gempad.pad() < 0){
      if (verbose_>0) std::cout <<" gempad.pad() is within pad range gempad "<< gempad <<" npad "<< nGEMPads << std::endl;
      return;
    }
  	const LocalPoint& lpGEM(gemRoll->centreOfPad(gempad.pad()));
  	const GlobalPoint& gp_pad = GlobalPoint(gemRoll->toGlobal(lpGEM));
    float dphi = fabs(deltaPhi(float(gp_pad.phi()), float(gp_st_layer3[st-1].phi())));
    if (hasStub_st[st-1]
        and fabs(dphi) < minGEMCSCdPhi_
        and fabs(gp_pad.eta() - gp_st_layer3[st-1].eta()) < minGEMCSCdEta_
        and fabs(dphi) < fabs(dphi_gemcsc_st[st-1])){
      gp_ge[st-1] = GlobalPoint(gp_pad);
      dphi_gemcsc_st[st-1] = fabs(deltaPhi(float(gp_pad.phi()), float(gp_st_layer3[st-1].phi())));
      if (verbose_>0) std::cout <<"GEMid "<< gemid <<" found matched GEMPad phi "<< phi_gem[st-1] <<" dphi "<< dphi_gemcsc_st[st-1] << std::endl;
    }
  }
}


void DisplacedMuonTriggerPtassignment::matchGEMPadsToTrack(const GEMPadDigiCollection& gemPads, GEMDetId id)
{
    int st = id.station();

    for (auto iC = gemPads.begin(); iC != gemPads.end(); iC++){
	GEMDetId p_id((*iC).first);

	if (p_id.station() != id.station() or p_id.chamber() != id.chamber() or p_id.region() != id.region()) continue;

	const auto& pads(gemPads.get(p_id));
	GEMDetId ch_id(p_id.region(), p_id.ring(), p_id.station(), p_id.layer(), p_id.chamber(), 0);
	const GEMChamber* gemChamber(gemGeometry_->chamber(ch_id.rawId()));
	const auto& gemRoll(gemChamber->etaPartition(p_id.roll()));//any roll

	for (auto d = pads.first; d != pads.second; d++){

	    const LocalPoint lpGEM(gemRoll->centreOfPad(d->pad()));
	    const GlobalPoint& gp_pad = GlobalPoint(gemRoll->toGlobal(lpGEM));
	    float dphi = fabs(deltaPhi(float(gp_pad.phi()), float(gp_st_layer3[st-1].phi())));

	    if (hasStub_st[st-1]
          and fabs(dphi) < minGEMCSCdPhi_
          and fabs(gp_pad.eta() - gp_st_layer3[st-1].eta()) < minGEMCSCdEta_
          and fabs(dphi) < fabs(dphi_gemcsc_st[st-1])){

		    gp_ge[st-1] = gp_pad;
		    dphi_gemcsc_st[st-1] = fabs(deltaPhi(float(gp_pad.phi()), float(gp_st_layer3[st-1].phi())));
		    phi_gem[st-1] = gp_pad.phi();
		    hasGEMPad_st[st-1] = true;

		    if (verbose_>0) std::cout <<"GEMid "<< p_id <<" found matched GEMPad "<< d->pad() <<" phi "<< phi_gem[st-1] <<" dphi "<< dphi_gemcsc_st[st-1] << std::endl;
	    }

	}
    }

}

void DisplacedMuonTriggerPtassignment::matchME0SegmentsToTrack(const ME0SegmentCollection& me0segments)
{
  for(auto iC = me0segments.id_begin(); iC != me0segments.id_end(); ++iC){
    const auto& ch_segs = me0segments.get(*iC);
    for(auto iS = ch_segs.first; iS != ch_segs.second; ++iS){
	const GlobalPoint& gpME0( me0Geometry_->idToDet( *iC )->surface().toGlobal(iS->localPosition()) );
	if (verbose_)
	    std::cout <<"ME0Detid "<< iS->me0DetId()<<" segment "<< *iS << std::endl;
	if (verbose_)
	    std::cout <<"CSC stub gp eta "<< gp_st_layer3[0].eta() <<" phi "<< gp_st_layer3[0].phi() <<" gpME0 eta "<< gpME0.eta()<<" phi "<< gpME0.phi() << std::endl;
	float dPhi = gpME0.phi()- gp_st_layer3[0].phi();
	float dEta = gpME0.eta() - gp_st_layer3[0].eta();
	if (fabs(dPhi) < minGEMCSCdPhi_ and fabs(dEta) < minGEMCSCdEta_ and fabs(dPhi) < fabs(dphi_me0_st1)){
	    //if (verbose_)
		std::cout <<"Found matched ME0, id "<< iS->me0DetId() <<" phi "<< gpME0.phi() <<" eta "<< gpME0.eta() << std::endl;
	    phi_me0 = gpME0.phi();
	    gp_me0 = gpME0;
	    dphi_me0_st1 = dPhi;
	    hasME0 = true;
	}
    }
  }
}


float DisplacedMuonTriggerPtassignment::deltaYcalculation(GlobalPoint gp1, GlobalPoint gp2) const
{
   float anglea = gp2.phi();
   float newyst1 = -gp1.x()*sin(anglea) + gp1.y()*cos(anglea);
   float newyst2 = -gp2.x()*sin(anglea) + gp2.y()*cos(anglea);
   return (newyst2-newyst1);

}


float DisplacedMuonTriggerPtassignment::deltadeltaYcalculation(GlobalPoint gp1, GlobalPoint gp2, GlobalPoint gp3, float eta, int par) const
{

   float anglea = gp2.phi();
   float newyst1 = -gp1.x()*sin(anglea) + gp1.y()*cos(anglea);
   float newyst2 = -gp2.x()*sin(anglea) + gp2.y()*cos(anglea);
	//float newxst3 = gp3.x()*cos(anglea) + gp3.y()*sin(anglea);
   float newyst3 = -gp3.x()*sin(anglea) + gp3.y()*cos(anglea);
   float deltay12 = newyst2-newyst1;
   float deltay23 = newyst3-newyst2;
   //std::cout <<" angle in st2 "<< anglea <<" newyst1 "<< newyst1 <<" newyst2 "<< newyst2 << " newyst3 "<< newyst3 << std::endl;
   int neta = PtassignmentHelper::GetEtaPartition(eta);

   if (meRing_st1 == 1 and neta == 1) neta =2;
   if (meRing_st1 == 2 and neta == 2) neta =1;

   if (par<0 or par>3 or neta==-1) return -99;
   return (deltay23-PtassignmentHelper::PositionEpLUT[par][neta][0]*deltay12);

}

float DisplacedMuonTriggerPtassignment::phiMomentum_Xfactor(float phi_CSC, float phi_GEM, float xfactor) const
{


   if (fabs(phi_CSC) > M_PI or fabs(phi_GEM) > M_PI) {
       std::cout <<"warning phi is not vaild , phi_CSC "<< phi_CSC <<" phi_GEM "<< phi_GEM << std::endl;
       return -9;
   }

   float dphi = deltaPhi(phi_CSC,phi_GEM);
   float y = 1.0-cos(dphi)- xfactor;

   float phi_diff = 0.0;
   if (fabs(y) > 0.0) phi_diff = atan(sin(dphi)/y);
   else phi_diff = M_PI/2.0;

   if (phi_diff <= -M_PI) phi_diff = phi_diff+2*M_PI;
   else if (phi_diff > M_PI) phi_diff = phi_diff-2*M_PI;

   float phiM = phi_GEM-phi_diff;
   if (phiM <= -M_PI) phiM = phiM+2*M_PI;
   else if (phiM > M_PI) phiM = phiM-2*M_PI;

   //std::cout <<"PhiMomentum_Xfactor: dphi "<< dphi <<" phi_poistion1 "<< phi_GEM <<" phi_position2 "<< phi_CSC <<" Xfactor "<<X <<" phi_diff "<< phi_diff <<" phiM "<< phiM << std::endl;

   return phiM;
}


bool DisplacedMuonTriggerPtassignment::runPositionbased()
{
   if (npar<0 or npar>=4 or not(hasStub_st[2])){
        std::cout <<" failed to runPositionbased  npar "<< npar << std::endl;
   	return false;
   }
   ddY123 = deltadeltaYcalculation(gp_st_layer3[0], gp_st_layer3[1], gp_st_layer3[2], gp_st_layer3[1].eta(), npar);
   deltaY12 = deltaYcalculation(gp_st_layer3[0], gp_st_layer3[1]);
   deltaY23 = -deltaYcalculation(gp_st_layer3[2], gp_st_layer3[1]);
   if (npar>=0 and npar<=3){
        position_pt = 2.0;
   	int neta = PtassignmentHelper::GetEtaPartition(eta_st2);
   	for (int i=0; i<PtassignmentHelper::NPt2; i++){
		if (fabs(ddY123) <= PtassignmentHelper::PositionbasedDDYLUT[i][neta][npar])
		    position_pt = float(PtassignmentHelper::PtBins2[i]);
		else
		    break;
		if (verbose_>0)
		    std::cout <<"eta "<< eta_st2 <<" neta "<< neta <<" npar "<< npar <<" fabs ddY123 "<< fabs(ddY123) <<" cut "<< PtassignmentHelper::PositionbasedDDYLUT[i][neta][npar] <<" position pt "<< position_pt<<std::endl;
	}
   }
   return true;
}

// run the direction based algorithm. Option to include GE21 hits or not
bool DisplacedMuonTriggerPtassignment::runDirectionbased(bool useME0GE11, bool useGE21)
{
  if (useGE21 and meRing==1) return runDirectionbasedGE21(useME0GE11);
  else return runDirectionbasedCSConly(useME0GE11);
}

//use GE21 if GE21 pads are available. use GE11 if GE11 pads are available
bool DisplacedMuonTriggerPtassignment::runDirectionbasedGE21(bool useME0GE11)
{
   //if (not (npar<4 and npar>=0 and hasGEMPad_st1 and hasGEMPad_st2)) return false;
   if (not (npar<4 and npar>=0)) return false;
   //if (fabs(phi_gem[1])>4) return false;//check this because we want to use setPhiGE21() to set phi_gem[1](using 2strips-pad)

   float xfactor_st1 = 0.0;
   float xfactor_st2 = 0.0;

   //phiM_st1
   if (meRing==1 and fabs(gp_st_layer3[0].eta()) <= me0MinEta_ and useME0GE11){

        if (not hasGEMPad_st[0]) return false;
	xfactor_st1 = xfactor*fabs(gp_ge[0].z() - gp_st_layer3[0].z());
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer3[0].phi(), gp_ge[0].phi(), xfactor_st1);//

   //}else if (meRing == 1 and fabs(gp_st_layer3[0].eta()) >= me0MinEta_-0.05 and useME0GE11){
   }else if (meRing == 1 and fabs(gp_st_layer3[0].eta()) >= me0MinEta_ and useME0GE11){

        if (not hasME0) return false;
	xfactor_st1 = xfactor*fabs(gp_me0.z() - gp_st_layer3[0].z());
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer3[0].phi(), gp_me0.phi(), xfactor_st1);//

   //}else if (meRing == 1 and fabs(gp_st_layer3[0].eta()) >= me0MinEta_ and not(useME0)){
   }else if (meRing == 1 and not(useME0GE11)){ //

        xfactor_st1 = xfactor*fabs(z_st_layers[0][0] - z_st_layers[0][5])/(xfactor*fabs(gp_st_layer3[0].z() - z_st_layers[0][5])+1);
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer6[0].phi(), gp_st_layer1[0].phi(), xfactor_st1);//

   }else if (meRing == 2){

        xfactor_st1 = xfactor*fabs(z_st_layers[0][0] - z_st_layers[0][5])/(xfactor*fabs(gp_st_layer3[0].z() - z_st_layers[0][5])+1);
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer6[0].phi(), gp_st_layer1[0].phi(), xfactor_st1);//

   }else
       return false;

   //phiM_st2
   if (meRing==1 and hasGEMPad_st[1]){

	xfactor_st2 = xfactor*fabs(gp_ge[1].z() - gp_st_layer3[1].z())/(xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[1].z())+1);
   	phiM_st2 = phiMomentum_Xfactor(gp_st_layer3[1].phi(), phi_gem[1], xfactor_st2);

   }else if (meRing == 2){

   	xfactor_st2 = xfactor*fabs(z_st_layers[1][0] - z_st_layers[1][5])/(xfactor*fabs(gp_st_layer3[0].z() - z_st_layers[1][5])+1);
   	phiM_st2 = phiMomentum_Xfactor(gp_st_layer6[1].phi(), gp_st_layer1[1].phi(), xfactor_st2);//

   }else
       return false;

   float xfactor_st12 = xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[1].z())/(xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[1].z())+1);
   float xfactor_st23 = xfactor*fabs(gp_st_layer3[1].z() - gp_st_layer3[2].z())/(xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[2].z())+1);
   phiM_st12 = phiMomentum_Xfactor(gp_st_layer3[1].phi(), gp_st_layer3[0].phi(), xfactor_st12);
   phiM_st23 = phiMomentum_Xfactor(gp_st_layer3[2].phi(), gp_st_layer3[1].phi(), xfactor_st23);
   if (verbose_>0)  std::cout <<"DisplacedMuonTrigger, direction with GE21, meRing "<< meRing <<" xfactor_st1 "<< xfactor_st1 <<" phiM_st1 "<< phiM_st1
       			<<" xfactor_st2 "<< xfactor_st2 <<" phiM_st2 "<< phiM_st2 << std::endl;

   //make sure both phiM_st1 and phiM_st2 are reasonable, 4 can be changed into M_PI later
   dPhi_dir_st1_st2 = (fabs(phiM_st1)<4 and fabs(phiM_st2)<4)? deltaPhi(phiM_st1, phiM_st2):-9;
   dPhi_dir_st1_st12 = (fabs(phiM_st1)<4 and fabs(phiM_st12)<4)? deltaPhi(phiM_st1, phiM_st12):-9;
   dPhi_dir_st2_st23 = (fabs(phiM_st2)<4 and fabs(phiM_st23)<4)? deltaPhi(phiM_st2, phiM_st23):-9;
   dPhi_dir_st12_st23 = (fabs(phiM_st12)<4 and fabs(phiM_st23)<4)? deltaPhi(phiM_st12, phiM_st23):-9;

   if (fabs(dPhi_dir_st1_st2) > 4){
   	std::cout <<"error in runDirectionbased withGE21, dPhi_dir_st1_st2 "<< dPhi_dir_st1_st2 <<" phiM_st1 "<< phiM_st1 <<" phiM_st2 "<< phiM_st2 <<std::endl;
	return false;
   }

   /*if (npar>=0 and npar<=3){
        direction_pt = 2.0;
   	int neta = PtassignmentHelper::GetEtaPartition(eta_st2);
   	for (int i=0; i<PtassignmentHelper::NPt2; i++){
		if (fabs(dPhi_dir_st1_st2) <= PtassignmentHelper::DirectionbasedDeltaPhiLUT[i][neta][npar])
		    direction_pt = float(PtassignmentHelper::PtBins2[i]);
		else
		    break;
		if (verbose_>0)
		    std::cout <<"eta "<< eta_st2 <<" neta "<< neta <<" npar "<< npar <<" fabs dphi "<< fabs(dPhi_dir_st1_st2) <<" cut "<< PtassignmentHelper::DirectionbasedDeltaPhiLUT[i][neta][npar] <<" direction pt "<< direction_pt<<std::endl;
	}
   }*/
   return true;
}


bool DisplacedMuonTriggerPtassignment::runDirectionbasedCSConly(bool useME0GE11)
{

   //z_st_layers should be used at sim level, set Z and phi for layer1 and layer6 at sim level, or rebuild constructor?
   if (not ( npar<4 and npar>=0)) return false;
   float xfactor_st1 = 0.0;
   float xfactor_st2 = 0.0;

   //phiM_st1
   if (meRing==1 and fabs(gp_st_layer3[0].eta()) <= me0MinEta_ and useME0GE11){

        if (not hasGEMPad_st[0]) return false;
	xfactor_st1 = xfactor*fabs(gp_ge[0].z() - gp_st_layer3[0].z());
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer3[0].phi(), gp_ge[0].phi(), xfactor_st1);//

   }else if (meRing == 1 and fabs(gp_st_layer3[0].eta()) >= me0MinEta_ and useME0GE11){

        if (not hasME0) return false;
	xfactor_st1 = xfactor*fabs(gp_me0.z() - gp_st_layer3[0].z());
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer3[0].phi(), gp_me0.phi(), xfactor_st1);//

   //}else if (meRing == 1 and fabs(gp_st_layer3[0].eta()) >= me0MinEta_ and not(useME0)){
   }else if (meRing == 1 and not(useME0GE11)){

        xfactor_st1 = xfactor*fabs(z_st_layers[0][0] - z_st_layers[0][5])/(xfactor*fabs(gp_st_layer3[0].z() - z_st_layers[0][5])+1);
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer6[0].phi(), gp_st_layer1[0].phi(), xfactor_st1);//

   }else if (meRing == 2){

        xfactor_st1 = xfactor*fabs(z_st_layers[0][0] - z_st_layers[0][5])/(xfactor*fabs(gp_st_layer3[0].z() - z_st_layers[0][5])+1);
   	phiM_st1 = phiMomentum_Xfactor(gp_st_layer6[0].phi(), gp_st_layer1[0].phi(), xfactor_st1);//

   }else
       return false;



   xfactor_st2 = xfactor*fabs(z_st_layers[1][0] - z_st_layers[1][5])/(xfactor*fabs(gp_st_layer3[0].z() - z_st_layers[1][5])+1);
   float xfactor_st12 = xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[1].z())/(xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[1].z())+1);
   float xfactor_st23 = xfactor*fabs(gp_st_layer3[1].z() - gp_st_layer3[2].z())/(xfactor*fabs(gp_st_layer3[0].z() - gp_st_layer3[2].z())+1);
   phiM_st2 = phiMomentum_Xfactor(phi_st_layers[1][5], phi_st_layers[1][0], xfactor_st2);
   phiM_st12 = phiMomentum_Xfactor(gp_st_layer3[1].phi(), gp_st_layer3[0].phi(), xfactor_st12);
   phiM_st23 = phiMomentum_Xfactor(gp_st_layer3[2].phi(), gp_st_layer3[1].phi(), xfactor_st23);

   //if phi in layer1 and layer6 in station1 and 2 are not set, then here phiM return -9
   if (verbose_>0)  std::cout <<"DisplacedMuonTrigger CSConly direction, meRing "<< meRing <<" xfactor_st1 "<< xfactor_st1 <<" phiM_st1 "<< phiM_st1
       			<<" xfactor_st2 "<< xfactor_st2 <<" phiM_st2 "<< phiM_st2 << std::endl;

   dPhi_dir_st1_st2 = (fabs(phiM_st1)<4 and fabs(phiM_st2)<4)? deltaPhi(phiM_st1, phiM_st2):-9;
   dPhi_dir_st1_st12 = (fabs(phiM_st1)<4 and fabs(phiM_st12)<4)? deltaPhi(phiM_st1, phiM_st12):-9;
   dPhi_dir_st2_st23 = (fabs(phiM_st2)<4 and fabs(phiM_st23)<4)? deltaPhi(phiM_st2, phiM_st23):-9;
   dPhi_dir_st12_st23 = (fabs(phiM_st12)<4 and fabs(phiM_st23)<4)? deltaPhi(phiM_st12, phiM_st23):-9;

   if (fabs(dPhi_dir_st1_st2) > 4){
   	std::cout <<"error in runDirectionbased withoutGE21, dPhi_dir_st1_st2 "<< dPhi_dir_st1_st2 <<" phiM_st1 "<< phiM_st1 <<" phiM_st2 "<< phiM_st2 <<std::endl;
	return false;
   }

   /*if (npar>=0 and npar<=3){
        direction_pt = 2.0;
   	int neta = PtassignmentHelper::GetEtaPartition(eta_st2);
   	for (int i=0; i<PtassignmentHelper::NPt2; i++){
		if (fabs(dPhi_dir_st1_st2) <= PtassignmentHelper::DirectionbasedDeltaPhiME21CSConlyLUT[i][neta][npar])
		    direction_pt = float(PtassignmentHelper::PtBins2[i]);
		else
		    break;
		if (verbose_>0)
		    std::cout <<"eta "<< eta_st2 <<" neta "<< neta <<" npar "<< npar <<" fabs dphi "<< fabs(dPhi_dir_st1_st2) <<" cut "<< PtassignmentHelper::DirectionbasedDeltaPhiME21CSConlyLUT[i][neta][npar] <<" direction pt "<< direction_pt<<std::endl;
	}

   }*/
   return true;
}

//not use now
void DisplacedMuonTriggerPtassignment::runHybrid(bool useME0GE11, bool useGE21)
{

   //firstly to run through position-based and direction-based
   hybrid_pt = 2.0;
   runPositionbased();
   runDirectionbased(useME0GE11, useGE21);

   if (npar>=0 and npar<=3){
	if (fabs(ddY123)>=40 or fabs(dPhi_dir_st1_st2)>=1.0){
	   hybrid_pt = 2.0;
	   return;
	}

   	int neta = PtassignmentHelper::GetEtaPartition(eta_st2);
	//ignore pt=40
   	for (int i=0; i<PtassignmentHelper::NPt-1; i++){
           if(useGE21 and PtassignmentHelper::ellipse(PtassignmentHelper::HybridDDYAndDeltaPhiLUT[i][neta][npar][0],
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUT[i][neta][npar][1],
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUT[i][neta][npar][2],
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUT[i][neta][npar][3],
					 // not using charge info for now, may update it later
		  			  //PtassignmentHelper::HybridDDYAndDeltaPhiLUT[i][neta][npar][4], ddY123*charge, dPhi_dir_st1_st2*charge) <=1)
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUT[i][neta][npar][4], ddY123, dPhi_dir_st1_st2) <=1)
		hybrid_pt = PtassignmentHelper::PtBins[i];
	   else if(not(useGE21) and PtassignmentHelper::ellipse(PtassignmentHelper::HybridDDYAndDeltaPhiLUTME21CSConly[i][neta][npar][0],
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUTME21CSConly[i][neta][npar][1],
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUTME21CSConly[i][neta][npar][2],
		  			  PtassignmentHelper::HybridDDYAndDeltaPhiLUTME21CSConly[i][neta][npar][3],
		  		//  PtassignmentHelper::HybridDDYAndDeltaPhiLUTME21CSConly[i][neta][npar][4], ddY123*charge, dPhi_dir_st1_st2*charge) <=1)
		  			PtassignmentHelper::HybridDDYAndDeltaPhiLUTME21CSConly[i][neta][npar][4], ddY123, dPhi_dir_st1_st2) <=1)
		hybrid_pt = PtassignmentHelper::PtBins[i];
	   else//make sure LUT is consitent
	   	break;
	   if (verbose_>0)
   		std::cout <<"eta_st2 "<< eta_st2 <<" npar "<< npar <<" charge "<< charge <<" ddY123 "<< ddY123 << " dphi_dir "<< dPhi_dir_st1_st2 <<" hybrid_pt "<< hybrid_pt << std::endl;

	}
   }
}

float DisplacedMuonTriggerPtassignment::getlocalPhiDirection(int st) const
{
    //st =1 :station1 , st=2: station2
    //st = 12 : between station1 and station2; st = 23 : between station2 and station3
   if (st==1 and hasStub_st[0]) return phiM_st1;
   else if (st==2 and hasStub_st[1]) return phiM_st2;
   else if (st == 12 and hasStub_st[0] and hasStub_st[1]) return phiM_st12;
   else if (st == 23 and hasStub_st[1] and hasStub_st[2]) return phiM_st23;
   else{
   	std::cout <<" error in getlocalPhiDirection, st  "<<st <<" not in range or not not have stub or GEMpad" << std::endl;
	return -99;
   }
}

float DisplacedMuonTriggerPtassignment::getdeltaPhiDirection(int st1, int st2) const
{
   if (((st1 == 1 and st2 == 2) or (st1 == 1 and st2 == 2)) and hasStub_st[0] and hasStub_st[1]) return dPhi_dir_st1_st2;
   else if (((st1 == 1 and st2 == 12) or (st1 == 12 and st2 == 1)) and hasStub_st[0] and hasStub_st[1]) return dPhi_dir_st1_st12;
   else if (((st1 == 2 and st2 == 23) or (st1 == 23 and st2 == 2)) and hasStub_st[1] and hasStub_st[2]) return dPhi_dir_st2_st23;
   else if (((st1 == 12 and st2 == 23) or (st1 == 23 and st2 == 12)) and hasStub_st[0] and hasStub_st[1] and hasStub_st[2]) return dPhi_dir_st12_st23;
   else{
   	std::cout <<" error in getdeltaPhiDirection, st1 "<< st1 <<" st2 "<< st2 <<" not in range or not not have stub or GEMpad" << std::endl;
	return -99;
   }

}

int
DisplacedMuonTriggerPtassignment::getHalfStrip(const CSCComparatorDigi& digi)
{
  return (digi.getStrip() - 1) * 2 + digi.getComparator();
}

float
DisplacedMuonTriggerPtassignment::getFractionalStrip(const CSCComparatorDigi&d)
{
  return d.getStrip() + d.getComparator()/2. - 3/4.;
}

bool
DisplacedMuonTriggerPtassignment::stubInDTTFTracks(const L1MuDTTrackSegPhi& candidateStub,
                                                   const L1MuDTTrackCollection& l1Tracks) const
{
  bool isMatched = false;
  for (const auto& tftrack: l1Tracks){
    const auto& stubCollection = tftrack.second;
    for (const auto& stub: stubCollection) {
      if (candidateStub == stub) {
        isMatched = true;
        break;
      }
    }
  }
  return isMatched;
}

bool
DisplacedMuonTriggerPtassignment::stubInCSCTFTracks(const CSCCorrelatedLCTDigi& candidateStub,
                                                    const l1t::EMTFTrackCollection& l1Tracks) const
{
  bool isMatched = false;
  for (const auto& tftrack: l1Tracks){
    for (const l1t::EMTFHit& hit : tftrack.Hits()){
      if (hit.Is_CSC()) continue;
      const CSCCorrelatedLCTDigi& csc_hit = hit.CSC_LCTDigi();
      if (csc_hit == candidateStub) {
        isMatched = true;
        break;
      }
    }
  }
  return isMatched;
}

double
phiL1DTTrack(const L1MuDTTrack& track)
{
  int phi_local = track.phi_packed(); //range: 0 < phi_local < 31
  if ( phi_local > 15 ) phi_local -= 32; //range: -16 < phi_local < 15
  double dttrk_phi_global = normalizedPhi((phi_local*(M_PI/72.))+((M_PI/6.)*track.spid().sector()));// + 12*i->scNum(); //range: -16 < phi_global < 147
  // if(dttrk_phi_global < 0) dttrk_phi_global+=2*M_PI; //range: 0 < phi_global < 147
  // if(dttrk_phi_global > 2*M_PI) dttrk_phi_global-=2*M_PI; //range: 0 < phi_global < 143
  return dttrk_phi_global;
}

#endif
