import unittest as test
import mvdtool as mt
import numpy

from os import path
_dir,_ = path.split(__file__)

class Test_Read_MVD3(test.TestCase):
    mvd_path = path.join(_dir, "circuit.mvd3").encode()

    def test_is_mvd(self):
        assert mt.is_mvd_file(self.mvd_path) == mt.MVDType.MVD3

    def test_number_neurons(self):
        mvd = mt.MVD3File(self.mvd_path)
        assert mvd.getNbNeuron() == 1000

    def test_positions_vals(self):
        mvd = mt.MVD3File(self.mvd_path)
        posics = mvd.getPositions()
        assert posics.shape[0] == 1000
        assert numpy.allclose(posics[0], [40.821401, 1986.506637, 10.788424])

    def test_positions_range(self):
        mvd = mt.MVD3File(self.mvd_path)
        posics = mvd.getPositions()
        posics_range = mvd.getPositions(mt.MVD3.Range(10,10))

        assert len(posics_range) == 10
        assert numpy.allclose(posics_range, posics[10:20])


if __name__ == '__main__':
    test.main(verbosity=2, buffer=None)
