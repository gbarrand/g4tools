
if [ "`uname`" = "Darwin" ] ; then
  mpi_home=/opt/local/libexec/openmpi-mp
  PATH="${mpi_home}:${PATH}"
  export PATH

elif [ "`uname -n | grep ccage `" != "" ] ; then
  mpi_home=/usr/lib64/openmpi
  PATH="${mpi_home}/bin:${PATH}"
  export PATH
  if [ -z "${LD_LIBRARY_PATH}" ] ; then
    LD_LIBRARY_PATH="${mpi_home}/lib"
    export LD_LIBRARY_PATH
  else
    LD_LIBRARY_PATH="${mpi_home}/lib:${LD_LIBRARY_PATH}"
    export LD_LIBRARY_PATH
  fi
fi

