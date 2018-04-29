// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/hdf5/header>
#include <tools/hdf5/h2file>

#include <tools/histo/h1d>
#include <tools/histo/h2d>
#include <tools/histo/p1d>
#include <tools/histo/p2d>

#include <iostream>
#include <tools/mem>
#include <tools/random>

#include <tools/args>
#include <cstdlib>

int main(int argc,char** argv) {

#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif

  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  //////////////////////////////////////////////////////////
  /// create and fill histograms : //////////////////////////
  //////////////////////////////////////////////////////////
  tools::histo::h1d h1("Random Gauss",100,-5.0,5.0);

 {tools::random::gauss rg(0,1);
  for(unsigned int index=0;index<10000;index++) h1.fill(rg.shoot(),1);}

  tools::random::gauss rg(1,2);
  tools::random::bw rbw(0,1);
  
  tools::histo::h2d h2("Gauss_BW",20,-5,5,20,-2,2);
 {for(unsigned int count=0;count<30000;count++) h2.fill(rg.shoot(),rbw.shoot(),0.8);}

  tools::histo::p1d p1("Profile",100,-5,5,-2,2);
 {for(unsigned int count=0;count<10000;count++) p1.fill(rg.shoot(),rbw.shoot(),1);}
  
  tools::histo::p2d p2("Profile2D",100,-5,5,100,-5,5,-2,2);
 {for(unsigned int count=0;count<40000;count++) p2.fill(rg.shoot(),rg.shoot(),rbw.shoot(),1);}

  ///////////////////////////////////////////////////////////////////
  /// write : ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  if(verbose) std::cout << "write ..." << std::endl;
  
 {hid_t file = ::H5Fcreate("histos.hdf5",H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if(file<0) {
    std::cout << "can't create file." << std::endl;
    return EXIT_FAILURE;
  }

  // create a header with general infos :
  if(!tools::hdf5::write_header(file)) {
    std::cout << "write_header() failed." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  // create a directory :
  hid_t histos = tools_H5Gcreate(file,"histos",0);
  if(histos<0) {
    std::cout << "can't create group." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  if(!tools::hdf5::write_atb(histos,"type","directory")) {
    std::cout << "write_atb() class failed." << std::endl;
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(!tools::hdf5::write_histo(std::cout,histos,"h1",h1)) {
    std::cout << "tools::hdf5::write(h1) failed." << std::endl;
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(!tools::hdf5::write_histo(std::cout,histos,"h2",h2)) {
    std::cout << "tools::hdf5::write(h2) failed." << std::endl;
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(!tools::hdf5::write_profile(std::cout,histos,"p1",p1)) {
    std::cout << "tools::hdf5::write(p1) failed." << std::endl;
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(!tools::hdf5::write_profile(std::cout,histos,"p2",p2)) {
    std::cout << "tools::hdf5::write(p2) failed." << std::endl;
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  ::H5Gclose(histos);
  ::H5Fclose(file);}

  ///////////////////////////////////////////////////////////////////
  /// read : ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  if(verbose) std::cout << "read ..." << std::endl;
  
 {hid_t file = H5Fopen("histos.hdf5",H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file<0) {
    std::cout << "can't open file." << std::endl;
    return EXIT_FAILURE;
  }

 {std::string swriter;
  int data_schema_version;
  if(!tools::hdf5::read_header(file,swriter,data_schema_version)) {
    std::cout << "read_header() failed." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  if(verbose) std::cout << "read : writer " << swriter << std::endl;
  if(verbose) std::cout << "read : data_schema_version " << data_schema_version << std::endl;}
  
  hid_t histos = tools_H5Gopen(file,"histos");
  if(histos<0) {
    std::cout << "can't open group." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
 
 {tools::histo::h1d* rh1;
  if(!tools::hdf5::read_histo(std::cout,histos,"h1",rh1)) {
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  if(!rh1->equals(h1,prec,::fabs)) {
    std::cout << "read h1d != write histo." << std::endl;
  }
  delete rh1;}
 
 {tools::histo::h2d* rh2;
  if(!tools::hdf5::read_histo(std::cout,histos,"h2",rh2)) {
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  if(!rh2->equals(h2,prec,::fabs)) {
    std::cout << "read h2d != write histo." << std::endl;
  }
  delete rh2;}

 {tools::histo::p1d* rp1;
  if(!tools::hdf5::read_profile(std::cout,histos,"p1",rp1)) {
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  if(!rp1->equals(p1,prec,::fabs)) {
    std::cout << "read p1d != write histo." << std::endl;
  }
  delete rp1;}
 
 {tools::histo::p2d* rp2;
  if(!tools::hdf5::read_profile(std::cout,histos,"p2",rp2)) {
    ::H5Gclose(histos);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  if(!rp2->equals(p2,prec,::fabs)) {
    std::cout << "read p2d != write histo." << std::endl;
  }
  delete rp2;}
 
  ::H5Gclose(histos); 
  ::H5Fclose(file);}
  
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
  if(num_open) std::cout << "warning : after read num_open " << num_open << std::endl;}

 if(verbose) std::cout << "exit ..." << std::endl;
  
#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif

  return EXIT_SUCCESS;
}
