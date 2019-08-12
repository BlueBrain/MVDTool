"""Convert MVD3 circuits to SONATA files.
"""
import argparse
import h5py
import numpy as np


class Converter:
    """Convert an MVD3 file to SONATA

    Not all fields will be translated, only a minimal subset will be
    retained:

     - etype
     - mtype
     - morphology
     - region
     - synapse_class

    As well as position and rotation.

    Args:
        mvd3: the input filename
        sonata: the output filename
        population: the name of the nodeset in the SONATA file
        group: the node group to write
        offset: the first entry to copy
        entries: the number of entries to copy
        stride: select every nth element
    """

    __ENUMS__ = ["etype", "mtype", "morphology", "region", "synapse_class"]

    def __init__(
        self,
        mvd3: str,
        sonata: str,
        population: str = "default",
        group: int = 0,
        offset: int = 0,
        entries: int = -1,
        stride: int = 1,
    ) -> None:
        self._mvd = h5py.File(mvd3, "r")
        self._sonata = h5py.File(sonata, "a")
        self._root = f"/nodes/{population}/"
        self._dirname = f"/nodes/{population}/{group}"
        self._0 = offset
        if entries > 0:
            self._n = offset + entries * stride
        else:
            self._n = len(self._mvd["/cells/properties/mtype"]) - offset
        self._s = stride
        print(f"{self._0}, {self._n}, {self._s}")

    def transfer_attribute(self, name: str) -> None:
        """Copy an enumeration attribute

        Args:
            name: the name of the attribute to copy
        """
        self._sonata.create_dataset(
            f"{self._dirname}/@library/{name}", data=self._mvd[f"/library/{name}"]
        )
        self._sonata.create_dataset(
            f"{self._dirname}/{name}",
            data=self._mvd[f"/cells/properties/{name}"][self._0 : self._n : self._s],
        )

    def create_ids(self) -> None:
        """Copy the positions
        """
        pos = self._mvd[f"/cells/positions"]
        self._sonata.create_dataset(
            f"{self._root}/node_type_id",
            data=np.zeros(shape=pos.shape[0])[self._0 : self._n : self._s],
        )
        self._sonata.create_dataset(
            f"{self._root}/node_group_id",
            data=np.zeros(shape=pos.shape[0])[self._0 : self._n : self._s],
        )

    def transfer_positions(self) -> None:
        """Copy the positions
        """
        pos = self._mvd[f"/cells/positions"]
        for dim, name in enumerate("xyz"):
            self._sonata.create_dataset(
                f"{self._dirname}/{name}", data=pos[self._0 : self._n : self._s, dim]
            )

    def transfer_orientations(self) -> None:
        """Copy the rotation quaternions, swapping elements as needed
        """
        rot = self._mvd[f"/cells/orientations"]
        for dim, name in enumerate("xyzw"):
            self._sonata.create_dataset(
                f"{self._dirname}/orientation_{name}",
                data=rot[self._0 : self._n : self._s, dim],
            )

    def export(self) -> None:
        """Translate the files
        """
        self.create_ids()
        self.transfer_positions()
        self.transfer_orientations()
        for e in self.__ENUMS__:
            try:
                self.transfer_attribute(e)
            except KeyError as err:
                print(f"Unable to transfer {e}: {err}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Convert MVD3 to SONATA")
    parser.add_argument(
        "--population", default="default", type=str, help="which population to write to"
    )
    parser.add_argument(
        "--offset",
        default=0,
        type=int,
        help="the first entry to copy (defaults to zero)",
    )
    parser.add_argument(
        "--entries",
        default=-1,
        type=int,
        help="how many entries to copy (must be greater zero, defaults to all)",
    )
    parser.add_argument(
        "--stride",
        default=1,
        type=int,
        metavar="N",
        help="copy every Nth entry (defaults to 1)",
    )
    parser.add_argument("MVD3")
    parser.add_argument("SONATA")
    args = parser.parse_args()
    Converter(
        args.MVD3,
        args.SONATA,
        population=args.population,
        offset=args.offset,
        entries=args.entries,
        stride=args.stride,
    ).export()
