import mvdtool as mt
from os import path
_dir,_ = path.split(__file__)

mvdfile=mt.MVD3File(path.join(_dir, "circuit.mvd3"))
posics = mvdfile.getPositions()
assert len(posics) == 1000
