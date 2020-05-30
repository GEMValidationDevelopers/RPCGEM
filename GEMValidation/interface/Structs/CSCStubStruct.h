#ifndef GEMCode_GEMValidation_CSCStubStruct
#define GEMCode_GEMValidation_CSCStubStruct

#include "TTree.h"
#include <string>

namespace gem {

  struct CSCStubStruct {

    Int_t chamber_lct_odd;
    Int_t chamber_lct_even;

    bool has_clct_odd;
    bool has_alct_odd;
    bool has_lct_odd;

    bool has_clct_even;
    bool has_alct_even;
    bool has_lct_even;

    Float_t phi_layer1_fit_even, phi_layer3_fit_even;
    float phi_layer6_fit_even, phi_layer1_fit_odd;
    float phi_layer3_fit_odd, phi_layer6_fit_odd;
    Float_t z_layer3_fit_even, z_layer3_fit_odd;

    bool bend_lct_odd;
    bool bend_lct_even;

    bool bx_lct_odd, bx_alct_odd, bx_clct_odd;
    bool bx_lct_even, bx_alct_even, bx_clct_even;

    bool hs_lct_odd;
    bool wg_lct_odd;
    bool hs_lct_even;
    bool wg_lct_even;

    Float_t phi_lct_odd;
    Float_t phi_lct_even;
    Float_t eta_lct_odd;
    Float_t eta_lct_even;
    Float_t dphi_lct_odd;  // dphi stored as data member in LCT
    Float_t dphi_lct_even;
    Float_t chi2_lct_odd, chi2_lct_even;

    Float_t timeErr_lct_odd, timeErr_lct_even;
    Int_t nHits_lct_odd, nHits_lct_even;
    Float_t dR_sim_lct_odd, dR_sim_lct_even;

    Float_t perp_lct_odd;
    Float_t perp_lct_even;

    Bool_t passdphi_odd;
    Bool_t passdphi_even;

    Float_t fitperp_lct_odd;
    Float_t fitperp_lct_even;

    Int_t quality_clct_odd;
    Int_t quality_clct_even;
    Int_t quality_alct_odd;
    Int_t quality_alct_even;

    Int_t lct_type;
    Int_t quality_odd;
    Int_t quality_even;

    void init() {
      quality_odd = 0;
      quality_even = 0;

      phi_layer1_fit_even = -9.0;
      phi_layer1_fit_odd = -9.0;

      z_layer3_fit_even = -9.0;
      z_layer3_fit_odd = -9.0;

      phi_layer3_fit_even = -9.0;
      phi_layer3_fit_odd = -9.0;
      phi_layer6_fit_even = -9.0;
      phi_layer6_fit_odd = -9.0;

      has_alct_even = 0;
      has_clct_even = 0;
      has_lct_even = 0;
      has_alct_odd = 0;
      has_clct_odd = 0;
      has_lct_odd = 0;

      chamber_lct_odd = -1;
      chamber_lct_even = -1;

      bend_lct_odd = -9;
      bend_lct_even = -9;
      dphi_lct_odd = -9;
      dphi_lct_even = -9;

      bx_lct_odd = -9;
      bx_lct_even = -9;
      hs_lct_odd = 0;
      hs_lct_even = 0;
      wg_lct_odd = 0;
      wg_lct_even = 0;
      phi_lct_odd = -9.;
      phi_lct_even = -9.;
      eta_lct_odd = -9.;
      eta_lct_even = -9.;

      chi2_lct_odd = -99999;
      chi2_lct_even = -99999;

      timeErr_lct_odd = -9999;
      timeErr_lct_even = -9999;

      nHits_lct_odd = 0;
      nHits_lct_even = 0;
      dR_sim_lct_odd = 10;
      dR_sim_lct_even = 10;
      passdphi_odd = false;
      passdphi_even = false;
      perp_lct_odd = -1;
      perp_lct_even = -1;
      fitperp_lct_odd = -1;
      fitperp_lct_even = -1;

      quality_clct_odd = -1;
      quality_clct_even = -1;
      quality_alct_odd = -1;
      quality_alct_even = -1;

      bx_clct_odd = -9;
      bx_clct_even = -9;
      bx_alct_odd = -9;
      bx_alct_even = -9;

      lct_type = -1;
    };

    TTree* book(TTree* t) {

      t->Branch("quality_odd", &quality_odd);
      t->Branch("quality_even", &quality_even);

      t->Branch("has_clct_odd", &has_clct_odd);
      t->Branch("has_alct_odd", &has_alct_odd);
      t->Branch("has_lct_odd", &has_lct_odd);

      t->Branch("has_clct_even", &has_clct_even);
      t->Branch("has_alct_even", &has_alct_even);
      t->Branch("has_lct_even", &has_lct_even);

      t->Branch("quality_clct_odd", &quality_clct_odd);
      t->Branch("quality_clct_even", &quality_clct_even);

      t->Branch("bx_clct_odd", &bx_clct_odd);
      t->Branch("bx_clct_even", &bx_clct_even);

      t->Branch("quality_alct_odd", &quality_alct_odd);
      t->Branch("quality_alct_even", &quality_alct_even);

      t->Branch("bx_alct_odd", &bx_alct_odd);
      t->Branch("bx_alct_even", &bx_alct_even);

      t->Branch("chamber_lct_odd", &chamber_lct_odd);
      t->Branch("chamber_lct_even", &chamber_lct_even);

      t->Branch("z_layer3_fit_even", &z_layer3_fit_even);
      t->Branch("z_layer3_fit_odd", &z_layer3_fit_odd);

      t->Branch("phi_layer1_fit_even", &phi_layer1_fit_even);
      t->Branch("phi_layer1_fit_odd", &phi_layer1_fit_odd);
      t->Branch("phi_layer3_fit_even", &phi_layer3_fit_even);
      t->Branch("phi_layer3_fit_odd", &phi_layer3_fit_odd);
      t->Branch("phi_layer6_fit_even", &phi_layer6_fit_even);
      t->Branch("phi_layer6_fit_odd", &phi_layer6_fit_odd);

      t->Branch("bend_lct_odd", &bend_lct_odd);
      t->Branch("bend_lct_even", &bend_lct_even);

      t->Branch("bx_lct_odd", &bx_lct_odd);
      t->Branch("bx_lct_even", &bx_lct_even);
      t->Branch("hs_lct_odd", &hs_lct_odd);
      t->Branch("hs_lct_even", &hs_lct_even);
      t->Branch("wg_lct_even", &wg_lct_even);
      t->Branch("wg_lct_odd", &wg_lct_odd);
      t->Branch("phi_lct_odd", &phi_lct_odd);
      t->Branch("phi_lct_even", &phi_lct_even);
      t->Branch("eta_lct_odd", &eta_lct_odd);
      t->Branch("eta_lct_even", &eta_lct_even);
      t->Branch("perp_lct_odd", &perp_lct_odd);
      t->Branch("perp_lct_even", &perp_lct_even);
      t->Branch("fitperp_lct_odd", &fitperp_lct_odd);
      t->Branch("fitperp_lct_even", &fitperp_lct_even);
      t->Branch("dphi_lct_odd", &dphi_lct_odd);
      t->Branch("dphi_lct_even", &dphi_lct_even);
      t->Branch("chi2_lct_odd", &chi2_lct_odd);
      t->Branch("chi2_lct_even", &chi2_lct_even);
      t->Branch("timeErr_lct_odd", &timeErr_lct_odd);
      t->Branch("timeErr_lct_even", &timeErr_lct_even);
      t->Branch("nHits_lct_odd", &nHits_lct_odd);
      t->Branch("nHits_lct_even", &nHits_lct_even);
      t->Branch("dR_sim_lct_odd", &dR_sim_lct_odd);
      t->Branch("dR_sim_lct_even", &dR_sim_lct_even);
      t->Branch("passdphi_odd", &passdphi_odd);
      t->Branch("passdphi_even", &passdphi_even);
      t->Branch("lct_type", &lct_type);

      return t;
    }
  };
}  // namespace

#endif
