import ROOT as rt
import sys


def printAboveThreshold(threshold,tree,runNumber,c,channels=["ch1","ch2"]):

    condition = ""
    for ch in channels:
        
        condition += "(amplitude.%s/65535.>%f)*"%(ch,threshold)
    condition = condition[:-1] # remove trailing *
    condition = "("+condition+")"
    
    
    entries = tree.Draw("t1>>htemp(1024,0,1024)","-c1*%s"%(condition))
    events = entries/1024
    tree.Draw("t1>>htemp(1024,0,1024)","-c1/%i*%s"%(events,condition))
    c.Print("run%i_ch1_%sgt%imV.png"%(runNumber,"and".join(channels),threshold*1000))
    tree.Draw("t2>>htemp(1024,0,1024)","-c2/%i*%s"%(events,condition))
    c.Print("run%i_ch2_%sgt%imV.png"%(runNumber,"and".join(channels),threshold*1000))
    tree.Draw("t3>>htemp(1024,0,1024)","-c3/%i*%s"%(events,condition))
    c.Print("run%i_ch3_%sgt%imV.png"%(runNumber,"and".join(channels),threshold*1000))
    tree.Draw("t4>>htemp(1024,0,1024)","-c4/%i*%s"%(events,condition))
    c.Print("run%i_ch4_%sgt%imV.png"%(runNumber,"and".join(channels),threshold*1000))
    
    print ""
    print "%i events with %s above %i mV amplitude" %(events," AND ".join(channels),threshold*1000)
    print ""
    
if __name__ == "__main__":

    if len(sys.argv)<2 or ('.root' not in sys.argv[1]):    
        print "give a root file as input"
        sys.exit()
    fileName = sys.argv[1]
    rootFile = rt.TFile.Open(fileName)
    tree = rootFile.Get("T")

    runNumber = int(fileName.split(".root")[0].split("_")[-1])
    
    c = rt.TCanvas()
    
    #just ch1 above 0 mV
    printAboveThreshold(0,tree,runNumber,c,["ch1"]) # 
    
    #just ch3 above 10 mV
    printAboveThreshold(0.01,tree,runNumber,c,["ch3"])
    
    #ch1 AND ch2 above 100 mV
    printAboveThreshold(0.1,tree,runNumber,c,["ch1","ch2"])


