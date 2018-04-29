// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

// to run :
//   Darwin> mpirun-openmpi-mp -np 3 ./bin_clang/mpi_histos -verbose

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/mpi/world>
#include <tools/mpi/hmpi>

#include <tools/wroot/file>
#include <tools/wroot/to>
#include <tools/histo/histos>

#include <tools/randd>
#include <tools/args>

#include <iostream>
#include <cstdlib>

int main(int argc,char **argv) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM

  std::string progname = tools::nosuffix(argv[0]);

  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  tools::mpi::world mpi_world;
  tools::impi_world& _mpi = mpi_world;
  // use the interface :
  if(!_mpi.init(&argc,&argv)) {
    std::cout << "MPI_Init() failed." << std::endl;
    return EXIT_FAILURE;
  }
  int comm_size;
  if(!_mpi.size(comm_size)) {}
  if(verbose) std::cout << "Comm size " << comm_size << std::endl;
  if(comm_size<3) {
    std::cout << "This program requires at least three processes." << std::endl;
    return EXIT_FAILURE;
  }
  int rank;
  if(!_mpi.rank(rank)) {}

 {std::string name;
  if(!_mpi.processor_name(name)) {}
  if(verbose) std::cout << "rank " << rank << " : processor name " << name << std::endl;}

  const int tag = 13;

  typedef std::pair<std::string,void*> class_pointer;

  if(rank==0) {

    //////////////////////////////////////////////////////////
    /// create a .root file : ////////////////////////////////
    //////////////////////////////////////////////////////////
    std::string file = progname+".root";
    tools::wroot::file rfile(std::cout,file);
    if(!rfile.is_open()) {
      std::cout << "rank " << rank << " : can't open root file " << file << std::endl;
      return EXIT_FAILURE;
    }

    /////////////////////////////////////////////////////////////////////////
    /// wait histos from rank=1,2 : /////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    MPI_Comm comm = MPI_COMM_WORLD;
    tools::mpi::hmpi hmpi(std::cout,rank,tag,comm,verbose);
    tools::histo::hmpi& _hmpi = hmpi;

    std::vector<class_pointer> hs;

   {int rank_src = 1;
    if(!_hmpi.wait_histos(rank_src,hs)) {
      std::cout << "rank " << rank << " : wait_histos from " << rank_src << " : failed." << std::endl;
      return EXIT_FAILURE;
    }
    if(!tools::wroot::write_histos(rfile.dir(),hs)) {
      std::cout << "rank " << rank << " : write_histos to root file failed." << std::endl;
      return EXIT_FAILURE;
    }
    tools::histo::delete_histos(hs);}

   {int rank_src = 2;
    if(!_hmpi.wait_histos(rank_src,hs)) {
      std::cout << "rank " << rank << " : wait_histos from " << rank_src << " : failed." << std::endl;
      return EXIT_FAILURE;
    }
    if(!tools::wroot::write_histos(rfile.dir(),hs)) {
      std::cout << "rank " << rank << " : write_histos to root file failed." << std::endl;
      return EXIT_FAILURE;
    }
    tools::histo::delete_histos(hs);}

    // wait another bunch of histos from rank=2 by using the same tools::mpi::hmpi object :
   {int rank_src = 2;
    if(!_hmpi.wait_histos(rank_src,hs)) {
      std::cout << "rank " << rank << " : wait_histos from " << rank_src << " : failed." << std::endl;
      return EXIT_FAILURE;
    }
    if(!tools::wroot::write_histos(rfile.dir(),hs)) {
      std::cout << "rank " << rank << " : write_histos to root file failed." << std::endl;
      return EXIT_FAILURE;
    }
    tools::histo::delete_histos(hs);}

    //////////////////////////////////////////////////////////
    /// write and close file : ///////////////////////////////
    //////////////////////////////////////////////////////////
   {unsigned int n;
    if(!rfile.write(n)) {
      std::cout << "root file write failed." << std::endl;
      return EXIT_FAILURE;
    }}
    rfile.close();
    if(verbose) std::cout << "root file " << file << " closed." << std::endl;

  }

  if(rank==1) {

    /////////////////////////////////////////////////////////////////////////
    /// create some histos : ////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    tools::histo::h1d h1("h1d",100,-5,5);
   {tools::rgaussd rg(1,2);
    unsigned int entries = 10000;
    for(unsigned int count=0;count<entries;count++) h1.fill(rg.shoot(),1.4);}

    tools::histo::h2d h2("h2d",100,-5,5,100,-2,2);
   {tools::rgaussd rg(1,2);
    tools::rgaussd rbw(0,1);
    unsigned int entries = 10000;
    for(unsigned int count=0;count<entries;count++) h2.fill(rg.shoot(),rbw.shoot());}

    /////////////////////////////////////////////////////////////////////////
    /// send histos to rank 0 : /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    MPI_Comm comm = MPI_COMM_WORLD;
    tools::mpi::hmpi hmpi(std::cout,rank,tag,comm,verbose);
    tools::histo::hmpi& _hmpi = hmpi;

   {_hmpi.beg_send(2); //IMPORTANT : give first the number of histos.
    _hmpi.pack(h1);
    _hmpi.pack(h2);
    int rank_dest = 0;
    if(!_hmpi.send(rank_dest)) {
      std::cout << "rank " << rank << " : can't send histos." << std::endl;
      return EXIT_FAILURE;
    }}

  }

  if(rank==2) {

    tools::histo::h3d h3("h3d",100,-5,5,100,-5,5,100,-2,2);
   {tools::rgaussd rg(1,2);
    tools::rgaussd rbw(0,1);
    unsigned int entries = 10000;
    for(unsigned int count=0;count<entries;count++) h3.fill(rg.shoot(),rg.shoot(),rbw.shoot());}

    /////////////////////////////////////////////////////////////////////////
    /// create some profiles : //////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    tools::histo::p1d p1("p1d",100,-5,5,-2,2);
   {tools::rgaussd rg(1,2);
    unsigned int entries = 10000;
    for(unsigned int count=0;count<entries;count++) p1.fill(rg.shoot(),rg.shoot(),1.4);}

    tools::histo::p2d p2("p2d",100,-5,5,100,-5,5,-2,2);
   {tools::rgaussd rg(1,2);
    tools::rgaussd rbw(0,1);
    unsigned int entries = 10000;
    for(unsigned int count=0;count<entries;count++) p2.fill(rg.shoot(),rbw.shoot(),rg.shoot());}

    /////////////////////////////////////////////////////////////////////////
    /// send histos to rank 0 : /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    MPI_Comm comm = MPI_COMM_WORLD;
    tools::mpi::hmpi hmpi(std::cout,rank,tag,comm,verbose);
    tools::histo::hmpi& _hmpi = hmpi;

   {_hmpi.beg_send(3); //IMPORTANT : give first the number of histos.
    _hmpi.pack(p1);
    _hmpi.pack(p2);
    _hmpi.pack(h3);
    int rank_dest = 0;
    if(!_hmpi.send(rank_dest)) {
      std::cout << "rank " << rank << " : can't send histos." << std::endl;
      return EXIT_FAILURE;
    }}

    // have another sending by using the same tools::mpi::hmpi object :
   {_hmpi.beg_send(2); //IMPORTANT : give first the number of histos.
    _hmpi.pack(p1);
    _hmpi.pack(p2);
    int rank_dest = 0;
    if(!_hmpi.send(rank_dest)) {
      std::cout << "rank " << rank << " : can't send histos." << std::endl;
      return EXIT_FAILURE;
    }}

  }

  ::MPI_Finalize();

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM
  return EXIT_SUCCESS;
}
