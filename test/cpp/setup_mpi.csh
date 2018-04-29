
if ( `uname` == "Darwin" ) then
  set mpi_home=/opt/local/libexec/openmpi-mp
  setenv PATH "${mpi_home}:${PATH}"

else if ( `uname -n | grep ccage ` != "" ) then
  set mpi_home=/usr/lib64/openmpi
  setenv PATH "${mpi_home}/bin:${PATH}"
  if ( "`printenv LD_LIBRARY_PATH`" == "" ) then
    setenv LD_LIBRARY_PATH "${mpi_home}/lib"
  else
    setenv LD_LIBRARY_PATH "${mpi_home}/lib:${LD_LIBRARY_PATH}"
  endif
endif


endif


# exa of usage :
# csh> mpirun -np 2 ./bin_sdk_mac/mpi -verbose

