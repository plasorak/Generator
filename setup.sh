
setup geant4 v4_11_1_p01ba -q e26:prof
setup inclxx v5_2_9_5e -f Linux64bit+3.10-2.17 -q "e26:prof"
setup lhapdf v6_5_4 -f Linux64bit+3.10-2.17 -q "e26:p3915:prof"
setup log4cpp v1_1_3e -f Linux64bit+3.10-2.17 -q "e26:prof"

./configure \
    --enable-lhapdf6 \
    --disable-lhapdf5 \
    --enable-incl \
    --enable-geant4 \
    --enable-nucleon-decay \
    --with-incl-lib=/cvmfs/larsoft.opensciencegrid.org/products/inclxx/v5_2_9_5e/Linux64bit+3.10-2.17-e26-prof/lib \
    --with-incl-inc=/cvmfs/larsoft.opensciencegrid.org/products/inclxx/v5_2_9_5e/Linux64bit+3.10-2.17-e26-prof/include/inclxx

make

export PATH=/dune/app/users/plasorak/LiquidO/DUNE-LO-Sims/Generator/bin:$PATH
export LD_LIBRARY_PATH=/dune/app/users/plasorak/LiquidO/DUNE-LO-Sims/Generator/lib:$LD_LIBRARY_PATH

echo
echo
echo "to generate events"
echo
echo
echo "gevgen --tune AR23_20b_00_000 -n 10 -e 1 -p 14 -t 1000060120 --cross-sections /dune/app/users/plasorak/LiquidO/DUNE-LO-Sims/genie_xsec/v3_04_00/NULL/AR2320b00000-k250-e1000/data/gxspl-NUsmall.xml"
