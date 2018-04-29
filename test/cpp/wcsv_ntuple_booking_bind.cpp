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
  unsigned int user_index;
  nbk.add_column<unsigned int>("index",user_index);
  double user_rgauss;
  nbk.add_column<double>("rgauss",user_rgauss);
  float user_rbw;
  nbk.add_column<float>("rbw",user_rbw);
  std::string user_str;
  nbk.add_column<std::string>("string",user_str);
  //nbk.add_column<bool>("not_handled");
  std::vector<int> user_vec_int;
  nbk.add_column<int>("name_vec_int",user_vec_int);
  std::vector<unsigned int> user_vec_uint;
  nbk.add_column<unsigned int>("name_vec_uint",user_vec_uint);
  std::vector<std::string> user_vec_string;
  nbk.add_column<std::string>("name_vec_string",user_vec_string);
  //////////////////////////////////////////////////////////
  /// create and write a ntuple : //////////////////////////
  //////////////////////////////////////////////////////////
  tools::wcsv::ntuple ntu(writer,std::cout,nbk); //default sep is ','

  if(ntu.columns().size()) {

    //ntu.write_commented_header(writer);
 
    //::printf("debug : %lu\n",ntu.find_column<unsigned int>("index"));
    //::printf("debug : %lu\n",ntu.find_column_ref<unsigned int>("index"));

    // fill :
    char snum[128];
    for(unsigned int count=0;count<entries;count++) {    
      user_index = count;
      user_rgauss = rg.shoot();
      user_rbw = rbw.shoot();
      user_str = "str ";
      if(!tools::numas(count,user_str)){}
      
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
