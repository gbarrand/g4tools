// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//  This program produces a mt_ntuple.root file by using multiple threads.

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

    tools::wroot::branch* main_branch = m_main_ntuple.get_row_wise_branch();
    bool row_wise = main_branch?true:false;
    
    tools::wroot::imt_ntuple* _imt_ntuple = 0;     //for add_row, end_fill
    tools::wroot::base_pntuple* _base_pntuple = 0; //to find columns.
    
    std::vector<tools::wroot::branch*> main_branches; //for column wise.
    
    if(row_wise) {
      tools::wroot::mt_ntuple_row_wise* mt_ntuple =
        new tools::wroot::mt_ntuple_row_wise(m_out,
                                             main_file.byte_swap(),main_file.compression(),
                                             m_main_ntuple.dir().seek_directory(),
                                             *main_branch,
                                             main_branch->basket_size(),m_nbk,m_verbose);
      _imt_ntuple = static_cast<tools::wroot::imt_ntuple*>(mt_ntuple);
      _base_pntuple = static_cast<tools::wroot::base_pntuple*>(mt_ntuple);
    } else {
      m_main_ntuple.get_branches(main_branches);
      std::vector<tools::uint32> basket_sizes;
     {tools_vforcit(tools::wroot::branch*,main_branches,it) basket_sizes.push_back((*it)->basket_size());}
   
      tools::wroot::mt_ntuple_column_wise* mt_ntuple =
        new tools::wroot::mt_ntuple_column_wise(m_out,
                                                main_file.byte_swap(),main_file.compression(),
                                                m_main_ntuple.dir().seek_directory(),
	                                        main_branches,
                                                basket_sizes,m_nbk,m_verbose);
      _imt_ntuple = static_cast<tools::wroot::imt_ntuple*>(mt_ntuple);
      _base_pntuple = static_cast<tools::wroot::base_pntuple*>(mt_ntuple);
    }

    bool col_not_found = false;
    tools::wroot::base_pntuple::column<double>* col_rgauss = _base_pntuple->find_column<double>("rgauss");
    if(!col_rgauss) {m_out << "column rgauss not found." << std::endl;col_not_found = true;}
    tools::wroot::base_pntuple::column<float>* col_rbw = _base_pntuple->find_column<float>("rbw");
    if(!col_rbw) {m_out << "column rbw not found." << std::endl;col_not_found = true;}
    tools::wroot::base_pntuple::column_string* col_str = _base_pntuple->find_column_string("string");
    if(!col_str) {m_out << "column string not found." << std::endl;col_not_found = true;}
    tools::wroot::base_pntuple::column<int>* col_count = _base_pntuple->find_column<int>("count");
    if(!col_count) {m_out << "column count not found." << std::endl;col_not_found = true;}
    tools::wroot::base_pntuple::std_vector_column<double>* col_vec_d = _base_pntuple->find_column_vector<double>("vec_d");
    if(!col_vec_d) {m_out << "column vec_d not found." << std::endl;col_not_found = true;}
    tools::wroot::base_pntuple::column_vector_string* col_vec_s = _base_pntuple->find_column_vector_string("vec_s");
    if(!col_vec_s) {m_out << "column vec_s not found." << std::endl;col_not_found = true;}

    if(col_not_found) {
      _base_pntuple->print_columns(m_out);
      delete _imt_ntuple;
      m_mutex.lock();
      m_continue = false;
      m_mutex.unlock();
      return;    
    }
    
    mutex _mutex(m_main_mutex); //and not m_mutex.

   {tools::rgaussd rg(1,2);
    tools::rbwf rbwf(0,1);
    tools::rbwd rbwd(-1,1);
    tools::rtausmed rflat;
    //unsigned int user_vec_d_count = 0;
    std::vector<double> user_vec_d;
    std::string stmp;
    unsigned int entries = m_megas*1000000;
    for(unsigned int count=0;count<entries;count++) {
    //if(!col_rgauss->fill(rg.shoot())) {
    //  m_out << "col_rgauss fill failed." << std::endl;
    //  break;
    //}
    //col_rgauss->variable() = rg.shoot();
      *col_rgauss = rg.shoot();
      
      if(!col_rbw->fill(rbwf.shoot())) {
        m_out << "col_rbw fill failed." << std::endl;
        break;
      }
      
      if(!tools::num2s(count,stmp)){}
      if(!col_str->fill("str "+stmp)) {
        m_out << "col_str fill failed." << std::endl;
        break;
      }
      if(!col_count->fill(count)) {
        m_out << "col_count fill failed." << std::endl;
        break;
      }
     
     {user_vec_d.clear();
      unsigned int number = row_wise ? count%100 : (unsigned int)(10*rflat.shoot());
      for(unsigned int i=0;i<number;i++) user_vec_d.push_back(rg.shoot());
      col_vec_d->fill(user_vec_d);
      //user_vec_d_count += number;
     }
     
     {std::vector<std::string>& vec_s = col_vec_s->variable();
      vec_s.clear();
      unsigned int number = row_wise ? count%5 : (unsigned int)(5*rflat.shoot());
      for(unsigned int i=0;i<number;i++) {
        if(!tools::num2s(i,stmp)){}
        vec_s.push_back(stmp);
      }}

      if(!_imt_ntuple->add_row(_mutex,main_file)) { //it may lock/unlock when passing a basket to a main_ntuple branch.
        m_out << "ntuple fill failed." << std::endl;
        break;
      }
    }
    //m_out << "user_vec_d_count " << user_vec_d_count << std::endl;
    }

    if(!_imt_ntuple->end_fill(_mutex,main_file)) { //important.
      m_out << "end_fill() failed." << std::endl;
    }
    
    delete _imt_ntuple;

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
  std::string file = "mt_ntuple.root";
  
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
 {tools::ntuple_booking nbk("rg_rbw","Randoms");
  nbk.add_column<double>("rgauss");
  nbk.add_column<float>("rbw");
  nbk.add_column<std::string>("string");
  nbk.add_column<int>("count");
  nbk.add_column_vec<double>("vec_d");
  nbk.add_column_vec<std::string>("vec_s");
  
  tools::wroot::ntuple* main_ntuple = new tools::wroot::ntuple(rfile.dir(),nbk,row_wise); //owned by the directory.
  //main_ntuple->print_columns(std::cout);
  
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

