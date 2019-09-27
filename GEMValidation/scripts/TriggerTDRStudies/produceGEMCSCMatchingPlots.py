import os

from ROOT import *

## run quiet mode
import sys
sys.argv.append( '-b' )

import ROOT
gROOT.SetBatch(1)

from GEMCSCValidation import simTrackToCscSimHitMatching
from GEMCSCValidation import simTrackToCscStripsWiresMatching
from GEMCSCValidation import simTrackToCscStripsWiresMatching_2
from GEMCSCValidation import simTrackToCscAlctClctMatching
from GEMCSCValidation import simTrackToCscAlctClctMatching_2
from GEMCSCValidation import simTrackToCscLctMatching
from GEMCSCValidation import simTrackToCscMpLctMatching

def enum(*sequential, **named):
  enums = dict(zip(sequential, range(len(sequential))), **named)
  reverse = dict((value, key) for key, value in enums.iteritems())
  enums['reverse_mapping'] = reverse
  return type('Enum', (), enums)

class TriggerTDREfficiencyPlotter():
  def __init__(self):
    self.inputDir = os.getenv("CMSSW_BASE") + "/src/GEMCode/GEMValidation/scripts/TriggerTDRStudies/"
    self.inputFile = "../../../../out_ana.root"
    self.targetDir = "plots/"
    self.ext = ".png"
    self.analyzer = "TDRAnalyzer"
    self.effSt = "trk_eff_"
    self.stations = enum('CSC_ME11',
                         'CSC_ME12','CSC_ME13','CSC_ME21','CSC_ME22',
                         'CSC_ME31','CSC_ME32','CSC_ME41','CSC_ME42')
    self.stationsToUse = [self.stations.CSC_ME11,
                          self.stations.CSC_ME12,
                          self.stations.CSC_ME13,
                          self.stations.CSC_ME21,
                          self.stations.CSC_ME22,
                          self.stations.CSC_ME31,
                          self.stations.CSC_ME32,
                          self.stations.CSC_ME41,
                          self.stations.CSC_ME42]
    self.file = TFile.Open(self.inputDir + self.inputFile)
    self.dirAna = (self.file).Get(self.analyzer)
    self.treeEffSt = []
    for x in self.stationsToUse:
      print self.effSt + self.stations.reverse_mapping[x]
      self.treeEffSt.append(self.dirAna.Get(self.effSt + self.stations.reverse_mapping[x]))
    self.yMin = 0.5
    self.yMax = 1.05
    self.etaMin = 1.55
    self.etaMax = 2.45
    self.pu = 0
    self.matchAlctGem = True

print "Make new plotter"
plotter = TriggerTDREfficiencyPlotter()
for i in range(len(plotter.stationsToUse)):
  st = plotter.stationsToUse[i]
  print "Processing station ", plotter.stations.reverse_mapping[st]
  print plotter.stationsToUse.index(st), plotter.treeEffSt[plotter.stationsToUse.index(st)]
  simTrackToCscSimHitMatching(plotter,st)
  continue
  #simTrackToCscStripsWiresMatching(plotter,st)
  simTrackToCscStripsWiresMatching_2(plotter,st)
  simTrackToCscAlctClctMatching(plotter,st)
  simTrackToCscAlctClctMatching_2(plotter,st)
  simTrackToCscLctMatching(plotter,st)
  #simTrackToCscMpLctMatching(plotter,st)
