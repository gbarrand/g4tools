// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

// Write/read inlib histos in a hdf5 file.


#include <tools/hdf5/ntuple>

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>
#include <tools/histo/h1d>

#include <tools/test>
#include "../sys/dir"
#include "../sys/process"
#include <tools/file>
#include <tools/num2s>
#include <tools/strip>

bool test_hdf5_ntuple(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string file;
  if(!tools::num2s(tools::process_id(),file)) {}
  file = "utest_"+file+".hdf5";
  if(a_verbose) a_out << "test_ntuple_hdf5 : file : " << file << std::endl;

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  unsigned int wentries = 9923;
  unsigned int basket_size = 433;
  //unsigned int wentries = 1299709;
  //unsigned int basket_size = 2017;
  if(a_verbose) a_out << "entries : " << wentries << std::endl;  
  if(a_verbose) a_out << "basket_size : " << basket_size << std::endl;
  
  unsigned int compress = 1;
  
  tools::histo::h1d hgw("rgauss",100,-5,5);
  tools::histo::h1d hvdw("rgauss",100,-5,5);
  std::vector<double> wsome_entries;
  
  unsigned int wncols = 0;
  
  ///////////////////////////////////////////////////////////////////////
  /// write : ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  if(a_verbose) a_out << "test_ntuple_hdf5 : write ..." << std::endl;  

 {hid_t _file = ::H5Fcreate(file.c_str(),H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,_file,0)) return false;

  // create a directory :
  hid_t ntuples = tools_H5Gcreate(_file,"ntuples",0);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,ntuples,0)) {
    ::H5Fclose(_file);
    return false;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,tools::hdf5::write_atb(ntuples,"type","directory"),true)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }

  if(a_verbose) a_out << "compression : " << compress << std::endl;

 {wncols = 0;
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  nbk.add_column<double>("rgauss");wncols++;
  nbk.add_column<float>("rbw");wncols++;
  nbk.add_column<std::string>("string");wncols++;
  
  std::vector<float> user_vec_f;
  nbk.add_column<float>("vec_float",user_vec_f);wncols++;
  std::vector<double> user_vec_d;
  nbk.add_column<double>("vec_double",user_vec_d);wncols++;
  
  tools::hdf5::ntuple ntuple(a_out,ntuples,nbk,compress,basket_size);
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,ntuple.columns().size(),wncols)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  
  tools::hdf5::ntuple::column<double>* col_rgauss = ntuple.find_column<double>("rgauss");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rgauss)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  tools::hdf5::ntuple::column<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rbw)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  tools::hdf5::ntuple::column_string* col_str = ntuple.find_column_string("string");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_str)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
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
    if(!tools::equal(a_out,__FILE__,__LINE__,col_rgauss->fill(vd),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    if(!tools::equal(a_out,__FILE__,__LINE__,col_rbw->fill(rbwf.shoot()),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    if(!tools::num2s(count,stmp)){}
    if(!tools::equal(a_out,__FILE__,__LINE__,col_str->fill("str "+stmp),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
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
   
    if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.add_row(),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }

    if(count<=5) {
      if(a_verbose) a_out << "count " << count << " : " << vd << std::endl;
      wsome_entries.push_back(vd);
    }
    if(count>(wentries-5)) {
      if(a_verbose) a_out << "count " << count << " : " << vd << std::endl;
      wsome_entries.push_back(vd);
    }
    
    if(count==entries_3) ntuple.set_basket_size(613);
    if(count==entries_2_3) ntuple.set_basket_size(827);
  }}
  
 {tools::hdf5::ntuple ntuple(a_out,ntuples,"empty_ntuple",compress,basket_size);}

 {tools::ntuple_booking nbk("same_booking","Randoms");
  nbk.add_column<double>("rgauss");
  nbk.add_column<float>("rbw");
  tools::hdf5::ntuple ntuple(a_out,ntuples,nbk,compress,basket_size);}
  
  ::H5Gclose(ntuples);
  ::H5Fclose(_file);
  }
 
  ///////////////////////////////////////////////////////////////////////
  /// read : ////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////

  if(a_verbose) a_out << "test_ntuple_hdf5 : read ..." << std::endl;
  
 {hid_t _file = H5Fopen(file.c_str(),H5F_ACC_RDONLY, H5P_DEFAULT);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,_file,0)) return false;

  hid_t ntuples = tools_H5Gopen(_file,"ntuples");
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,ntuples,0)) {
    ::H5Fclose(_file);
    return false;
  }
 
  tools::hdf5::ntuple ntuple(a_out,ntuples,"rg_rbw"); //false = read.

  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,ntuple.columns().size(),wncols)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  
  tools::hdf5::ntuple::column<double>* col_rgauss = ntuple.find_column<double>("rgauss");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rgauss)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  tools::hdf5::ntuple::column<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rbw)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  
  tools::hdf5::ntuple::column_string* col_str = ntuple.find_column_string("string");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_str)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  tools::hdf5::ntuple::std_vector_column<float>* col_vec_float =
    ntuple.find_std_vector_column<float>("vec_float");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_vec_float)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  tools::hdf5::ntuple::std_vector_column<double>* col_vec_double =
    ntuple.find_std_vector_column<double>("vec_double");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_vec_double)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  
  tools::uint64 entries;
  if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.entries(entries),true)) {
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }

  if(!tools::equal<tools::uint64>(a_out,__FILE__,__LINE__,entries,tools::uint64(wentries))) {
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }

  //std::vector<double> user_vec_d;
  
 {std::string stmp;
  tools::histo::h1d hgr("rgauss",100,-5,5);
  tools::histo::h1d hvdr("rgauss",100,-5,5);
  std::vector<double> rsome_entries;
  for(tools::uint64 row=0;row<entries;row++) {
    if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.get_row(),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    
    double vd;
    if(!tools::equal(a_out,__FILE__,__LINE__,col_rgauss->get_entry(vd),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
      
    hgr.fill(vd);
    float vf;
    if(!tools::equal(a_out,__FILE__,__LINE__,col_rbw->get_entry(vf),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    
    std::string vs;
    if(!tools::equal(a_out,__FILE__,__LINE__,col_str->get_entry(vs),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    stmp = "str ";
    if(!tools::numas(row,stmp)){}
    if(!tools::equal(a_out,__FILE__,__LINE__,vs,stmp)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    
  //col_vec_double->get_entry(user_vec_d);
 //{tools_vforcit(double,user_vec_d,it) hvdr.fill(*it);}
    const std::vector<double>& uvd = col_vec_double->data();
    {tools_vforcit(double,uvd,it) hvdr.fill(*it);}

    if(row<=5) {
      if(a_verbose) a_out << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
    if(row>(entries-5)) {
      if(a_verbose) a_out << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
  }
      
  double prec = 1e-8;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,hgw.equals(hgr,prec,::fabs),true)) {
    //hgw.hprint(a_out);
    //hgr.hprint(a_out);
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }
 
  if(!tools::equal(a_out,__FILE__,__LINE__,
		   tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs),true)) {
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }
  
  if(!tools::equal(a_out,__FILE__,__LINE__,hvdw.equals(hvdr,prec,::fabs),true)) {
    //hvdw.hprint(a_out);
    //hvdr.hprint(a_out);
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }}
  
  // read again :
  if(a_verbose) a_out << "read again ..." << std::endl;
 {ntuple.reset();
  ntuple.set_basket_size(983);
  tools::histo::h1d hgr("rgauss",100,-5,5);
  std::vector<double> rsome_entries;
  tools::uint64 entries_3 = tools::uint64(entries/3);
  tools::uint64 entries_2_3 = 2*entries_3;
  for(tools::uint64 row=0;row<entries;row++) {
    if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.get_row(),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    double vd;
    col_rgauss->get_entry(vd);
    hgr.fill(vd);
    float vf;
    col_rbw->get_entry(vf);
    if(row<=5) {
      if(a_verbose) a_out << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
    if(row>(entries-5)) {
      if(a_verbose) a_out << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
    if(row==entries_3) ntuple.set_basket_size(613);
    if(row==entries_2_3) ntuple.set_basket_size(827);
  }}

  ::H5Gclose(ntuples); 
  ::H5Fclose(_file);

  }

  ///////////////////////////////////////////////////////////////////////
  /// read by using variable binding : //////////////////////////////////
  ///////////////////////////////////////////////////////////////////////

  if(a_verbose) a_out << "test_ntuple_hdf5 : read (binding) ..." << std::endl;
  
 {hid_t _file = H5Fopen(file.c_str(),H5F_ACC_RDONLY, H5P_DEFAULT);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,_file,0)) return false;

  hid_t ntuples = tools_H5Gopen(_file,"ntuples");
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,ntuples,0)) {
    ::H5Fclose(_file);
    return false;
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
  
//tools::hdf5::ntuple ntuple(a_out,ntuples,"rg_rbw",bd);
  tools::hdf5::ntuple ntuple(a_out,ntuples,"rg_rbw");
  if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.initialize(a_out,bd),true)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }

  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,ntuple.columns().size(),wncols)) {
    ::H5Gclose(ntuples);
    ::H5Fclose(_file);
    return false;
  }
  
  tools::uint64 entries;
  if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.entries(entries),true)) {
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }

  if(!tools::equal<tools::uint64>(a_out,__FILE__,__LINE__,entries,tools::uint64(wentries))) {
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }

  //std::vector<double> user_vec_d;
  
 {std::string stmp;
  tools::histo::h1d hgr("rgauss",100,-5,5);
  tools::histo::h1d hvdr("rgauss",100,-5,5);
  std::vector<double> rsome_entries;
  for(tools::uint64 row=0;row<entries;row++) {
    if(!tools::equal(a_out,__FILE__,__LINE__,ntuple.get_row(),true)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    
    hgr.fill(user_rgauss);
    
    stmp = "str ";
    if(!tools::numas(row,stmp)){}
    if(!tools::equal(a_out,__FILE__,__LINE__,user_string,stmp)) {
      ::H5Gclose(ntuples); 
      ::H5Fclose(_file);
      return false;
    }
    
  //col_vec_double->get_entry(user_vec_d);
 //{tools_vforcit(double,user_vec_d,it) hvdr.fill(*it);}
   {tools_vforcit(double,user_vec_double,it) hvdr.fill(*it);}

    if(row<=5) {
      if(a_verbose) a_out << "count " << row << " : " << user_rgauss << std::endl;
      rsome_entries.push_back(user_rgauss);
    }
    if(row>(entries-5)) {
      if(a_verbose) a_out << "count " << row << " : " << user_rgauss << std::endl;
      rsome_entries.push_back(user_rgauss);
    }
  }
      
  double prec = 1e-8;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,hgw.equals(hgr,prec,::fabs),true)) {
    //hgw.hprint(a_out);
    //hgr.hprint(a_out);
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }
 
  if(!tools::equal(a_out,__FILE__,__LINE__,
		   tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs),true)) {
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }
  
  if(!tools::equal(a_out,__FILE__,__LINE__,hvdw.equals(hvdr,prec,::fabs),true)) {
    //hvdw.hprint(a_out);
    //hvdr.hprint(a_out);
    ::H5Gclose(ntuples); 
    ::H5Fclose(_file);
    return false;
  }}
  
  ::H5Gclose(ntuples); 
  ::H5Fclose(_file);

  }
 
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
 
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
  if(!tools::equal<ssize_t>(a_out,__FILE__,__LINE__,num_open,0)) return false;}
 
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  TOOLS_TEST_FUNC(tools::file::std_remove(file))
    
  return true;
}
