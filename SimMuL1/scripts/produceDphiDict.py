##
## This script produces the dictionary of bending angles
##

from cuts import *

## run quiet mode
import sys
sys.argv.append( '-b' )

from ROOT import *
gROOT.SetBatch(1)

def getTree(fileName):
    """Get tree for given filename"""

    analyzer = "GEMCSCAnalyzer"
    trk_eff = "trk_eff"

    file = TFile.Open(fileName)
    if not file:
        sys.exit('Input ROOT file %s is missing.' %(fileName))

    dir = file.Get(analyzer)
    if not dir:
        sys.exit('Directory %s does not exist.' %(dir))
        
    tree = dir.Get(trk_eff)
    if not tree:
        sys.exit('Tree %s does not exist.' %(tree))

    return tree

def dphiCut(h, fractionToKeep):
    """Get the dPhi cut corresponding to the fractionToKeep [95,98,99]"""

    ax = h.GetXaxis()
    total = h.Integral()
    bin = 1
    fractionToKeep = fractionToKeep/100.
    for b in range(ax.GetNbins()):
        if (h.Integral(0,b)/total > fractionToKeep):
            bin = b - 1
            break

    ## interpolate
    x1 = ax.GetBinUpEdge(bin)
    x2 = ax.GetBinUpEdge(bin + 1)
    y1 = h.Integral(0, bin)/total
    y2 = h.Integral(0, bin + 1)/total
    x = x1 + (fractionToKeep - y1)/(y2-y1)*(x2-x1)
    return x


def produceDphiDict(filesDir, outFileName):
    """Get the libary of dPhi values"""

    pt = ["pt5","pt10","pt15","pt20","pt30","pt40"]
    fr = [95, 98, 99]
    dphis = [[[0 for x in xrange(2)] for x in xrange(len(fr))] for x in xrange(len(pt))] 
    
    for n in range(len(pt)):
        t = getTree("%sgem_csc_delta_%s_pad4.root"%(filesDir,pt[n]))
        t.Draw("TMath::Abs(dphi_pad_odd)>>dphi_odd(600,0.,0.03)" , ok_pad1_lct1)
        t.Draw("TMath::Abs(dphi_pad_even)>>dphi_even(600,0.,0.03)" , ok_pad2_lct2)
        h_dphi_odd = TH1F(gDirectory.Get("dphi_odd"))
        h_dphi_even = TH1F(gDirectory.Get("dphi_even"))
        for f in range(len(fr)):
            dphis[n][f][0] = dphiCut(h_dphi_odd, fr[f])
            dphis[n][f][1] = dphiCut(h_dphi_even, fr[f])
            
    ## print the dphi library for these samples
    outfile = open("%s"%(outFileName),"w")
    outfile.write("dphi_lct_pad = {\n")
    for f in range(len(fr)):
        outfile.write('    "%d" : {\n'%(fr[f]))
        if f == len(fr)-1:
            endchar1 = ""
        else:
            endchar1 = ","
        for n in range(len(pt)):
            pt_string = ("%s"%(pt[n])).ljust(4)
            if n == len(pt)-1:
                endchar = ""
            else:
                endchar = ","
            outfile.write("        '%s' : { 'odd' :  %.8f, 'even' : %.8f }%s\n"%(pt_string, dphis[n][f][0], dphis[n][f][1], endchar))
        outfile.write('        }%s\n'%(endchar1))

    outfile.write('}\n')

    ## close the output file
    outfile.close()

    ## print some additional information
    print "dPhi library written to:", outfile.name
    outfile = open("%s"%(outFileName),"r")
    print outfile.read()
    outfile.close()


if __name__ == "__main__":  
    produceDphiDict("files/", "GEMCSCdPhiDict.py")
