import mvdtool as mt
from os import path
_dir,_ = path.split(__file__)

mvdfile=mt.MVD2File(path.join(_dir, "circuit.mvd2"))
n_neurons = mvdfile.getNbNeuron()
assert n_neurons == 1000
