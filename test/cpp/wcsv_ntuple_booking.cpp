// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//  This program produces a wcsv_booking.csv file.

#include <tools/wcsv_ntuple>

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>

#include <fstream>
#include <cstdio>

#include <iostream>
#include <cstdlib>

int main(int,char**) {

  //////////////////////////////////////////////////////////
  /// create a .csv file : /////////////////////////////////
  //////////////////////////////////////////////////////////
  std::ofstream writer("wcsv_booking.csv");
  if(writer.fail()) {
    std::cout << "can't open wcsv_booking.csv." << std::endl;
    return EXIT_FAILURE;
  }

  unsigned int entries = 1000;
  tools::rgaussd rg(1,2);
  tools::rbwf rbw(0,1);

  //////////////////////////////////////////////////////////
  /// create a ntuple_booking object : /////////////////////
  //////////////////////////////////////////////////////////
  tools::ntuple_booking nbk;
  nbk.add_column<unsigned int>("index");
  nbk.add_column<double>("rgauss");
  nbk.add_column<float>("rbw");
  //WARNING : strings must not contain the "sep" and "sep vec" character which are ',' and ';' by default.
  nbk.add_column<std::string>("string");
  //nbk.add_column<bool>("not_handled");
  std::vector<int> user_vec_int;
  nbk.add_column<int>("name_vec_int",user_vec_int);
  std::vector<unsigned int> user_vec_uint;
  nbk.add_column<unsigned int>("name_vec_uint",user_vec_uint);
  //WARNING : strings must not contain the "sep" and "sep vec" character which are ',' and ';' by default.
  std::vector<std::string> user_vec_string;
  nbk.add_column<std::string>("name_vec_string",user_vec_string);
  //////////////////////////////////////////////////////////
  /// create and write a ntuple : //////////////////////////
  //////////////////////////////////////////////////////////
  tools::wcsv::ntuple ntu(writer,std::cout,nbk); //default sep is ','

  if(ntu.columns().size()) {

    //ntu.write_commented_header(writer);

    tools::wcsv::ntuple::column<unsigned int>* col_index = ntu.find_column<unsigned int>("index");
    tools::wcsv::ntuple::column<double>* col_rgauss = ntu.find_column<double>("rgauss");
    tools::wcsv::ntuple::column<float>* col_rbw = ntu.find_column<float>("rbw");
    tools::wcsv::ntuple::column<std::string>* col_str = ntu.find_column<std::string>("string");

    // fill :
    char snum[128];
    std::string stmp;
    for(unsigned int count=0;count<entries;count++) {    
      col_index->fill(count);
      col_rgauss->fill(rg.shoot());
      col_rbw->fill(rbw.shoot());

      if(!tools::num2s(count,stmp)){}
      if(!col_str->fill("str "+stmp)) {
        std::cout << "col_str fill failed." << std::endl;
        break;
      }
    
      // fill vector
      user_vec_int.clear();
      int number = count%5;
     {for(int i=0;i<number;i++) {
        user_vec_int.push_back(-i);
      }}

      // fill vector
      user_vec_uint.clear();
     {for(int i=0;i<number;i++) {
        user_vec_uint.push_back(i);
      }}

      user_vec_string.clear();
     {for(int i=0;i<number;i++) {
        ::sprintf(snum,"s_%d",i); //WARNING : strings must not contain the "sep vec" character which is ';' by default.
        user_vec_string.push_back(snum);
      }}

      ntu.add_row(); // it will write columns data as a row in the file.
    }

  }

  //////////////////////////////////////////////////////////
  /// close file : /////////////////////////////////////////
  //////////////////////////////////////////////////////////
  writer.close();

  return EXIT_SUCCESS;
}
