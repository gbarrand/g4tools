// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

// This program produces a mt_ntuple_seq.root file by using tools::wroot::mt_ntuple_[row,column]_wise
// but by NOT using multiple threads. The filling logic is similar to the mt_ntuple.cpp
// program but by doing "threads in sequence". It permits to help debugging a truely
// multiple threads situation if needed. It permits to use the tools::mem counting.

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

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
  virtual bool lock() {return true;}
  virtual bool unlock() {return true;}
public:
  mutex(){}
  virtual ~mutex(){}
protected:
  mutex(const mutex& a_from):parent(a_from){}
  mutex& operator=(const mutex&){return *this;}
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
  tools::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  unsigned int num_threads;
  args.find<unsigned int>("-threads",num_threads,2);

  unsigned int num_megas;
  args.find<unsigned int>("-megas",num_megas,1);

  unsigned int basket_size;
  args.find<unsigned int>("-basket_size",basket_size,32000);

  bool row_wise = !args.is_arg("-column_wise"); //default is row_wise.
  
  //////////////////////////////////////////////////////////
  /// create a .root file : ////////////////////////////////
  //////////////////////////////////////////////////////////
  if(verbose) std::cout << (row_wise?"row_wise":"column_wise") << std::endl;
  
  std::string file = "mt_ntuple_seq.root";
  
 {tools::wroot::file rfile(std::cout,file);
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
  tools::wroot::ntuple* main_ntuple = 0;
  
  // the booking object :
  tools::ntuple_booking nbk("rg_rbw","Randoms");
  
 {//user variables to create "ref columns" in the main ntuple. (They are not used to fill data) :
  double user_rgauss;
  float user_rbw;
  std::string user_string;
  int user_count;
  std::vector<double> user_vec_d;
  std::vector<std::string> user_vec_s;
  
  nbk.add_column("rgauss",user_rgauss);
  nbk.add_column("rbw",user_rbw);
  nbk.add_column("string",user_string);
  nbk.add_column("count",user_count);
  nbk.add_column("vec_d",user_vec_d);
  nbk.add_column("vec_s",user_vec_s);

  main_ntuple = new tools::wroot::ntuple(rfile.dir(),nbk,row_wise); //owned by the directory.
  if(!row_wise) main_ntuple->set_basket_size(basket_size);}
  
  mutex _mutex;

 {for(unsigned int ithread=0;ithread<num_threads;ithread++) {

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

    if(!nbk_thread.has_similar_layout(std::cout,nbk)) {
      std::cout << "local thread :"
                << " not similar booking as the main one."
                << std::endl;
      break;
    }
    
    tools::wroot::imt_ntuple* pntuple = 0;
    
    std::vector<tools::wroot::branch*> main_branches; //for column wise.
    
    if(row_wise) {
      tools::wroot::branch* main_branch = main_ntuple->get_row_wise_branch();
      pntuple = new tools::wroot::mt_ntuple_row_wise(std::cout,
                                                     rfile.byte_swap(),rfile.compression(),rfile.dir().seek_directory(),
                                                     *main_branch,
                                                     basket_size,nbk_thread,verbose);
    } else {
      main_ntuple->get_branches(main_branches);
      std::vector<tools::uint32> basket_sizes;
     {tools_vforcit(tools::wroot::branch*,main_branches,it) basket_sizes.push_back((*it)->basket_size());}
  
      pntuple = new tools::wroot::mt_ntuple_column_wise(std::cout,
                                                        rfile.byte_swap(),rfile.compression(),rfile.dir().seek_directory(),
                                                        main_branches,
                                                        basket_sizes,nbk_thread,verbose);
    }


   {tools::rgaussd rg(1,2);
    tools::rbwf rbwf(0,1);
    tools::rbwd rbwd(-1,1);
    tools::rtausmed rflat;
    unsigned int user_vec_d_count = 0;
    std::string stmp;
    unsigned int entries = num_megas*1000000;
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
      user_vec_d_count += number;}
    
     {user_vec_s.clear();
      unsigned int number = row_wise ? count%5 : (unsigned int)(5*rflat.shoot());
      for(unsigned int i=0;i<number;i++) {
        if(!tools::num2s(i,stmp)){}
        user_vec_s.push_back(stmp);
      }}
    
      if(!pntuple->add_row(_mutex,rfile)) {
        std::cout << "ntuple fill failed." << std::endl;
        break;
      }
    }
    if(verbose) std::cout << "thread : " << ithread << ", user_vec_d_count " << user_vec_d_count << std::endl;}

    if(!pntuple->end_fill(_mutex,rfile)) { //important.
      std::cout << "end_fill() failed." << std::endl;
    }
    
    delete pntuple;
  }}    

  main_ntuple->merge_number_of_entries();

  if(verbose) std::cout << "entries " << main_ntuple->entries() << std::endl;

  //////////////////////////////////////////////////////////
  /// write and close file : ///////////////////////////////
  //////////////////////////////////////////////////////////
 {unsigned int n;
  if(!rfile.write(n)) {
    std::cout << "file write failed." << std::endl;
  }}

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

