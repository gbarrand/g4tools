// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#ifndef TOOLS_DONT_HAVE_ZLIB
#define TOOLS_DONT_HAVE_ZLIB
#endif //TOOLS_DONT_HAVE_ZLIB

#include <tools/wroot/file>
#include <tools/wroot/ntuple>
#include <tools/rroot/file>
#include <tools/rroot/fac>
#include <tools/rroot/ntuple>

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

bool test_root_ntuple(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string file;
  if(!tools::num2s(tools::process_id(),file)) {}
  file = "utest_"+file+".root";
  if(a_verbose) a_out << "test_ntuple_root : file : " << file << std::endl;

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  unsigned int wentries = 9923;
  //unsigned int wentries = 992300;
  //unsigned int basket_size = 433;
  //unsigned int wentries = 1299709;
  //unsigned int basket_size = 2017;
  if(a_verbose) a_out << "entries : " << wentries << std::endl;  
  //if(a_verbose) a_out << "basket_size : " << basket_size << std::endl;
  
#ifdef TOOLS_DONT_HAVE_ZLIB
#else
  unsigned int compress = 1;
#endif
  
  tools::histo::h1d hgw("rgauss",100,-5,5);
  tools::histo::h1d hvdw("rgauss",100,-5,5);
  std::vector<double> wsome_entries;
  
  unsigned int wncols = 0;
  
  ///////////////////////////////////////////////////////////////////////
  /// write : ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  if(a_verbose) a_out << "test_ntuple_root : write ..." << std::endl;  

 {tools::wroot::file rfile(a_out,file);
#ifdef TOOLS_DONT_HAVE_ZLIB
#else
  if(args.is_arg("-noz")){
  } else {
    if(a_verbose) a_out << "compression : " << compress << std::endl;
    rfile.add_ziper('Z',exlib::compress_buffer);
    rfile.set_compression(compress);
  }
#endif

  wncols = 0;
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  double user_rgauss;
  nbk.add_column("rgauss",user_rgauss);wncols++;
  nbk.add_column<float>("rbw");wncols++;
  std::string user_string;
  nbk.add_column<std::string>("string",user_string);wncols++;
  
  std::vector<float> user_vec_f;
  nbk.add_column<float>("vec_float",user_vec_f);wncols++;
  std::vector<double> user_vec_d;
  nbk.add_column<double>("vec_double",user_vec_d);wncols++;
  
  //WARNING : the ntuple can't be on the stack. It is owned
  //          by the directory.

  tools::wroot::ntuple* ntu = new tools::wroot::ntuple(rfile.dir(),nbk);
  tools::wroot::ntuple& ntuple = *ntu;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,ntuple.columns().size(),wncols)) return false;
  
  tools::wroot::ntuple::column<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rbw)) return false;
  //tools::wroot::ntuple::column_string* col_str = ntuple.find_column_string("string");
  //if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_str)) return false;
  
  tools::rgaussd vrg(5,2);
  
  tools::rgaussd rg(1,2);
  tools::rbwf rbwf(0,1);
  std::string stmp;
  //unsigned int entries_3 = wentries/3;
  //unsigned int entries_2_3 = 2*entries_3;
  for(unsigned int count=0;count<wentries;count++) {    
    double vd = rg.shoot();
    hgw.fill(vd);
    user_rgauss = vd;
    TOOLS_TEST_FUNC(col_rbw->fill(rbwf.shoot()))
    if(!tools::num2s(count,stmp)){}
    user_string = "str "+stmp;
    //TOOLS_TEST_FUNC(col_str->fill("str "+stmp))
    
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
   
    TOOLS_TEST_FUNC(ntuple.add_row())

    if(count<=5) {
      if(a_verbose) a_out << "count " << count << " : " << vd << std::endl;
      wsome_entries.push_back(vd);
    }
    if(count>(wentries-5)) {
      if(a_verbose) a_out << "count " << count << " : " << vd << std::endl;
      wsome_entries.push_back(vd);
    }
    
    //if(count==entries_3) ntuple.set_basket_size(613);
    //if(count==entries_2_3) ntuple.set_basket_size(827);
  }
  
 {unsigned int n;
  TOOLS_TEST_FUNC(rfile.write(n))}
 
  rfile.close();
  }
 
  ///////////////////////////////////////////////////////////////////////
  /// read : ////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////

  if(a_verbose) a_out << "test_ntuple_root : read ..." << std::endl;

  bool rverbose = false;
 {tools::rroot::file rfile(a_out,file,rverbose);
#ifdef TOOLS_DONT_HAVE_ZLIB
#else
  rfile.add_unziper('Z',tools::decompress_buffer);
#endif

  tools::rroot::key* key = rfile.dir().find_key("rg_rbw");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,key)) return false;
  
  unsigned int sz;
  char* buf = key->get_object_buffer(rfile,sz);
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,buf)) return false;
  tools::rroot::buffer b(a_out,rfile.byte_swap(),sz,buf,key->key_length(),rverbose);
  tools::rroot::fac fac(a_out);
  tools::rroot::tree tree(rfile,fac);
  TOOLS_TEST_FUNC(tree.stream(b))
  //tree.dump(a_out,"","  ");
  tools::uint64 entries = tree.entries();

  // read with the flat ntuple API :
  tools::rroot::ntuple ntuple(tree); //use the flat ntuple API.
  tools::ntuple_binding nbd;
  double v_rgauss;
  nbd.add_column("rgauss",v_rgauss);
  float v_rbw;
  nbd.add_column("rbw",v_rbw);
  std::string v_string;
  nbd.add_column("string",v_string);
  std::vector<float> user_vec_f;
  nbd.add_column("vec_float",user_vec_f);
  std::vector<double> user_vec_d;
  nbd.add_column("vec_double",user_vec_d);
  TOOLS_TEST_FUNC(ntuple.initialize(a_out,nbd))
    
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,ntuple.columns().size(),wncols)) return false;
  
  tools::read::icolumn<double>* col_rgauss = ntuple.find_column<double>("rgauss");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rgauss)) return false;
  tools::read::icolumn<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!tools::valid_pointer(a_out,__FILE__,__LINE__,col_rbw)) return false;
  
  if(!tools::equal<tools::uint64>(a_out,__FILE__,__LINE__,entries,tools::uint64(wentries))) return false;

  std::string stmp;
  tools::histo::h1d hgr("rgauss",100,-5,5);
  tools::histo::h1d hvdr("rgauss",100,-5,5);
  std::vector<double> rsome_entries;
  ntuple.start();
  tools::uint64 row = 0;
  while(ntuple.next()){
    TOOLS_TEST_FUNC(ntuple.get_row())

    double vd = v_rgauss;
    
    hgr.fill(v_rgauss);
    
    stmp = "str ";
    if(!tools::numas(row,stmp)){}
    if(!tools::equal(a_out,__FILE__,__LINE__,v_string,stmp)) return false;
    
   {tools_vforcit(double,user_vec_d,it) hvdr.fill(*it);}
    
    if(row<=5) {
      if(a_verbose) a_out << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }
    if(row>(entries-5)) {
      if(a_verbose) a_out << "count " << row << " : " << vd << std::endl;
      rsome_entries.push_back(vd);
    }

    row++;
  }
      
  double prec = 1e-8;
  
  //hgw.hprint(a_out);
  //hgr.hprint(a_out);
  
  if(!tools::equal(a_out,__FILE__,__LINE__,hgw.equals(hgr,prec,::fabs),true)) return false;
  if(!tools::equal(a_out,__FILE__,__LINE__,
		   tools::vectors_are_equal(wsome_entries,rsome_entries,prec,::fabs),true)) return false;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,hvdw.equals(hvdr,prec,::fabs),true)) return false;
  
  /*
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
      if(a_verbose) ::printf("count %lu : %g\n",row,vd);
      rsome_entries.push_back(vd);
    }
    if(row>(entries-5)) {
      if(a_verbose) ::printf("count %lu : %g\n",row,vd);
      rsome_entries.push_back(vd);
    }
    if(row==entries_3) ntuple.set_basket_size(613);
    if(row==entries_2_3) ntuple.set_basket_size(827);
  }}
  
  */
  }
 
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  TOOLS_TEST_FUNC(tools::file::std_remove(file))
    
  return true;
}
