// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

// Write/read inlib histos in a hdf5 file.


#include <tools/hdf5/h2file>
#include <tools/random>
#include <tools/histo/h1d>
#include <tools/histo/h2d>
#include <tools/histo/p1d>
#include <tools/histo/p2d>

#include <tools/test>
#include "../sys/dir"
#include "../sys/process"
#include <tools/file>
#include <tools/num2s>
#include <tools/strip>

bool test_hdf5_histo(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string file;
  if(!tools::num2s(tools::process_id(),file)) {}
  file = "utest_"+file+".hdf5";
  if(a_verbose) a_out << "test_histo_hdf5 : file : " << file << std::endl;

  //////////////////////////////////////////////////////////
  /// create and fill histograms : //////////////////////////
  //////////////////////////////////////////////////////////
  tools::histo::h1d h1("Random Gauss",100,-5.0,5.0);

 {tools::random::gauss rg(0,1);
  for(unsigned int index=0;index<10000;index++) h1.fill(rg.shoot(),1);}

  tools::histo::h2d h2("Gauss_BW",20,-5,5,20,-2,2);
  tools::random::gauss rg(1,2);
  tools::random::bw rbw(0,1);
 {for(unsigned int count=0;count<30000;count++) h2.fill(rg.shoot(),rbw.shoot(),0.8);}

  tools::histo::p1d p1("Profile",100,-5,5,-2,2);
 {for(unsigned int count=0;count<10000;count++) p1.fill(rg.shoot(),rbw.shoot(),1);}
  
  tools::histo::p2d p2("Profile2D",100,-5,5,100,-5,5,-2,2);
 {for(unsigned int count=0;count<40000;count++) p2.fill(rg.shoot(),rg.shoot(),rbw.shoot(),1);}

  ///////////////////////////////////////////////////////////////////////
  /// write : ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  if(a_verbose) a_out << "test_histo_hdf5 : write ..." << std::endl;  

 {hid_t _file = ::H5Fcreate(file.c_str(),H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
   if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,_file,0)) return false;
  
  // create a directory :
  hid_t histos = tools_H5Gcreate(_file,"histos",0);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,histos,0)) {
    ::H5Fclose(_file);
    return false;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::write_atb(histos,"type","directory"),true)) {
    ::H5Fclose(_file);
    return false;
  }  

  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::write_histo(a_out,histos,"h1",h1),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return false;
  }  
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::write_histo(a_out,histos,"h2",h2),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return false;
  }  
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::write_profile(a_out,histos,"p1",p1),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return false;
  }  
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::write_profile(a_out,histos,"p2",p2),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return false;
  }  

  ::H5Gclose(histos);
  ::H5Fclose(_file);}

  ///////////////////////////////////////////////////////////////////////
  /// read : ////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////

  if(a_verbose) a_out << "test_histo_hdf5 : read ..." << std::endl;

 {hid_t _file = H5Fopen(file.c_str(),H5F_ACC_RDONLY, H5P_DEFAULT);
   if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,_file,0)) return false;

  hid_t histos = tools_H5Gopen(_file,"histos");
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,histos,0)) {
    ::H5Fclose(_file);
    return false;
  }
 
 {tools::histo::h1d* rh1;
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::read_histo(a_out,histos,"h1",rh1),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  TOOLS_TEST_FUNC(rh1->equals(h1,prec,::fabs))
  delete rh1;}
 
 {tools::histo::h2d* rh2;
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::read_histo(a_out,histos,"h2",rh2),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return false;
  }  
  double prec = 1e-8;
  TOOLS_TEST_FUNC(rh2->equals(h2,prec,::fabs))
  delete rh2;}

 {tools::histo::p1d* rp1;
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::read_profile(a_out,histos,"p1",rp1),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  TOOLS_TEST_FUNC(rp1->equals(p1,prec,::fabs))
  delete rp1;}
 
 {tools::histo::p2d* rp2;
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::read_profile(a_out,histos,"p2",rp2),true)) {
    ::H5Gclose(histos);
    ::H5Fclose(_file);
    return EXIT_FAILURE;
  }
  double prec = 1e-8;
  TOOLS_TEST_FUNC(rp2->equals(p2,prec,::fabs))
  delete rp2;}
 
  ::H5Gclose(histos); 
  ::H5Fclose(_file);}
  
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
   if(!tools::equal<ssize_t>(a_out,__FILE__,__LINE__,num_open,0)) return false;}
   
  TOOLS_TEST_FUNC(tools::file::std_remove(file))
    
  return true;
}
