// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.


#include <tools/hdf5/header>
#include <tools/hdf5/ntuple>

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>
#include <tools/histo/h1d>

#include <tools/args>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  unsigned int wentries;
  unsigned int basket_size;
  args.find<unsigned int>("-entries",wentries,9923);        //it is a prime!
  args.find<unsigned int>("-basket_size",basket_size,433);  //it is a prime.
  //args.find<unsigned int>("-entries",wentries,1299709);     //it is a prime!
  //args.find<unsigned int>("-basket_size",basket_size,2017); //it is a prime.
  
  if(verbose) std::cout << "entries : " << wentries << std::endl;  
  if(verbose) std::cout << "basket_size : " << basket_size << std::endl;
  
  unsigned int compress;
  args.find<unsigned int>("-compress",compress,0);
  
  tools::histo::h1d hgw("rgauss",100,-5,5);
  tools::histo::h1d hvdw("rgauss",100,-5,5);
  std::vector<double> wsome_entries;
  
  unsigned int wncols = 0;
  ///////////////////////////////////////////////////////////////////
  /// write : ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  if(verbose) std::cout << "write ..." << std::endl;
  
 {hid_t file = ::H5Fcreate("ntuple.hdf5",H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
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
  hid_t ntuples = tools_H5Gcreate(file,"ntuples",0);
  if(ntuples<0) {
    std::cout << "can't create group." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  if(!tools::hdf5::write_atb(ntuples,"type","directory")) {
    std::cout << "write_atb() class failed." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(verbose) std::cout << "compression : " << compress << std::endl;

 {wncols = 0;
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  double user_rgauss;
  nbk.add_column<double>("rgauss",user_rgauss);wncols++;
  nbk.add_column<float>("rbw");wncols++;
  nbk.add_column<std::string>("string");wncols++;
  
  std::vector<float> user_vec_f;
  nbk.add_column<float>("vec_float",user_vec_f);wncols++;
  std::vector<double> user_vec_d;
  nbk.add_column<double>("vec_double",user_vec_d);wncols++;
  
  tools::hdf5::ntuple ntuple(std::cout,ntuples,nbk,compress,basket_size);
  if(ntuple.columns().size()!=wncols) {
    std::cout << "mismatch column numbers :"
	      << " " << ntuple.columns().size() << ". " << wncols << " expected." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  
  /*
  tools::hdf5::ntuple::column<double>* col_rgauss = ntuple.find_column<double>("rgauss");
  if(!col_rgauss) {
    std::cout << "column rgauss not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
    }*/
  tools::hdf5::ntuple::column<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!col_rbw) {
    std::cout << "column rbw not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  tools::hdf5::ntuple::column_string* col_str = ntuple.find_column_string("string");
  if(!col_str) {
    std::cout << "column strings not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  
  tools::rgaussd vrg(5,2);
  
  tools::rgaussd rg(1,2);
  tools::rbwf rbwf(0,1);
  std::string stmp;
  unsigned int entries_3 = wentries/3;
  unsigned int entries_2_3 = 2*entries_3;
  for(unsigned int count=0;count<wentries;count++) {    
    double vd = rg.shoot();
    hgw.fill(vd);
    user_rgauss = vd;
    if(!col_rbw->fill(rbwf.shoot())) {
      std::cout << "col_rbw fill failed." << std::endl;
      break;
    }
    
    if(!tools::num2s(count,stmp)){}
    if(!col_str->fill("str "+stmp)) {
      std::cout << "col_str fill failed." << std::endl;
      break;
    }
    
   {double dnumber = vrg.shoot();
    size_t number = size_t(dnumber>=0?dnumber:0);
    user_vec_f.resize(number);
    for(size_t i=0;i<number;i++) user_vec_f[i] = rbwf.shoot();}

   {double dnumber = vrg.shoot();
    size_t number = size_t(dnumber>=0?dnumber:0);
    user_vec_d.resize(number);     
    for(size_t i=0;i<number;i++) {
      double v = rg.shoot();
      user_vec_d[i] = v;
      hvdw.fill(v);
    }}

    if(!ntuple.add_row()) {
      std::cout << "ntuple fill failed." << std::endl;
      break;
    }

    if(count<=5) {
      if(verbose) std::cout << "count " << count << " : " << vd << std::endl;
      wsome_entries.push_back(vd);
    }
    if(count>(wentries-5)) {
      if(verbose) std::cout << "count " << count << " : " << vd << std::endl;
      wsome_entries.push_back(vd);
    }
    
    if(count==entries_3) ntuple.set_basket_size(613);
    if(count==entries_2_3) ntuple.set_basket_size(827);
  }}
  
 {tools::hdf5::ntuple ntuple(std::cout,ntuples,"empty_ntuple",compress,basket_size);}

 {tools::ntuple_booking nbk("same_booking","Randoms");
  nbk.add_column<double>("rgauss");
  nbk.add_column<float>("rbw");
  tools::hdf5::ntuple ntuple(std::cout,ntuples,nbk,compress,basket_size);}
  
  ::H5Gclose(ntuples);
  ::H5Fclose(file);
  }
 
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
  if(num_open) std::cout << "write : warning : H5F num_open " << num_open << std::endl;}
 
  ///////////////////////////////////////////////////////////////////
  /// read : ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  if(verbose) std::cout << "read ..." << std::endl;
 
 {hid_t file = H5Fopen("ntuple.hdf5",H5F_ACC_RDONLY, H5P_DEFAULT);
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
  
  hid_t ntuples = tools_H5Gopen(file,"ntuples");
  if(ntuples<0) {
    std::cout << "can't open group." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
 
  tools::hdf5::ntuple ntuple(std::cout,ntuples,"rg_rbw");

  if(ntuple.columns().size()!=wncols) {
    std::cout << "mismatch column numbers :"
	      << " " << ntuple.columns().size() << ". " << wncols << " expected." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  
  tools::hdf5::ntuple::column<double>* col_rgauss = ntuple.find_column<double>("rgauss");
  if(!col_rgauss) {
    std::cout << "column rgauss not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  tools::hdf5::ntuple::column<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!col_rbw) {
    std::cout << "column rbw not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  tools::hdf5::ntuple::column_string* col_str = ntuple.find_column_string("string");
  if(!col_str) {
    std::cout << "column strings not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  tools::hdf5::ntuple::std_vector_column<float>* col_vec_float = ntuple.find_std_vector_column<float>("vec_float");
  if(!col_vec_float) {
    std::cout << "column vec_float not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  tools::hdf5::ntuple::std_vector_column<double>* col_vec_double = ntuple.find_std_vector_column<double>("vec_double");
  if(!col_vec_double) {
    std::cout << "column vec_double not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  
  tools::uint64 entries;
  if(!ntuple.entries(entries)) {
    std::cout << "entries() failed." << std::endl;
    ::H5Gclose(ntuples); 
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(entries!=wentries) {
    std::cout << "read entries != write entries." << std::endl;
    ::H5Gclose(ntuples); 
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  std::vector<double> user_vec_d;
  
  std::string stmp;
  tools::histo::h1d hgr("rgauss",100,-5,5);
  tools::histo::h1d hvdr("rgauss",100,-5,5);
  std::vector<double> rsome_entries;
  for(tools::uint64 row=0;row<entries;row++) {
    if(!ntuple.get_row()) {
      std::cout << "ntuple.get_row() failed." << std::endl;
      ::H5Gclose(ntuples); 
      ::H5Fclose(file);
      return EXIT_FAILURE;
    }
    
    double vd;
    col_rgauss->get_entry(vd);
    hgr.fill(vd);
    float vf;
    col_rbw->get_entry(vf);
    
    std::string vs;
    col_str->get_entry(vs);
    stmp = "str ";
    if(!tools::numas(row,stmp)){}
    if(vs!=stmp) {
      std::cout << "for row " << row << ", string mismatch : "
		<< tools::sout(vs) << ". Expected " << tools::sout(stmp) << "." << std::endl;
      ::H5Gclose(ntuples); 
      ::H5Fclose(file);
      return EXIT_FAILURE;
    }
    
  //col_vec_double->get_entry(user_vec_d);
 //{tools_vforcit(double,user_vec_d,it) hvdr.fill(*it);}
    const std::vector<double>& uvd = col_vec_double->data();
    {tools_vforcit(double,uvd,it) hvdr.fill(*it);}

    if(row<=5) {
      if(verbose) std::cout << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
    if(row>(entries-5)) {
      if(verbose) std::cout << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
  }
      
  double prec = 1e-8;
  
  if(!hgw.equals(hgr,prec,::fabs)) {
    std::cout << "read histo != write histo." << std::endl;
    hgw.hprint(std::cout);
    hgr.hprint(std::cout);
  }
  if(!tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs)) {
    std::cout << "read some entries != write some entries." << std::endl;
  }
  
  if(!hvdw.equals(hvdr,prec,::fabs)) {
    std::cout << "read histo != write histo." << std::endl;
    hvdw.hprint(std::cout);
    hvdr.hprint(std::cout);
  }
  
  if(!tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs)) {
    std::cout << "read some entries != write some entries." << std::endl;
  }

  // read again :
  if(verbose) std::cout << "read again ..." << std::endl;
 {ntuple.reset();
  ntuple.set_basket_size(983);
  //tools::histo::h1d hgrr("rgauss",100,-5,5);
  std::vector<double> rrsome_entries;
  tools::uint64 entries_3 = tools::uint64(entries/3);
  tools::uint64 entries_2_3 = 2*entries_3;
  for(tools::uint64 row=0;row<entries;row++) {
    if(!ntuple.get_row()) {
      std::cout << "ntuple.get_row() failed." << std::endl;
      ::H5Gclose(ntuples); 
      ::H5Fclose(file);
      return EXIT_FAILURE;
    }
    double vd;
    col_rgauss->get_entry(vd);
    //hgrr.fill(vd);
    float vf;
    col_rbw->get_entry(vf);
    if(row<=5) {
      if(verbose) std::cout << "count " << row << " : " << vd << std::endl;
      rrsome_entries.push_back(vd);
    }
    if(row>(entries-5)) {
      if(verbose) std::cout << "count " << row << " : " << vd << std::endl;
      rrsome_entries.push_back(vd);
    }
    if(row==entries_3) ntuple.set_basket_size(613);
    if(row==entries_2_3) ntuple.set_basket_size(827);
  }}
      
  ::H5Gclose(ntuples); 
  ::H5Fclose(file);

  }

  ///////////////////////////////////////////////////////////////////
  /// read (binding) : //////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  if(verbose) std::cout << "read with user variables binding..." << std::endl;
 
 {hid_t file = H5Fopen("ntuple.hdf5",H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file<0) {
    std::cout << "can't open file." << std::endl;
    return EXIT_FAILURE;
  }

  hid_t ntuples = tools_H5Gopen(file,"ntuples");
  if(ntuples<0) {
    std::cout << "can't open group." << std::endl;
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }


  tools::ntuple_binding bd;
  double user_rgauss;
  bd.add_column("rgauss",user_rgauss);
  float user_rbw;
  bd.add_column("rbw",user_rbw);
  std::string user_string;
  bd.add_column("string",user_string);
  std::vector<float> user_vec_float;
  bd.add_column("vec_float",user_vec_float);
  std::vector<double> user_vec_double;
  bd.add_column("vec_double",user_vec_double);
  
//tools::hdf5::ntuple ntuple(std::cout,ntuples,"rg_rbw",bd);
  tools::hdf5::ntuple ntuple(std::cout,ntuples,"rg_rbw");
  if(!ntuple.initialize(std::cout,bd)) {
    std::cout << "ntuple.initialize(binding) failed." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  
  if(ntuple.columns().size()!=wncols) {
    std::cout << "mismatch column numbers :"
	      << " " << ntuple.columns().size() << ". " << wncols << " expected." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }
  
  if(ntuple.columns().empty()) {
    std::cout << "can't initialize ntuple for read with binding." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  tools::uint64 entries;
  if(!ntuple.entries(entries)) {
    std::cout << "entries() failed." << std::endl;
    ::H5Gclose(ntuples); 
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  if(entries!=wentries) {
    std::cout << "read entries != write entries." << std::endl;
    ::H5Gclose(ntuples); 
    ::H5Fclose(file);
    return EXIT_FAILURE;
  }

  std::vector<double> user_vec_d;
  
  std::string stmp;
  tools::histo::h1d hgr("rgauss",100,-5,5);
  tools::histo::h1d hvdr("rgauss",100,-5,5);
  std::vector<double> rsome_entries;
  for(tools::uint64 row=0;row<entries;row++) {
    if(!ntuple.get_row()) {
      std::cout << "ntuple.get_row() failed." << std::endl;
      ::H5Gclose(ntuples); 
      ::H5Fclose(file);
      return EXIT_FAILURE;
    }
    
    hgr.fill(user_rgauss);
    
    stmp = "str ";
    if(!tools::numas(row,stmp)){}
    if(user_string!=stmp) {
      std::cout << "for row " << row << ", string mismatch : "
		<< tools::sout(user_string) << ". Expected " << tools::sout(stmp) << "." << std::endl;
      ::H5Gclose(ntuples); 
      ::H5Fclose(file);
      return EXIT_FAILURE;
    }
    
  //col_vec_double->get_entry(user_vec_d);
 //{tools_vforcit(double,user_vec_d,it) hvdr.fill(*it);}
    {tools_vforcit(double,user_vec_double,it) hvdr.fill(*it);}

    if(row<=5) {
      if(verbose) std::cout << "count " << row << " : " << user_rgauss << std::endl;
      rsome_entries.push_back(user_rgauss);
    }
    if(row>(entries-5)) {
      if(verbose) std::cout << "count " << row << " : " << user_rgauss << std::endl;
      rsome_entries.push_back(user_rgauss);
    }
  }
      
  double prec = 1e-8;
  
  if(!hgw.equals(hgr,prec,::fabs)) {
    std::cout << "read histo != write histo." << std::endl;
    hgw.hprint(std::cout);
    hgr.hprint(std::cout);
  }
  if(!tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs)) {
    std::cout << "read some entries != write some entries." << std::endl;
  }
  
  if(!hvdw.equals(hvdr,prec,::fabs)) {
    std::cout << "read histo != write histo." << std::endl;
    hvdw.hprint(std::cout);
    hvdr.hprint(std::cout);
  }
  
  if(!tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs)) {
    std::cout << "read some entries != write some entries." << std::endl;
  }

  ::H5Gclose(ntuples); 
  ::H5Fclose(file);}
 
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
  if(num_open) std::cout << "read : warning : H5F num_open " << num_open << std::endl;}

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
  std::cout << "exit ..." << std::endl;
#endif

  return EXIT_SUCCESS;
}
