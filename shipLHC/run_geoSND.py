#!/usr/bin/env python
import os
import sys
import ROOT
import json

import shipunit as u
import shipRoot_conf
import rootUtils as ut
from ShipGeoConfig import ConfigRegistry
from argparse import ArgumentParser

import pprint 

parser = ArgumentParser()
group = parser.add_mutually_exclusive_group()

group.add_argument("--testbeam",   help="use geometry of H8/H6 testbeam setup", required=False, action="store_true")
group.add_argument("--AdvSND",   help="Use AdvSND setup", required=False, action="store_true")
group.add_argument("--myGeo",   help="Use myGeo setup", required=False, action="store_true")
parser.add_argument("-o", "--output",dest="outputDir",  help="Output directory", required=False,  default=".")
parser.add_argument("--verbose",   dest="verbose",   help="verbose mode, useful to learn the code", required=False, action="store_true")

options = parser.parse_args()

# user hook
userTask = False

print(options)

if options.verbose:  import pdb

def pause(msg, last = False, skip_pdb = False):
    #used for debug
    verboseTag = "[VERBOSE] "
    print( verboseTag, msg)
    if skip_pdb == False: pdb.set_trace()
    if last == True: sys.exit()

# running in verbose mode
if options.verbose:  pause("Begin - verbose -", skip_pdb=True)

class MyTask(ROOT.FairTask):
    "user task"

    def Exec(self,opt):
        ioman = ROOT.FairRootManager.Instance()
        MCTracks = ioman.GetObject("MCTrack")
        print('Hello',opt,MCTracks.GetEntries())
        fMC = ROOT.TVirtualMC.GetMC()
        if MCTracks.GetEntries()>100:  fMC.StopRun()

shipRoot_conf.configure(0)     # load basic libraries, prepare atexit for python

if options.testbeam:  
    tag = "testbeam"
    snd_geo = ConfigRegistry.loadpy("$SNDSW_ROOT/geometry/sndLHC_H6geom_config.py")

elif options.AdvSND:
    #at the moment $SNDSW_ROOT points to a compilation of master (where advSND_geom_config.py does not exist)
    #snd_geo = ConfigRegistry.loadpy("$SNDSW_ROOT/geometry/AdvSND_geom_config.py") 
    tag = "AdvSND"
    snd_geo = ConfigRegistry.loadpy("/afs/cern.ch/work/e/escalant/private/SND_LHC_escalant/sndsw/geometry/AdvSND_geom_config.py")
elif options.myGeo:
    #at the moment $SNDSW_ROOT points to a compilation of master (where advSND_geom_config.py does not exist)
    #snd_geo = ConfigRegistry.loadpy("$SNDSW_ROOT/geometry/AdvSND_geom_config.py") 
    tag = "myGeo"
    snd_geo = ConfigRegistry.loadpy("/afs/cern.ch/work/e/escalant/private/SND_LHC_escalant/sndsw/geometry/myGeom_config.py")
    #print("removing detectors")
    #del snd_geo['EmulsionDet']
    #del snd_geo['MuFilter']
    #del snd_geo['Scifi']
    #del snd_geo['Magnet']
else:
    tag = "NominalSND"
    snd_geo = ConfigRegistry.loadpy("$SNDSW_ROOT/geometry/sndLHC_geom_config.py")

pdb.set_trace()
if options.verbose:
    pp = pprint.PrettyPrinter(depth=2)
    pp.pprint(snd_geo)
    pause("GEOMETRY LOADED")

if not os.path.exists(options.outputDir):
  os.makedirs(options.outputDir)

# In general, the following parts need not be touched, except for user task
# ========================================================================

# ------------------------------------------------------------------------
# -----Create simulation run----------------------------------------
run = ROOT.FairRunSim()
mcEngine     = "TGeant4"
run.SetName(mcEngine)  # Transport engine
run.SetOutputFile(options.outputDir+"dummy.root")
run.SetUserConfig("g4Config.C") # user configuration file default g4Config.C 
#rtdb = run.GetRuntimeDb() 
# add user task
if userTask:
  userTask   = MyTask()
  run.AddTask(userTask)

if options.verbose:  
    print(userTask)
    pause("added userTask", skip_pdb=True)

# -----Create geometry----------------------------------------------
import shipLHC_conf as sndDet_conf
modules = sndDet_conf.configure(run,snd_geo)
if options.verbose:  
    print(modules)
    pause("create geometry with shipLHC_conf.configure()")

# -----Initialize simulation run------------------------------------
run.Init()

if options.verbose:  
    pause("Simulation Run is about to START", skip_pdb=True)

geoFile = "%s/geofile_full_%s.root" % (options.outputDir, tag)
if options.verbose:  
    pause("Create geometry file", skip_pdb=True)

run.CreateGeometryFile(geoFile)
# save detector parameters dictionary in geofile
import saveBasicParameters
saveBasicParameters.execute(geoFile,snd_geo)

# save geometry to text file
geoFileConfig = open("%s/geofile_full_%s.json" % (options.outputDir, tag), "w")
json.dump(snd_geo, geoFileConfig, indent=4)
geoFileConfig.close()

# -----Finish-------------------------------------------------------
print(' ') 
print("Macro finished succesfully.") 

print("Geometry .root file is ",geoFile)
print("Geometry .json file is ",geoFileConfig.name)

