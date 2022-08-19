from os import system as bash
import pdb

"""
Produces geometry file ntuples from a given config file
"""

if __name__ == "__main__":

    #script = "/afs/cern.ch/work/e/escalant/private/SND_LHC/sndsw/shipLHC/run_geoSND.py"
    script = "/eos/home-e/escalant/test/sndsw/shipLHC/run_geoSND.py"
    outDir = "/eos/user/e/escalant/lxplus/SND_LHC/data/geometries/"

    geometry = []
    geometry.append("--AdvSND") #configuration = "/afs/cern.ch/work/e/escalant/private/SND_LHC/sndsw/geometry/AdvSND_geom_config.py"
    geometry.append("--testbeam")     #configuration = "$SNDSW_ROOT/geometry/sndLHC_H6geom_config.py"
    geometry.append("--myGeo")     #configuration = "$SNDSW_ROOT/geometry/sndLHC_H6geom_config.py"
    geometry.append("")         #configuration = "$SNDSW_ROOT/geometry/sndLHC_geom_config.py"

    for kgeometry in geometry:
        command = "python {SCRIPT} --verbose {GEOMETRY} --output {OUTDIR}".format(SCRIPT=script, GEOMETRY=kgeometry, OUTDIR=outDir)
        print(command)
        bash(command)
        pdb.set_trace()
