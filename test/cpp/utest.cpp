// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <ostream>

bool test_histo(std::ostream&,bool);
bool test_root_histo(std::ostream&,bool);
bool test_root_ntuple(std::ostream&,bool);
bool test_csv_histo(std::ostream&,bool);
bool test_csv_ntuple(std::ostream&,bool);
bool test_columns(std::ostream&,bool);
bool test_viewplot(std::ostream&,bool);

////////////////////////////////////////////
/// tests needing an external : ////////////
////////////////////////////////////////////
#ifdef TOOLS_USE_EXPAT
bool test_axml(std::ostream&,bool);
#endif

#ifdef TOOLS_USE_HDF5
bool test_hdf5(std::ostream&,bool);
bool test_hdf5_histo(std::ostream&,bool);
bool test_hdf5_ntuple(std::ostream&,bool);
#endif

#ifdef TOOLS_USE_FREETYPE
bool test_lego_offscreen_zb(std::ostream&,bool);
#ifdef TOOLS_USE_EXPAT
bool test_viewplot_ttf(std::ostream&,bool);
#endif
#endif

////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/args>

#include <tools/utest>

#include <cstdlib>
#include <iostream>
#include <map>

int main(int argc,char* argv[]) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM

  tools::args args(argc,argv);

  bool verbose = false;
  if(args.is_arg("-verbose")) verbose = true;
  args.remove("-verbose");

#ifdef TOOLS_MEM
#else
  bool verb_exit = false;
  if(args.is_arg("-verb_exit")) verb_exit= true;
#endif
  args.remove("-verb_exit");
  
  unsigned int prec;
  if(args.find("-prec",prec)) std::cout.precision(prec);
  args.remove("-prec");

  ///////////////////////////////////////////////
  ///////////////////////////////////////////////
  ///////////////////////////////////////////////
  tools::utest utest(std::cout);

  utest.add("histo",test_histo);
  utest.add("root_histo",test_root_histo);
  utest.add("root_ntuple",test_root_ntuple);
  utest.add("csv_histo",test_csv_histo);
  utest.add("csv_ntuple",test_csv_ntuple);
  utest.add("columns",test_columns);
  utest.add("viewplot",test_viewplot);

#ifdef TOOLS_USE_EXPAT
  utest.add("axml",test_axml);
#endif
  
#ifdef TOOLS_USE_HDF5
  utest.add("hdf5",test_hdf5);
  utest.add("hdf5_histo",test_hdf5_histo);
  utest.add("hdf5_ntuple",test_hdf5_ntuple);
#endif

#ifdef TOOLS_USE_FREETYPE
  utest.add("lego_offscreen_zb",test_lego_offscreen_zb);
#ifdef TOOLS_USE_EXPAT
  utest.add("viewplot_ttf",test_viewplot_ttf);
#endif
#endif
  
  if(args.is_arg("-list")) {
    utest.list();
  } else {  
    args.remove_first();
    if(!utest.exec(args,verbose,argc,argv)) {
      std::cout << "tools::utest : failed : exit ..." << std::endl;
      return EXIT_FAILURE;
    }
  }

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
  std::cout << "tools::utest : exit(mem) ..." << std::endl;
#else  
  if(verbose||verb_exit) std::cout << "tools::utest : exit ..." << std::endl;
#endif //TOOLS_MEM

  return EXIT_SUCCESS;
}
