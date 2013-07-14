import FWCore.ParameterSet.Config as cms

process = cms.Process("MUTRG")


################################################################################
# 


dir_muPt5_pu0 = '/pnfs/cms/WAX/11/store/user/lpcgem/dildick/dildick/pT5_1M_v1/DigiL1CSC-MuonGunPt5_1M/82325e40d6202e6fec2dd983c477f3ca/'
dir_muPt20_pu0 = '/pnfs/cms/WAX/11/store/user/lpcgem/dildick/dildick/pT20_1M_v1/DigiL1CSC-MuonGunPt20_1M/82325e40d6202e6fec2dd983c477f3ca/'

dir_muPt20_pu200 = '/pnfs/cms/WAX/11/store/user/lpcgem/dildick/dildick/muonGun_50k_pT20_lpcgem/DigiL1CSC-MuonGunPt20_50k-PU200/0de68e8f275fa8e4e39f6990099d44a2/'
dir_mb_pu100 = '/pnfs/cms/WAX/11/store/user/lpcgem/yasser1/yasser/NeutrinoGunPt5-40_v3/NeutrinoGun_pileup100_Pt5-40_50k_digi/6e54e2af5e7284c73c4665d7969fdf1d/'

dirPt2Pt50 = '/pnfs/cms/WAX/11/store/user/lpcgem/khotilov/khotilov/MuomGUN_SIM_Pt2-50_100k/MuonGun_DIGI_L1_Pt2-50_100k/29891ddb18281fff4c42a6e5f5d4bc55/'

dirWMu = '/pnfs/cms/WAX/11/store/user/lpcgem/dildick/dildick/WtoMuNu_test/WtoMuNu_test/58b3ea3c731a8657fd489c8437f5f991/'

inputDir = dir_muPt20_pu200
inputDir = dir_muPt20_pu0
inputDir = dir_mb_pu100
inputDir = dirPt2Pt50
#inputDir = dirWMu


theOutDir = ''
theFileName = 'out.root'

min_clct_pattern = 2

gem_match = False
gem_match = True

lct_store_gemdphi = False
#lct_store_gemdphi = True

pt_dphi = 'pt40'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

#process.Timing = cms.Service("Timing")
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

from GEMCode.SimMuL1.GEMCSCdPhiLib import *
dphi_lct_pad = dphi_lct_pad98

################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS161_V12::All'
#process.GlobalTag.globaltag = 'DESIGN60_V5::All'

process.load('Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cfi')
process.load('Geometry.CommonDetUnit.globalTrackingGeometry_cfi')
process.load('Geometry.MuonNumbering.muonNumberingInitialization_cfi')
process.load('Geometry.TrackerGeometryBuilder.idealForDigiTrackerGeometryDB_cff')
process.load('Geometry.DTGeometryBuilder.idealForDigiDtGeometryDB_cff')
process.load('Geometry.CSCGeometryBuilder.idealForDigiCscGeometry_cff')
process.load('Geometry.GEMGeometry.gemGeometry_cfi')
process.load('Geometry.GEMGeometry.cmsExtendedGeometryPostLS1plusGEMXML_cfi')




############## digi customization

# GEM digitizer
process.load('SimMuon.GEMDigitizer.muonGEMDigis_cfi')

# GEM-CSC trigger pad digi producer
process.load('SimMuon.GEMDigitizer.muonGEMCSCPadDigis_cfi')


# customization of the process.pdigi sequence to add the GEM digitizer
from SimMuon.GEMDigitizer.customizeGEMDigi import *
#process = customize_digi_addGEM(process)  # run all detectors digi
process = customize_digi_addGEM_muon_only(process) # only muon+GEM digi
#process = customize_digi_addGEM_gem_only(process)  # only GEM digi

from SLHCUpgradeSimulations.Configuration.muonCustoms import customise_csc_geom_cond_digi
process = customise_csc_geom_cond_digi(process)

############### upgrade CSC L1 customizations
process.load("Configuration.StandardSequences.L1Emulator_cff")

# unganged local stubs emilator:
process.load('L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigisPostLS1_cfi')
process.simCscTriggerPrimitiveDigis = process.cscTriggerPrimitiveDigisPostLS1


process.simCscTriggerPrimitiveDigis.CSCComparatorDigiProducer = cms.InputTag( 'simMuonCSCDigis', 'MuonCSCComparatorDigi' )
process.simCscTriggerPrimitiveDigis.CSCWireDigiProducer       = cms.InputTag( 'simMuonCSCDigis', 'MuonCSCWireDigi' )
if gem_match:
    process.simCscTriggerPrimitiveDigis.gemPadProducer =  cms.untracked.InputTag("simMuonGEMCSCPadDigis","")
process.simCscTriggerPrimitiveDigis.clctSLHC.clctPidThreshPretrig = min_clct_pattern
process.simCscTriggerPrimitiveDigis.clctParam07.clctPidThreshPretrig = min_clct_pattern
tmb = process.simCscTriggerPrimitiveDigis.tmbSLHC
tmb.gemMatchDeltaPhiOdd = cms.untracked.double(dphi_lct_pad[pt_dphi]['odd'])
tmb.gemMatchDeltaPhiEven = cms.untracked.double(dphi_lct_pad[pt_dphi]['even'])
tmb.gemMatchDeltaEta = cms.untracked.double(0.08)
tmb.gemMatchDeltaBX = cms.untracked.int32(1)
if lct_store_gemdphi:
    tmb.gemClearNomatchLCTs = cms.untracked.bool(False)
    tmb.gemMatchDeltaPhiOdd = cms.untracked.double(2.)
    tmb.gemMatchDeltaPhiEven = cms.untracked.double(2.)


process.load('L1Trigger.CSCTrackFinder.csctfTrackDigisUngangedME1a_cfi')
process.simCsctfTrackDigis = process.csctfTrackDigisUngangedME1a
process.simCsctfTrackDigis.DTproducer = cms.untracked.InputTag("simDtTriggerPrimitiveDigis")
process.simCsctfTrackDigis.SectorReceiverInput = cms.untracked.InputTag("simCscTriggerPrimitiveDigis","MPCSORTED")

process.load("Configuration.StandardSequences.L1Extra_cff")
from SLHCUpgradeSimulations.Configuration.muonCustoms import customize_l1extra
process = customize_l1extra(process)
l1ep = process.l1extraParticles
l1ep.centralBxOnly = cms.bool(True)
l1ep.produceMuonParticles = cms.bool(True)
l1ep.produceCaloParticles = cms.bool(False)
l1ep.ignoreHtMiss = cms.bool(False)




################################################################################
# Input

import os
ls = os.listdir(inputDir)


process.source = cms.Source("PoolSource",
  duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
  inputCommands = cms.untracked.vstring(
    'keep  *_*_*_*',
    'drop *_simCscTriggerPrimitiveDigis_*_*',
    'drop *_simCsctfTrackDigis_*_*',
    'drop *_simDttfDigis_*_*',
    'drop *_simCsctfDigis_*_*',
    'drop *_simGmtDigis_*_*',
    'drop *_l1extraParticles_*_*'
  ),
  fileNames = cms.untracked.vstring(
    #'file:'+inFile
    #['file:'+inputDir+"/"+x for x in ls if x.endswith('root')]
    #'dcap://cmsdca.fnal.gov:24136/pnfs/fnal.gov/usr/cms/WAX/11/store/user/lpcgem/khotilov/khotilov/MuomGUN_SIM_Pt2-50_100k/MuonGun_DIGI_L1_Pt2-50_100k/29891ddb18281fff4c42a6e5f5d4bc55/out_22_1_pbQ.root'
    [inputDir[16:] + x for x in ls if x.endswith('root')]
  )
)


################################################################################
# Output

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(theOutDir+theFileName),
    outputCommands = cms.untracked.vstring(
        'keep  *_*_*_*',
        # drop all CF stuff
        'drop *_mix_*_*',
        # drop tracker simhits
        'drop PSimHits_*_Tracker*_*',
        # drop calorimetry stuff
        'drop PCaloHits_*_*_*',
        'drop L1Calo*_*_*_*',
        'drop L1Gct*_*_*_*',
        # drop calorimetry l1extra
        'drop l1extraL1Em*_*_*_*',
        'drop l1extraL1Jet*_*_*_*',
        'drop l1extraL1EtMiss*_*_*_*',
        # clean up simhits from other detectors
        'drop PSimHits_*_Totem*_*',
        'drop PSimHits_*_FP420*_*',
        'drop PSimHits_*_BSC*_*',
        # drop some not useful muon digis and links
        'drop *_*_MuonCSCStripDigi_*',
        'drop *_*_MuonCSCStripDigiSimLinks_*',
        'drop *SimLink*_*_*_*',
        'drop *RandomEngineStates_*_*_*',
        'drop *_randomEngineStateProducer_*_*'
    )
)


################################################################################
# custom sequences


process.mul1 = cms.Sequence(
  process.SimL1MuTriggerPrimitives *
  process.SimL1MuTrackFinders *
  process.simRpcTriggerDigis *
  process.simGmtDigis *
  process.L1Extra
)


process.muL1Short = cms.Sequence(
  process.simCscTriggerPrimitiveDigis *
  process.SimL1MuTrackFinders *
  process.simGmtDigis *
  process.L1Extra
)


################################################################################
# define path-steps

process.gem_pads        = cms.Path(process.simMuonGEMCSCPadDigis)
#process.l1emu_step      = cms.Path(process.mul1)
process.l1emu_step      = cms.Path(process.muL1Short)
process.endjob_step     = cms.Path(process.endOfProcess)
process.out_step        = cms.EndPath(process.output)

################################################################################
# Schedule definition

process.schedule = cms.Schedule(
    process.gem_pads,
    process.l1emu_step,
    process.endjob_step,
    process.out_step
)

