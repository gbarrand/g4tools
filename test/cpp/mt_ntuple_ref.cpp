// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//  This program produces a mt_ntuple_ref.root file by using multiple threads.
// The "_ref" refers to the fact that each ntuple column handles a reference to a user variable which is used at fill time.

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include "threads"

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>
#include <tools/wroot/file>
#include <tools/wroot/ntuple>

#include <tools/wroot/mt_ntuple_row_wise>
#include <tools/wroot/mt_ntuple_column_wise>

class mutex : public virtual tools::wroot::imutex {
  typedef tools::wroot::imutex parent;
public:
  virtual bool lock() {return m_mutex.lock();}
  virtual bool unlock() {return m_mutex.unlock();}
  //virtual bool trylock() {return m_mutex.trylock();}
public:
  mutex(tools::mutex& a_mutex):m_mutex(a_mutex){}
  virtual ~mutex(){}
protected:
  mutex(const mutex& a_from):parent(a_from),m_mutex(a_from.m_mutex){}
  mutex& operator=(const mutex&){return *this;}
protected:
  tools::mutex& m_mutex;
};

class thread_args : public tools::thread_args {
  typedef tools::thread_args parent;
public:
  thread_args(tools::mutex& a_main_mutex,
              size_t a_index,//size_t a_num_threads,
              const std::string& a_prefix,
              const tools::ntuple_booking& a_nbk,
              tools::wroot::ntuple& a_main_ntuple,
              unsigned int a_megas)
  :parent(a_main_mutex,a_prefix)
  ,m_index(a_index)
   //,m_num_threads(a_num_threads)
  ,m_nbk(a_nbk)
  ,m_main_ntuple(a_main_ntuple)
  ,m_megas(a_megas)
  {}
  virtual~ thread_args() {}
private: //thread_args not copiable.
  thread_args(const thread_args& a_from)
  :parent(a_from)
  ,m_index(a_from.m_index)
   //,m_num_threads(a_from.m_num_threads)
  ,m_main_ntuple(a_from.m_main_ntuple)
  {}
  thread_args& operator=(const thread_args& a_from){
    parent::operator=(a_from);
    return *this;
  }
public:
  void _main() {
    //m_out << "_main : thread " << m_index << std::endl;

    tools::wroot::ifile& main_file = m_main_ntuple.dir().file();
    
    //user variables used to create "ref columns" in the local thread ntuple and also used to fill user data :
    double user_rgauss;
    float user_rbw;
    std::string user_string;
    int user_count;
    std::vector<double> user_vec_d;
    std::vector<std::string> user_vec_s;

    // the booking object (it must have the same layout as the main one) :
    tools::ntuple_booking nbk_thread("rg_rbw","Randoms");
    nbk_thread.add_column("rgauss",user_rgauss);
    nbk_thread.add_column("rbw",user_rbw);
    nbk_thread.add_column("string",user_string);
    nbk_thread.add_column("count",user_count);
    nbk_thread.add_column("vec_d",user_vec_d);
    nbk_thread.add_column("vec_s",user_vec_s);

    if(!nbk_thread.has_similar_layout(m_out,m_nbk)) {
      m_out << "local thread : not similar booking as the main one." << std::endl;
      m_mutex.lock();
      m_continue = false;
      m_mutex.unlock();
    }
    
    tools::wroot::branch* main_branch = m_main_ntuple.get_row_wise_branch();
    bool row_wise = main_branch?true:false;
    
    tools::wroot::imt_ntuple* _ntuple = 0;
    
    std::vector<tools::wroot::branch*> main_branches; //for column wise.
    
    if(row_wise) {
      _ntuple = new tools::wroot::mt_ntuple_row_wise(m_out,
                                                     main_file.byte_swap(),main_file.compression(),
                                                     m_main_ntuple.dir().seek_directory(),
                                                     *main_branch,
                                                     main_branch->basket_size(),nbk_thread,m_verbose);
    } else {
      m_main_ntuple.get_branches(main_branches);
      std::vector<tools::uint32> basket_sizes;
     {tools_vforcit(tools::wroot::branch*,main_branches,it) basket_sizes.push_back((*it)->basket_size());}
   
      _ntuple = new tools::wroot::mt_ntuple_column_wise(m_out,
                                                        main_file.byte_swap(),main_file.compression(),
                                                        m_main_ntuple.dir().seek_directory(),
	                                                main_branches,
                                                        basket_sizes,nbk_thread,m_verbose);
    }

    mutex _mutex(m_main_mutex); //and not m_mutex.

   {tools::rgaussd rg(1,2);
    tools::rbwf rbwf(0,1);
    tools::rbwd rbwd(-1,1);
    tools::rtausmed rflat;
    //unsigned int user_vec_d_count = 0;
    std::string stmp;
    unsigned int entries = m_megas*1000000;
    for(unsigned int count=0;count<entries;count++) {
      // fill variables :
      user_rgauss = rg.shoot();
      user_rbw = rbwf.shoot();
      
      if(!tools::num2s(count,stmp)){}
      user_string = "str "+stmp;

      user_count = count;

     {user_vec_d.clear();
      unsigned int number = row_wise ? count%100 : (unsigned int)(10*rflat.shoot());
      for(unsigned int i=0;i<number;i++) user_vec_d.push_back(rg.shoot());
      //user_vec_d_count += number;
     }
     
     {user_vec_s.clear();
      unsigned int number = row_wise ? count%5 : (unsigned int)(5*rflat.shoot());
      for(unsigned int i=0;i<number;i++) {
        if(!tools::num2s(i,stmp)){}
        user_vec_s.push_back(stmp);
      }}
    
      if(!_ntuple->add_row(_mutex,main_file)) { //it may lock/unlock when passing a basket to a main_ntuple branch.
        m_out << "ntuple fill failed." << std::endl;
        break;
      }
    }
    //m_out << "user_vec_d_count " << user_vec_d_count << std::endl;
    }

    if(!_ntuple->end_fill(_mutex,main_file)) { //important.
      m_out << "end_fill() failed." << std::endl;
    }
    
    delete _ntuple;

    m_mutex.lock();
    m_continue = false;
    m_mutex.unlock();
  }
protected:
  size_t m_index;
  tools::ntuple_booking m_nbk;
  tools::wroot::ntuple& m_main_ntuple;
  unsigned int m_megas;
};

class threads : public tools::threads<thread_args> {
  typedef tools::threads<thread_args> parent;
public:
  virtual thread_args* create_args(tools::mutex& a_main_mutex,size_t a_index,size_t /*a_num_threads*/) {
    std::string prefix;
    tools::num2s(a_index,prefix);
    prefix += ":";
    return new thread_args(a_main_mutex,a_index,/*a_num_threads,*/prefix,m_nbk,m_main_ntuple,m_megas);
  }
public:
  threads(std::ostream& a_out,
                 const tools::ntuple_booking& a_nbk,
                 tools::wroot::ntuple& a_main_ntuple,
                 unsigned int a_megas,
                 bool a_verbose = false)
  :parent(a_out,a_verbose)
  ,m_nbk(a_nbk)
  ,m_main_ntuple(a_main_ntuple) 
  ,m_megas(a_megas)
  {}
  virtual ~threads() {}
protected:
  threads(const threads& a_from)
  :parent(a_from)
  ,m_nbk(a_from.m_nbk)
  ,m_main_ntuple(a_from.m_main_ntuple)
  {}
  threads& operator=(const threads&){return *this;}
protected:
  tools::ntuple_booking m_nbk;
  tools::wroot::ntuple& m_main_ntuple;
  unsigned int m_megas;
};

// if reading the file at end :
#include <tools/rroot/file>
#include <tools/rroot/ntuple>
#include <tools/rroot/fac>
#include <tools/eqT>

#ifndef TOOLS_DONT_HAVE_ZLIB
#define TOOLS_DONT_HAVE_ZLIB
#endif

#ifdef TOOLS_DONT_HAVE_ZLIB
#else
#include <tools/zlib>
#endif

#include <tools/args>
#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM
    
  //////////////////////////////////////////////////////////
  /// args : ///////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  bool row_wise = !args.is_arg("-column_wise"); //default is row_wise.
  
  //////////////////////////////////////////////////////////
  /// create a .root file : ////////////////////////////////
  //////////////////////////////////////////////////////////
  std::string file = "mt_ntuple_ref.root";
  
 {unsigned int num_threads;
  args.find<unsigned int>("-threads",num_threads,2);
  unsigned int num_megas;
  args.find<unsigned int>("-megas",num_megas,1);
  unsigned int basket_size;
  args.find<unsigned int>("-basket_size",basket_size,32000);
  
  if(verbose) {
    std::cout << (row_wise?"row_wise":"column_wise") << std::endl;
    std::cout << "num threads " << num_threads << std::endl;
    std::cout << "num megas (per thread) " << num_megas << std::endl;
    std::cout << "basket_size " << basket_size << std::endl;
  }
  
  tools::wroot::file rfile(std::cout,file);
#ifdef TOOLS_DONT_HAVE_ZLIB
#else
  if(args.is_arg("-noz")){
  } else {
    rfile.add_ziper('Z',exlib::compress_buffer);
    rfile.set_compression(9);
  }
#endif

  //////////////////////////////////////////////////////////
  /// create a ntuple from a ntuple_booking object. ////////
  //////////////////////////////////////////////////////////
 {//user variables used in the main ntuple to create "ref columns". (They are not used to fill data):
  double user_rgauss;
  float user_rbw;
  std::string user_string;
  int user_count;
  std::vector<double> user_vec_d;
  std::vector<std::string> user_vec_s;

  // the booking object :
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  nbk.add_column("rgauss",user_rgauss);
  nbk.add_column("rbw",user_rbw);
  nbk.add_column("string",user_string);
  nbk.add_column("count",user_count);
  nbk.add_column("vec_d",user_vec_d);
  nbk.add_column("vec_s",user_vec_s);
  
  tools::wroot::ntuple* main_ntuple = new tools::wroot::ntuple(rfile.dir(),nbk,row_wise); //owned by the directory.

  main_ntuple->set_basket_size(basket_size);

  if(num_threads) {
    threads _threads(std::cout,nbk,*main_ntuple,num_megas,verbose);
    if(_threads.start(num_threads)) {
      while(!_threads.ended()) {}
    }
  }

  main_ntuple->merge_number_of_entries();

  if(verbose) std::cout << "entries " << main_ntuple->entries() << std::endl;

  //////////////////////////////////////////////////////////
  /// write and close file : ///////////////////////////////
  //////////////////////////////////////////////////////////
 {unsigned int n;
  if(!rfile.write(n)) {
    std::cout << "file write failed." << std::endl;
  }}

  }
  
  rfile.close();}

  //////////////////////////////////////////////////////////
  /// read the file : //////////////////////////////////////
  //////////////////////////////////////////////////////////

#include "read_root.icc"
 
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

  if(verbose) std::cout << "exit ..." << std::endl;
  
#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM

  return EXIT_SUCCESS;
}

