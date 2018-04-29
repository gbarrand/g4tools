// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

// to run :
//   Darwin> mpirun-openmpi-mp -np 3 ./bin_clang/mpi_ntuple -verbose

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/mpi/world>
#include <tools/mpi/wrmpi>

#include <tools/wroot/file>
#include <tools/wroot/ntuple>
#include <tools/wroot/mpi_ntuple_row_wise>
#include <tools/wroot/mpi_ntuple_column_wise>

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>

// if reading the file at end :
#include <tools/rroot/file>
#include <tools/rroot/ntuple>
#include <tools/rroot/fac>
#include <tools/eqT>

#include <tools/args>

#ifdef TOOLS_DONT_HAVE_ZLIB
#else
#include <tools/zlib>
#endif

#include <iostream>
#include <cstdlib>

int main(int argc,char **argv) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  
  tools::mpi::world mpi_world;
  tools::impi_world& _mpi = mpi_world;
  // use the interface :
  if(!_mpi.init(&argc,&argv)) {
    std::cout << "MPI_Init() failed." << std::endl;
    return EXIT_FAILURE;
  }
  int comm_size;
  if(!_mpi.size(comm_size)) {}
  if(comm_size<3) {
    std::cout << "This program requires at least three processes. Only " << comm_size << " given." << std::endl;
    return EXIT_FAILURE;
  }
  int rank;
  if(!_mpi.rank(rank)) {}

  std::string processor_name;
  if(!_mpi.processor_name(processor_name)) {}

  const int tag = 13;

  // the ntuple_booking should be the same for all ranks (in this program it is not passed through MPI to ranks) :
  double user_rgauss;
  float user_rbw;
  std::string user_string;
  int user_count;
  std::vector<double> user_vec_d;
  std::vector<std::string> user_vec_s;
  
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  nbk.add_column("rgauss",user_rgauss);
  nbk.add_column("rbw",user_rbw);
  nbk.add_column("string",user_string);
//nbk.add_column<std::string>("string");
  nbk.add_column("count",user_count);
  nbk.add_column("vec_d",user_vec_d);
  nbk.add_column("vec_s",user_vec_s);

  if(rank==0) {

    std::string progname = tools::nosuffix(argv[0]);
    std::string file = progname+".root";
    
    //////////////////////////////////////////////////////////
    /// args : ///////////////////////////////////////////////
    //////////////////////////////////////////////////////////

    tools::args args(argc,argv);
  
    bool verbose = args.is_arg("-verbose");
    
    bool row_wise = !args.is_arg("-column_wise"); //default is row_wise.
    //bool row_wise = args.is_arg("-row_wise");
  
    //////////////////////////////////////////////////////////
    /// write file : /////////////////////////////////////////
    //////////////////////////////////////////////////////////
   {if(verbose) std::cout << "Comm size " << comm_size << std::endl;
    
    if(verbose) std::cout << "rank " << rank << " : processor name " << processor_name << std::endl;
    
    unsigned int num_megas;
    args.find<unsigned int>("-megas",num_megas,1);

    unsigned int basket_size;
    args.find<unsigned int>("-basket_size",basket_size,32000);

    if(verbose) std::cout << (row_wise?"row_wise":"column_wise") << std::endl;
    
    //////////////////////////////////////////////////////////
    /// create a .root file : ////////////////////////////////
    //////////////////////////////////////////////////////////
    tools::wroot::file rfile(std::cout,file);
    if(!rfile.is_open()) {
      std::cout << "rank " << rank << " : can't open root file " << file << std::endl;
      return EXIT_FAILURE;
    }

    //////////////////////////////////////////////////////////
    /// create a ntuple from a ntuple_booking object. ////////
    //////////////////////////////////////////////////////////
    tools::uint32 main_ntuple_id = 1;    
    tools::wroot::ntuple* main_ntuple = new tools::wroot::ntuple(rfile.dir(),nbk,row_wise); //owned by the directory.
    main_ntuple->set_basket_size(basket_size);

    std::vector<tools::wroot::branch*> main_branches; //if column_wise.
    std::vector<tools::uint32> basket_sizes;          //if column_wise.
    if(row_wise) {
    } else {
      main_ntuple->get_branches(main_branches);
      tools_vforcit(tools::wroot::branch*,main_branches,it) basket_sizes.push_back((*it)->basket_size());
    }

    MPI_Comm comm = MPI_COMM_WORLD;

   {int rank_dest = 1;
    tools::mpi::wrmpi _wrmpi(std::cout,comm);
    tools::impi& _impi = _wrmpi; //in the below, we use only the tools::impi interface.
    if(!_impi.bpack(verbose)) {std::cout << "bpack(verbose) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.pack(main_ntuple_id)) {std::cout << "pack(ntuple_id) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.bpack(row_wise)) {std::cout << "bpack(row_wise) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.bpack(rfile.byte_swap())) {std::cout << "bpack(byte_swap) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.pack(rfile.compression())) {std::cout << "pack(compression) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.pack(rfile.dir().seek_directory())) {std::cout << "pack(seek) failed."<< std::endl;return EXIT_FAILURE;}
    if(row_wise) {
      if(!_impi.pack(basket_size)) {std::cout << "pack(basket_size) failed."<< std::endl;return EXIT_FAILURE;}
    } else {
      if(!_impi.vpack(basket_sizes)) {std::cout << "vpack(basket_sizes) failed."<< std::endl;return EXIT_FAILURE;}
    }
    if(!_impi.pack(num_megas)) {std::cout << "pack(basket_size) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.send_buffer(rank_dest,tag)) {
      std::cout << "send_buffer() failed." << std::endl;
      return EXIT_FAILURE;
    }}

   {int rank_dest = 2;
    tools::mpi::wrmpi _wrmpi(std::cout,comm);
    tools::impi& _impi = _wrmpi; //in the below, we use only the tools::impi interface.
    if(!_impi.bpack(verbose)) {std::cout << "bpack(verbose) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.pack(main_ntuple_id)) {std::cout << "pack(ntuple_id) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.bpack(row_wise)) {std::cout << "bpack(row_wise) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.bpack(rfile.byte_swap())) {std::cout << "bpack(byte_swap) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.pack(rfile.compression())) {std::cout << "pack(compression) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.pack(rfile.dir().seek_directory())) {std::cout << "pack(seek) failed."<< std::endl;return EXIT_FAILURE;}
    if(row_wise) {
      if(!_impi.pack(basket_size)) {std::cout << "pack(basket_size) failed."<< std::endl;return EXIT_FAILURE;}
    } else {
      if(!_impi.vpack(basket_sizes)) {std::cout << "vpack(basket_sizes) failed."<< std::endl;return EXIT_FAILURE;}
    }
    if(!_impi.pack(num_megas)) {std::cout << "pack(basket_size) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.send_buffer(rank_dest,tag)) {
      std::cout << "send_buffer() failed." << std::endl;
      return EXIT_FAILURE;
    }}

    // wait baskets from other ranks :
    unsigned int num_end_fill = 0;
    while(true) { 

      tools::mpi::wrmpi _wrmpi(std::cout,comm);
      tools::impi& _impi = _wrmpi; //in the below, we use only the tools::impi interface.
	
      int probe_src;
      if(!_impi.wait_buffer(rank,tag,probe_src,false/*verbose*/)) {
        std::cout << "impi.wait_buffer() failed." << std::endl;
        return EXIT_FAILURE;
      }
      
      tools::uint32 protocol;
      if(!_impi.unpack(protocol)) {std::cout << "sunpack(protocol) failed."<< std::endl;return EXIT_FAILURE;}

      if(protocol==tools::wroot::mpi_protocol_basket()){
        //::printf("debug : from %d, receive basket : %d\n",probe_src,buffer_size);
        tools::uint32 ntuple_id;
        if(!_impi.unpack(ntuple_id)) {std::cout << "unpack(ntuple_id) failed."<< std::endl;return EXIT_FAILURE;}

	if(ntuple_id!=main_ntuple_id) {
          std::cout << "unknown ntuple_id " << ntuple_id << "." << std::endl;
          return EXIT_FAILURE;
	}

	if(!main_ntuple->mpi_add_basket(_impi)) {
          std::cout << "main_ntuple->mpi_add_basket() failed." << std::endl;	  
          return EXIT_FAILURE;
        }
	
      } else if(protocol==tools::wroot::mpi_protocol_end_fill()) {
        tools::uint32 ntuple_id;
        if(!_impi.unpack(ntuple_id)) {std::cout << "unpack(ntuple_id) failed."<< std::endl;return EXIT_FAILURE;}
	
	if(ntuple_id!=main_ntuple_id) {
          std::cout << "unknown ntuple_id " << ntuple_id << "." << std::endl;
          return EXIT_FAILURE;
	}
	
	if(!main_ntuple->mpi_end_fill(_impi)) {
          std::cout << "main_ntuple->mpi_end_fill() failed." << std::endl;	  
          return EXIT_FAILURE;
        }

        num_end_fill++;
        if(num_end_fill==2) break;

      } else {
	std::cout << "unknown protocol " << protocol << std::endl;
        return EXIT_FAILURE;
      }
    }

    main_ntuple->merge_number_of_entries();

    if(verbose) std::cout << "entries " << main_ntuple->entries() << std::endl;

    //////////////////////////////////////////////////////////
    /// write and close file : ///////////////////////////////
    //////////////////////////////////////////////////////////
   {unsigned int n;
    if(!rfile.write(n)) {
      std::cout << "root file write failed." << std::endl;
      return EXIT_FAILURE;
    }}
    rfile.close();
    if(verbose) std::cout << "root file " << file << " closed." << std::endl;}
   
    //////////////////////////////////////////////////////////
    /// read the file : //////////////////////////////////////
    //////////////////////////////////////////////////////////

#include "read_root.icc"

    if(verbose) std::cout << "end rank " << rank << std::endl;
  }

  if((rank==1)||(rank==2))  {

    MPI_Comm comm = MPI_COMM_WORLD;

    int rank_src = 0;

    bool verbose;
    tools::uint32 main_ntuple_id;
    bool row_wise;
    bool byte_swap;
    unsigned int compression;
    tools::wroot::seek seek_directory;
    tools::uint32 basket_size;
    std::vector<tools::uint32> basket_sizes;
    unsigned int num_megas;

   {tools::mpi::wrmpi _wrmpi(std::cout,comm);    
    tools::impi& _impi = _wrmpi; //in the below, we use only the tools::impi interface.
    
    int probe_src;
    if(!_impi.wait_buffer(rank,rank_src,tag,probe_src,false/*verbose*/)) {
      std::cout << "impi.wait_buffer() failed."<< std::endl;
      return EXIT_FAILURE;
    }
    
    if(!_impi.bunpack(verbose)) {std::cout << "bunpack(verbose) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.unpack(main_ntuple_id)) {std::cout << "unpack(ntuple_id) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.bunpack(row_wise)) {std::cout << "bunpack(row_wise) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.bunpack(byte_swap)) {std::cout << "bunpack(byte_swap) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.unpack(compression)) {std::cout << "unpack(compression) failed."<< std::endl;return EXIT_FAILURE;}
    if(!_impi.unpack(seek_directory)) {std::cout << "unpack(seek) failed."<< std::endl;return EXIT_FAILURE;}
    if(row_wise) {
      if(!_impi.unpack(basket_size)) {std::cout << "unpack(basket_size) failed."<< std::endl;return EXIT_FAILURE;}
    } else {
      if(!_impi.vunpack(basket_sizes)) {std::cout << "vunpack(basket_sizes) failed."<< std::endl;return EXIT_FAILURE;}
    }
    if(!_impi.unpack(num_megas)) {std::cout << "unpack(num_megas) failed."<< std::endl;return EXIT_FAILURE;}
    }

    if(verbose) std::cout << "rank " << rank << " : processor name " << processor_name << std::endl;
    
   //std::cout << "rank = " << rank
   //           << ", byte_swap = " << byte_swap
   //           << ", compression = " << compression
   //           << ", seek_directory = " << seek_directory
   //           << ", basket_sizes.size() = " << basket_sizes.size()
   //           << std::endl;

    /////////////////////////////////////////////////////////////////////////
    /// create the per rank ntuple : ////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    tools::wroot::impi_ntuple* _ntuple = 0;
    if(row_wise) {
      _ntuple = new tools::wroot::mpi_ntuple_row_wise(main_ntuple_id,
                                                      std::cout,byte_swap,compression,seek_directory,
						      basket_size,nbk,false/*verbose*/);
    } else {
      _ntuple = new tools::wroot::mpi_ntuple_column_wise(main_ntuple_id,
                                                         std::cout,byte_swap,compression,seek_directory,
							 basket_sizes,nbk,false/*verbose*/);
    }

    /*
    _ntuple->print_columns(std::cout);
    tools::wroot::base_pntuple::column_ref<double>* col_rgauss = _ntuple->find_column_ref<double>("rgauss");
    if(!col_rgauss) {std::cout << "column rgauss not found." << std::endl;return EXIT_FAILURE;}
    tools::wroot::base_pntuple::column_ref<float>* col_rbw = _ntuple->find_column_ref<float>("rbw");
    if(!col_rbw) {std::cout << "column rbw not found." << std::endl;return EXIT_FAILURE;}
    tools::wroot::base_pntuple::column_string_ref* col_str = _ntuple->find_column_string_ref("string");
    if(!col_str) {std::cout << "column strings not found." << std::endl;return EXIT_FAILURE;}
    */
  //tools::wroot::base_pntuple::column_string* col_str = _ntuple->find_column_string("string");
  //if(!col_str) {std::cout << "column string not found." << std::endl;return EXIT_FAILURE;}
    
    tools::mpi::wrmpi _wrmpi(std::cout,comm);    
    tools::impi& _impi = _wrmpi; //in the below, we use only the tools::impi interface.

   {tools::rgaussd rg(1,2);
    tools::rbwf rbwf(0,1);
    tools::rbwd rbwd(-1,1);
    tools::rtausmed rflat;
    std::string stmp;
    unsigned int entries = num_megas*1000000;
    for(unsigned int count=0;count<entries;count++) {    
      // fill variables :
      user_rgauss = rg.shoot();
      user_rbw = rbwf.shoot();
      
      if(!tools::num2s(count,stmp)){}
      user_string = "str "+stmp;
    //if(!col_str->fill("str "+stmp)) {
    //  std::cout << "col_str fill failed." << std::endl;
    //  break;
    //}

      user_count = count;
      
     {user_vec_d.clear();
      unsigned int number = row_wise ? count%100 : (unsigned int)(10*rflat.shoot());
      for(unsigned int i=0;i<number;i++) user_vec_d.push_back(rg.shoot());}

     {user_vec_s.clear();
      unsigned int number = row_wise ? count%5 : (unsigned int)(5*rflat.shoot());
      for(unsigned int i=0;i<number;i++) {
        if(!tools::num2s(i,stmp)){}
        user_vec_s.push_back(stmp);
      }}
     
      if(!_ntuple->add_row(_impi,rank_src,tag)) {
        std::cout << "ntuple fill failed." << std::endl;
        break;
      }
    }}

    if(!_ntuple->end_fill(_impi,rank_src,tag)) {} //important.

    delete _ntuple;

    if(verbose) std::cout << "end rank " << rank << std::endl;

  }

  ::MPI_Finalize();

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM
  return EXIT_SUCCESS;
}
