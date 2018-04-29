// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//  This program show how to use tools::threads.


#include <tools/hdf5/ntuple>

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include "threads"
#include <tools/num2s>

namespace app {

inline bool write_ntuple(std::ostream& a_out,size_t a_thread) {

  std::string file_name;
  file_name = "ntuple_";
  tools::numas(a_thread,file_name);
  file_name += ".hdf5";
  
  unsigned int wentries = 9923;   //it is a prime.
  unsigned int basket_size = 433; //a prime too.
  unsigned int compress = 0;

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  
  hid_t file = ::H5Fcreate(file_name.c_str(),H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if(file<0) {
    a_out << "thread " << a_thread << " : can't create file " << file_name << std::endl;
    return false;
  }

  // create a directory :
  hid_t ntuples = tools_H5Gcreate(file,"ntuples",0);
  if(ntuples<0) {
    a_out << "thread " << a_thread << " : can't create groupd ntuples." << std::endl;
    ::H5Fclose(file);
    return false;
  }
  if(!tools::hdf5::write_atb(ntuples,"type","directory")) {
    a_out << "thread " << a_thread << " : write_atb() class failed." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return false;
  }

 {size_t wncols = 0;
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  double user_rgauss;
  nbk.add_column<double>("rgauss",user_rgauss);wncols++;
  nbk.add_column<float>("rbw");wncols++;
  nbk.add_column<std::string>("string");wncols++;
  short user_short;
  nbk.add_column<short>("short",user_short);wncols++;
  
  std::vector<float> user_vec_f;
  nbk.add_column<float>("vec_float",user_vec_f);wncols++;
  std::vector<double> user_vec_d;
  nbk.add_column<double>("vec_double",user_vec_d);wncols++;
  
  tools::hdf5::ntuple ntuple(a_out,ntuples,nbk,compress,basket_size);
  if(ntuple.columns().size()!=wncols) {
    a_out << "thread " << a_thread << " :"
          << " mismatch column numbers :"
	  << " " << ntuple.columns().size() << ". " << wncols << " expected." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return false;
  }
  
  tools::hdf5::ntuple::column<float>* col_rbw = ntuple.find_column<float>("rbw");
  if(!col_rbw) {
    a_out << "thread " << a_thread << " : column rbw not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
    return false;
  }
  tools::hdf5::ntuple::column_string* col_str = ntuple.find_column_string("string");
  if(!col_str) {
    a_out << "thread " << a_thread << " : column strings not found." << std::endl;
    ::H5Gclose(ntuples);
    ::H5Fclose(file);
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
    user_rgauss = vd;
    user_short = (short)wentries;
    
    if(!col_rbw->fill(rbwf.shoot())) {
      a_out << "thread " << a_thread << " : col_rbw fill failed." << std::endl;
      break;
    }
    
    if(!tools::num2s(count,stmp)){}
    if(!col_str->fill("str "+stmp)) {
      a_out << "thread " << a_thread << " : col_str fill failed." << std::endl;
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
    }}

    if(!ntuple.add_row()) {
      a_out << "thread " << a_thread << " : ntuple fill failed." << std::endl;
      break;
    }

    if(count==entries_3) ntuple.set_basket_size(613);
    if(count==entries_2_3) ntuple.set_basket_size(827);
  }}
  
  ::H5Gclose(ntuples);
  ::H5Fclose(file);

  return true;
}
  
class args : public tools::thread_args {
  typedef tools::thread_args parent;
public:
  args(tools::mutex& a_main_mutex,
       size_t a_index,//size_t a_num_threads,
       const std::string& a_prefix)
  :parent(a_main_mutex,a_prefix)
  ,m_index(a_index)
   //,m_num_threads(a_num_threads)
  {}
  virtual~ args() {}
private: //args not copiable.
  args(const args& a_from)
  :parent(a_from)
  ,m_index(a_from.m_index)
   //,m_num_threads(a_from.m_num_threads)
  {}
  args& operator=(const args& a_from){
    parent::operator=(a_from);
    return *this;
  }
public:
  void _main() {
    //m_out << "app::args::_main : thread " << m_index << std::endl;

    if(!write_ntuple(m_out,m_index)) {}

    m_mutex.lock();
    m_continue = false;
    m_mutex.unlock();
  }
protected:
  size_t m_index;
};

class threads : public tools::threads<args> {
  typedef tools::threads<args> parent;
public:
  virtual args* create_args(tools::mutex& a_main_mutex,size_t a_index,size_t /*a_num_threads*/) {
    std::string prefix;
    tools::num2s(a_index,prefix);
    prefix += ":";
    return new args(a_main_mutex,a_index,/*a_num_threads,*/prefix);
  }
public:
  threads(std::ostream& a_out,bool a_verbose = false)
  :parent(a_out,a_verbose)
  {}
  virtual ~threads() {}
protected:
  threads(const threads& a_from)
  :parent(a_from)
  {}
  threads& operator=(const threads&){return *this;}
};

} //app
  
#include <tools/args>
#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM

#ifndef H5_HAVE_THREADSAFE
  std::cout << "Your HDF5 lib is not built with H5_HAVE_THREADSAFE." << std::endl;
  return EXIT_FAILURE;   
#endif    

  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  if(verbose) std::cout << "HDF5 version " << H5_PACKAGE_VERSION << std::endl;
  
  unsigned int num_threads;
  args.find<unsigned int>("-threads",num_threads,2);

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  
  if(num_threads) {
    app::threads _threads(std::cout,verbose);
    if(_threads.start(num_threads)) {
      while(!_threads.ended()) {}
    }
  }

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  if(verbose) std::cout << "exit..." << std::endl;

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM

  return EXIT_SUCCESS;
}
