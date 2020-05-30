#ifndef GEMCode_GEMValidation_CSCStubStruct
#define GEMCode_GEMValidation_CSCStubStruct

#include "TTree.h"

namespace gem {

  struct CSCStubStruct {

    static const int nStations = 11;

    // bools
    bool has_clct_odd[nStations];
    bool has_alct_odd[nStations];
    bool has_lct_odd[nStations];

    bool has_clct_even[nStations];
    bool has_alct_even[nStations];
    bool has_lct_even[nStations];

    // ints
    int bx_lct_odd[nStations];
    int bx_alct_odd[nStations];
    int bx_clct_odd[nStations];
    int bx_lct_even[nStations];
    int bx_alct_even[nStations];
    int bx_clct_even[nStations];

    int hs_lct_odd[nStations];
    int wg_lct_odd[nStations];
    int hs_lct_even[nStations];
    int wg_lct_even[nStations];

    int chamber_lct_odd[nStations];
    int chamber_lct_even[nStations];
    int bend_lct_odd[nStations];
    int bend_lct_even[nStations];

    int passdphi_odd[nStations];
    int passdphi_even[nStations];

    int quality_clct_odd[nStations];
    int quality_clct_even[nStations];
    int quality_alct_odd[nStations];
    int quality_alct_even[nStations];

    int lct_type[nStations];

    int quality_odd[nStations];
    int quality_even[nStations];

    int nHits_lct_odd[nStations];
    int nHits_lct_even[nStations];

    // floats
    float phi_lct_odd[nStations];
    float phi_lct_even[nStations];
    float eta_lct_odd[nStations];
    float eta_lct_even[nStations];
    float dphi_lct_odd[nStations];
    float dphi_lct_even[nStations];
    float chi2_lct_odd[nStations];
    float chi2_lct_even[nStations];

    float timeErr_lct_odd[nStations];
    float timeErr_lct_even[nStations];

    float perp_lct_odd[nStations];
    float perp_lct_even[nStations];

    void init() {
      for (unsigned i = 0 ; i < nStations; i++) {

        quality_odd[i] = 0;
        quality_even[i] = 0;

        has_alct_even[i] = 0;
        has_clct_even[i] = 0;
        has_lct_even[i] = 0;
        has_alct_odd[i] = 0;
        has_clct_odd[i] = 0;
        has_lct_odd[i] = 0;

        chamber_lct_odd[i] = -1;
        chamber_lct_even[i] = -1;

        bend_lct_odd[i] = -9;
        bend_lct_even[i] = -9;
        dphi_lct_odd[i] = -9;
        dphi_lct_even[i] = -9;

        bx_lct_odd[i] = -9;
        bx_lct_even[i] = -9;
        hs_lct_odd[i] = 0;
        hs_lct_even[i] = 0;
        wg_lct_odd[i] = 0;
        wg_lct_even[i] = 0;
        phi_lct_odd[i] = -9.;
        phi_lct_even[i] = -9.;
        eta_lct_odd[i] = -9.;
        eta_lct_even[i] = -9.;

        chi2_lct_odd[i] = -99999;
        chi2_lct_even[i] = -99999;

        timeErr_lct_odd[i] = -9999;
        timeErr_lct_even[i] = -9999;

        nHits_lct_odd[i] = 0;
        nHits_lct_even[i] = 0;

        passdphi_odd[i] = 0;
        passdphi_even[i] = 0;

        perp_lct_odd[i] = -1;
        perp_lct_even[i] = -1;

        quality_clct_odd[i] = -1;
        quality_clct_even[i] = -1;
        quality_alct_odd[i] = -1;
        quality_alct_even[i] = -1;

        bx_clct_odd[i] = -9;
        bx_clct_even[i] = -9;
        bx_alct_odd[i] = -9;
        bx_alct_even[i] = -9;

        lct_type[i] = -1;
      }
    };

    void book(TTree* t) {

      t->Branch("has_clct_odd", has_clct_odd, "has_clct_odd[nStations]/O");
      t->Branch("has_alct_odd", has_alct_odd, "has_alct_odd[nStations]/O");
      t->Branch("has_lct_odd", has_lct_odd, "has_lct_odd[nStations]/O");

      t->Branch("has_clct_even", has_clct_even, "has_clct_even[nStations]/O");
      t->Branch("has_alct_even", has_alct_even, "has_alct_even[nStations]/O");
      t->Branch("has_lct_even", has_lct_even, "has_lct_even[nStations]/O");

      t->Branch("quality_odd", quality_odd, "quality_odd[nStations]/I");
      t->Branch("quality_even", quality_even, "quality_even[nStations]/I");

      t->Branch("quality_clct_odd", quality_clct_odd, "quality_clct_odd[nStations]/I");
      t->Branch("quality_clct_even", quality_clct_even, "quality_clct_even[nStations]/I");

      t->Branch("bx_clct_odd", bx_clct_odd, "bx_clct_odd[nStations]/I");
      t->Branch("bx_clct_even", bx_clct_even, "bx_clct_even[nStations]/I");

      t->Branch("quality_alct_odd", quality_alct_odd, "quality_alct_odd[nStations]/I");
      t->Branch("quality_alct_even", quality_alct_even, "quality_alct_even[nStations]/I");

      t->Branch("bx_alct_odd", bx_alct_odd, "bx_alct_odd[nStations]/I");
      t->Branch("bx_alct_even", bx_alct_even, "bx_alct_even[nStations]/I");

      t->Branch("chamber_lct_odd", chamber_lct_odd, "chamber_lct_odd[nStations]/I");
      t->Branch("chamber_lct_even", chamber_lct_even, "chamber_lct_even[nStations]/I");

      t->Branch("bend_lct_odd", bend_lct_odd, "bend_lct_odd[nStations]/I");
      t->Branch("bend_lct_even", bend_lct_even, "bend_lct_even[nStations]/I");

      t->Branch("bx_lct_odd", bx_lct_odd, "bx_lct_odd[nStations]/I");
      t->Branch("bx_lct_even", bx_lct_even, "bx_lct_even[nStations]/I");

      t->Branch("hs_lct_odd", hs_lct_odd, "hs_lct_odd[nStations]/I");
      t->Branch("hs_lct_even", hs_lct_even, "hs_lct_even[nStations]/I");

      t->Branch("wg_lct_odd", wg_lct_odd, "wg_lct_odd[nStations]/I");
      t->Branch("wg_lct_even", wg_lct_even, "wg_lct_even[nStations]/I");

      t->Branch("phi_lct_odd", phi_lct_odd, "phi_lct_odd[nStations]/F");
      t->Branch("phi_lct_even", phi_lct_even, "phi_lct_even[nStations]/F");

      t->Branch("eta_lct_odd", eta_lct_odd, "eta_lct_odd[nStations]/F");
      t->Branch("eta_lct_even", eta_lct_even, "eta_lct_even[nStations]/F");

      t->Branch("perp_lct_odd", perp_lct_odd, "perp_lct_odd[nStations]/F");
      t->Branch("perp_lct_even", perp_lct_even, "perp_lct_even[nStations]/F");

      t->Branch("dphi_lct_odd", dphi_lct_odd, "dphi_lct_odd[nStations]/F");
      t->Branch("dphi_lct_even", dphi_lct_even, "dphi_lct_even[nStations]/F");

      t->Branch("chi2_lct_odd", chi2_lct_odd, "chi2_lct_odd[nStations]/F");
      t->Branch("chi2_lct_even", chi2_lct_even, "chi2_lct_even[nStations]/F");

      t->Branch("timeErr_lct_odd", timeErr_lct_odd, "timeErr_lct_odd[nStations]/F");
      t->Branch("timeErr_lct_even", timeErr_lct_even, "timeErr_lct_even[nStations]/F");

      t->Branch("nHits_lct_odd", nHits_lct_odd, "nHits_lct_odd[nStations]/I");
      t->Branch("nHits_lct_even", nHits_lct_even, "nHits_lct_even[nStations]/I");

      t->Branch("passdphi_odd", passdphi_odd, "passdphi_odd[nStations]/F");
      t->Branch("passdphi_even", passdphi_even, "passdphi_even[nStations]/F");

      t->Branch("lct_type", lct_type, "lct_type[nStations]/I");
    }
  };
}  // namespace

#endif
