// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//  This program produces a wroot.root file.
//
//  See rroot.C for an example of how to manipulate
// (and check !) the content of this file with CERN-ROOT.

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/wroot/file>
#include <tools/wroot/to>

#include <tools/histo/h1d>
#include <tools/histo/h2d>

#include <tools/histo/h1df>

#include <tools/randd>

//#define TOOLS_DONT_HAVE_ZLIB
#ifdef TOOLS_DONT_HAVE_ZLIB
#else
#include <toolx/zlib>
#endif

#include <tools/args>
#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {

#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM
  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  //////////////////////////////////////////////////////////
  /// create a .root file : ////////////////////////////////
  //////////////////////////////////////////////////////////

  std::string file = "wroot_cycle.root";

 {tools::wroot::file rfile(std::cout,file);
#ifdef TOOLS_DONT_HAVE_ZLIB
#else
  if(args.is_arg("-noz")){
  } else {
    rfile.add_ziper('Z',toolx::compress_buffer);
    rfile.set_compression(9);
  }
#endif

  tools::wroot::directory* dir = &rfile.dir();

  //////////////////////////////////////////////////////////
  /// create some histos : /////////////////////////////////
  //////////////////////////////////////////////////////////

  unsigned int num_megas;
  args.find<unsigned int>("-megas",num_megas,1);

  unsigned int entries = num_megas*10000;

  tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);

  tools::histo::h1d h("Gauss",100,-5,5);
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"1.4*entries");
  tools::histo::h2d h2("Gauss_BW",20,-5,5,20,-2,2);
  h2.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h2.add_annotation(tools::histo::key_axis_y_title(),"rand bw");
  h2.add_annotation(tools::histo::key_axis_z_title(),"0.8*entries");

  for (int loop = 1; loop<4; ++loop) {
    for(unsigned int count=0;count<entries*loop;count++) h.fill(rg.shoot(),1.4);
    if(verbose) {
      std::cout << "h1d : " << h.title()
                << ", all entries " << h.all_entries()
                << ", entries " << h.entries()
                << ", mean " << h.mean() << ", rms " << h.rms()
                << std::endl;
    }
    // write :
    if(!tools::wroot::to(*dir,h,"rg")) return EXIT_FAILURE;

    for(unsigned int count=0;count<entries*loop;count++) h2.fill(rg.shoot(),rbw.shoot(),0.8);
    if(verbose) {
      std::cout << "h2d : " << h2.title()
                << ", all entries " << h2.all_entries()
                << ", entries " << h2.entries()
                << ", mean_x " << h2.mean_x() << ", rms_x " << h2.rms_x()
                << ", mean_y " << h2.mean_y() << ", rms_y " << h2.rms_y()
                << std::endl;
    }
    // write :
    if(!tools::wroot::to(*dir,h2,"rgbw")) return EXIT_FAILURE;

    h.reset();
    h2.reset();
  }

  //////////////////////////////////////////////////////////
  /// write and close file : ///////////////////////////////
  //////////////////////////////////////////////////////////
 {unsigned int n;
  if(!rfile.write(n)) {
    std::cout << "file write failed." << std::endl;
  }}

 {std::cout << "keys:" << std::endl;
  tools_lforcit(tools::wroot::key*,dir->keys(),it) {
    std::cout << "----------------------------" << std::endl;
    std::cout << (*it)->object_name() << std::endl;
    std::cout << (*it)->object_title() << std::endl;
    std::cout << (*it)->object_class() << std::endl;
    std::cout << (*it)->object_size() << std::endl;
    std::cout << (*it)->cycle() << std::endl;
  }}

  rfile.close();}

  //////////////////////////////////////////////////////////
  /// read the file : //////////////////////////////////////
  //////////////////////////////////////////////////////////

// #include "read_root.icc"

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM

  return EXIT_SUCCESS;
}
