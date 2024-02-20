
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup root v6_26_06b -q e20:p3913:prof
setup geant4 v4_11_1_p01ba -q e26:prof
setup inclxx v5_2_9_5e -f Linux64bit+3.10-2.17 -q "e26:prof"
setup lhapdf v6_5_4 -f Linux64bit+3.10-2.17 -q "e26:p3915:prof"
setup log4cpp v1_1_3e -f Linux64bit+3.10-2.17 -q "e26:prof"

SH_SOURCE=${BASH_SOURCE[0]:-${(%):-%x}}
HERE=$(cd $(dirname ${SH_SOURCE}) && pwd)
export GENIE=${HERE}

./configure \
    --enable-lhapdf6 \
    --disable-lhapdf5 \
    --enable-incl \
    --enable-geant4 \
    --enable-nucleon-decay \
    --with-incl-lib=/cvmfs/larsoft.opensciencegrid.org/products/inclxx/v5_2_9_5e/Linux64bit+3.10-2.17-e26-prof/lib \
    --with-incl-inc=/cvmfs/larsoft.opensciencegrid.org/products/inclxx/v5_2_9_5e/Linux64bit+3.10-2.17-e26-prof/include/inclxx

make -j`nproc`

export PATH=${GENIE}/bin:$PATH
export LD_LIBRARY_PATH=${GENIE}/lib:$LD_LIBRARY_PATH

wget https://scisoft.fnal.gov/scisoft/packages/genie_xsec/v3_04_00/genie_xsec-3.04.00-noarch-AR2320i00000-k250-e1000.tar.bz2

tar -xvf genie_xsec-3.04.00-noarch-AR2320i00000-k250-e1000.tar.bz2

sed -i 's\AR23_20i_00_000\AR23_20b_00_000\g' ${GENIE}/genie_xsec/v3_04_00/NULL/AR2320b00000-k250-e1000/data/gxspl-NUsmall.xml

echo
echo
echo "To generate events:"
echo
echo
echo "gevgen --tune AR23_20b_00_000 -n 10 -e 1 -p 14 -t 1000060120 --cross-sections ${GENIE}/genie_xsec/v3_04_00/NULL/AR2320b00000-k250-e1000/data/gxspl-NUsmall.xml"
echo
echo