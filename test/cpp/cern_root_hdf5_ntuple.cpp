// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.


#include <tools/hdf5/ntuple>

#include <TH1D.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TApplication.h>

#include <iostream>

int main(int argc,char** argv) {

#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM

  ////////////////////////////////////////
  ////////////////////////////////////////
  ////////////////////////////////////////
  TH1D hgr("h","rgauss",100,-5,5);

  ////////////////////////////////////////
  ////////////////////////////////////////
  ////////////////////////////////////////
  
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

  tools::hdf5::ntuple ntuple(std::cout,ntuples,"rg_rbw");
  
  tools::hdf5::ntuple::column<double>* col_rgauss = ntuple.find_column<double>("rgauss");
  if(!col_rgauss) {
    std::cout << "column rgauss not found." << std::endl;
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

  for(tools::uint64 row=0;row<entries;row++) {
    if(!ntuple.get_row()) {
      std::cout << "ntuple.get_row() failed." << std::endl;
      ::H5Gclose(ntuples); 
      ::H5Fclose(file);
      return EXIT_FAILURE;
    }    
    double vd;
    col_rgauss->get_entry(vd);
    hgr.Fill(vd);
  }

  ::H5Gclose(ntuples); 
  ::H5Fclose(file);}
  
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
  if(num_open) std::cout << "warning : H5F num_open " << num_open << std::endl;}
 
  ////////////////////////////////////////////////////////
  /// plot histos : //////////////////////////////////////
  ////////////////////////////////////////////////////////
  new TApplication("cern_root_hdf5_ntuple",&argc,argv);

  TCanvas* plotter = new TCanvas("canvas","",10,10,800,600);
  plotter->Divide(1,1);  

  plotter->cd(1);
  hgr.Draw();

  plotter->Update();

  gSystem->Run();

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM

  return EXIT_SUCCESS;
}  
