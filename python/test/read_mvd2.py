import unittest as test
import mvdtool as mt
import numpy

from os import path
_dir,_ = path.split(__file__)


class Test_Read_MVD2(test.TestCase):
    mvd_path = path.join(_dir, "circuit.mvd2")

    def test_is_mvd(self):
        assert mt.is_mvd_file(self.mvd_path) == mt.MVDType.MVD2

    def test_totals(self):
        mvd = mt.MVD2File(self.mvd_path)
        assert mvd.getNbNeuron() == 1000
        assert mvd.getNbMorphoType() == 9
        assert mvd.getNbMorpho() == 52
        assert mvd.getNbColumns() == 10

    def test_getPositions_Rotations(self,):
        mvd = mt.MVD2File(self.mvd_path)

        posics = mvd.getPositions()
        # assert len(posics) == 1000
        # assert numpy.allclose(posics[0], [40.821401, 1986.506637, 10.788424])
        # assert numpy.allclose(posics[146], [26.931492, 1771.498694, 84.290181])

        #rots = mvd.getRotations()
        print "hello"
        # assert len(rots) == 1000
        # assert numpy.isclose(rots[0], -1.146572)
        # assert numpy.isclose(rots[146], -125.718090)



if __name__ == '__main__':
    test.main(verbosity=2, buffer=None)
