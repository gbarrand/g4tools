// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <TFile.h>
#include <TTree.h>
#include <TLeafD.h>
#include <TLeafC.h>

#include <tools/sto>
#include <tools/get_lines>
#include <tools/eqT>
#include <tools/mathd>

#include <iostream>
#include <sstream>

void root_read_rg_rbw(const std::string& a_file,bool verbose) {
  
  typedef char* cstr;
  
  TFile* file = new TFile(a_file.c_str(),"READ");
  if(verbose) file->ls();
   
  TTree* tree = (TTree*)file->Get("rg_rbw");
  if(!tree) {
    std::cout << "tree rg_rbw not found." << std::endl;
    file->Close();
    return;
  }
  int entries = tree->GetEntries();
  if(verbose) ::printf("tree entries %d\n",entries);

  bool row_wise = true;
  TBranch* event_branch = tree->FindBranch("row_wise_branch");
  if(!event_branch) {
    std::cout << "event_branch not found. Assume column_wise." << std::endl;
    //file->Close();
    //return;
    row_wise = false;
  }
  TLeaf* event_rgauss_leaf = tree->FindLeaf("rgauss");
  if(!event_rgauss_leaf) {
    std::cout << "event_rgauss_leaf not found." << std::endl;
    file->Close();
    return;
  }
  TLeaf* event_rbw_leaf = tree->FindLeaf("rbw");
  if(!event_rbw_leaf) {
    std::cout << "event_rbw_leaf not found." << std::endl;
    file->Close();
    return;
  }
  TLeafC* event_string_leaf = (TLeafC*)tree->FindLeaf("string");
  if(!event_string_leaf) {
    std::cout << "event_string_leaf not found." << std::endl;
    file->Close();
    return;
  }
  TLeaf* event_count_leaf = tree->FindLeaf("count");
  if(!event_count_leaf) {
    std::cout << "event_count_leaf not found." << std::endl;
    file->Close();
    return;
  }
  TLeaf* event_vec_d_count_leaf = 0;
  if(row_wise) {
    event_vec_d_count_leaf = (TLeafD*)tree->FindLeaf("vec_d_count");
    if(!event_vec_d_count_leaf) {
      std::cout << "event_vec_d_count_leaf not found." << std::endl;
      file->Close();
      return;
    }
  }
  TLeafD* event_vec_d_leaf = (TLeafD*)tree->FindLeaf("vec_d");
  if(!event_vec_d_leaf) {
    std::cout << "event_vec_d_leaf not found." << std::endl;
    file->Close();
    return;
  }
  TLeafC* event_vec_s_leaf = (TLeafC*)tree->FindLeaf("vec_s");
  if(!event_vec_s_leaf) {
    std::cout << "event_vec_s_leaf not found." << std::endl;
    file->Close();
    return;
  }
  
  struct event {
    double  user_rgauss;
    float   user_rbw;
    cstr*   user_string;
    int     user_count;
    double* user_vec_d;
    cstr*   user_vec_s;
  } _event = {0,0,0,0,0,0};

  if(verbose) {for(int count=0;count<5;count++) tree->Show(count);}
  if(verbose) {for(int count=entries-5;count<entries;count++) tree->Show(count);}
  
  if(event_branch) event_branch->SetAddress(&_event);
  
  event_rgauss_leaf->SetAddress(&_event.user_rgauss);
  event_rbw_leaf->SetAddress(&_event.user_rbw);
  event_string_leaf->SetAddress(_event.user_string);
  event_count_leaf->SetAddress(&_event.user_count);
  event_vec_d_leaf->SetAddress(_event.user_vec_d);
  event_vec_s_leaf->SetAddress(_event.user_vec_s);

  int event_vec_d_count = 0;
  if(event_vec_d_count_leaf) {
    event_vec_d_count_leaf->SetAddress(&event_vec_d_count);
  }
  
  double Sw = 0;
  double Sxw = 0;
  double Sx2w = 0;
  
  int user_string_count = 0;
  std::string stmp;

  bool check_string = true;
  //bool check_string = false;
  
 {for(int row=0;row<entries;row++) {
    tree->GetEntry(row);
    if(verbose) {
      ::printf("event %d :\n",row);
      ::printf("  event.user_rgauss %g\n",_event.user_rgauss);
      ::printf("  event.user_rbw %g\n",_event.user_rbw);
    
      char* _event_user_string = event_string_leaf->GetValueString();    
      ::printf("  event.user_string \"%s\"\n",_event_user_string?_event_user_string:"nil");
    
      ::printf("  event.user_count %d\n",_event.user_count);
    
     {int ndata = event_vec_d_leaf->GetNdata();
      ::printf("  event.user_vec_d %d\n",ndata);
      for(int i=0;i<ndata;i++) {
        double value = event_vec_d_leaf->GetValue(i);
        ::printf("    event.user_vec_d[%d] = %g\n",i,value);
      }}

      char* _event_user_vec_s = event_vec_s_leaf->GetValueString();
      ::printf("  event.user_vec_s \"%s\"\n",_event_user_vec_s?_event_user_vec_s:"nil");

    }
    //::printf("  event_vec_d_count %d\n",event_vec_d_count);
    
    Sw += 1;
    Sxw += _event.user_rgauss;
    Sx2w += _event.user_rgauss*_event.user_rgauss;
    
    if(check_string) {
      char* _event_user_string = event_string_leaf->GetValueString();
      if(::strlen(_event_user_string)<4) {
        std::cout << "bad string length " << ::strlen(_event_user_string) << std::endl;
        user_string_count = 0;
      } else {
        std::string user_string(_event_user_string);
        stmp = user_string.substr(4,user_string.size()-4);
        if(!tools::to<int>(stmp,user_string_count)) {
          std::cout << "string " << stmp << " not an integer." << std::endl;
          break;
        }
      }
    }

    if(row_wise) {
      if(check_string) {
        if(user_string_count!=_event.user_count)  {
          std::cout << "found an inconsistent row at row : "<< row
                    << ", user_string_count " << user_string_count
                    << ", user_count " << _event.user_count
                    << std::endl;
        }
      }
	
      if(event_vec_d_leaf->GetNdata()!=(_event.user_count%100))  {
        std::cout << "found an inconsistent row at row : "<< row
                  << ", user_vec_d.size() " << event_vec_d_leaf->GetNdata()
                  << ", user_count%100 " << (_event.user_count%100)
                  << std::endl;
      }
      
      if(check_string) {
        char* _event_user_vec_s = event_vec_s_leaf->GetValueString();
        std::vector<std::string> user_vec_s;
        tools::get_lines(_event_user_vec_s,user_vec_s);
        if(int(user_vec_s.size())!=(_event.user_count%5))  {
          std::cout << "found an inconsistent row at row : "<< row
                    << ", user_vec_s.size() " << user_vec_s.size()
                    << ", user_count%5 " << (_event.user_count%5)
                    << std::endl;
        }
      }
    }
    
  }}

  file->Close();

  if(Sw) {
    double mean = Sxw/Sw;
    double rms = ::sqrt(::fabs(Sx2w/Sw-mean*mean));
    double depsilon = 1e-2;
    if(!tools::numbers_are_equal(mean,1.0,depsilon,tools::dfabs)) {
      std::cout << "rgauss mean : " << mean << ". Expected 1.0." << std::endl;
    }
    if(!tools::numbers_are_equal(rms,2.0,depsilon,tools::dfabs)) {
      std::cout << "rgauss rms : " << rms << ". Expected 2.0." << std::endl;
    }
  }
  
}    

#include <tools/args>
#include <cstdlib>

int main(int argc,char** argv) {
  tools::args args(argc,argv);
  
  std::string file;
  if(!args.file(file)) {
    std::cout << "give a root file." << std::endl;
    return EXIT_FAILURE;
  }

  bool verbose = args.is_arg("-verbose");
  
  root_read_rg_rbw(file,verbose);
  
  return EXIT_SUCCESS;
}     
