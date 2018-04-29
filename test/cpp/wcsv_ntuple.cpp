// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//  This program produces a wcsv.csv file.

#include <tools/wcsv_ntuple>

#include <tools/randd>
#include <tools/randf>
#include <tools/num2s>

#include <fstream>

#include <tools/args>

#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {

  tools::args args(argc,argv);

  bool is_large = args.is_arg("-large");

  //////////////////////////////////////////////////////////
  /// create a .csv file : /////////////////////////////////
  //////////////////////////////////////////////////////////
  std::ofstream writer("wcsv.csv");
  if(writer.fail()) {
    std::cout << "can't open wcsv.csv." << std::endl;
    return EXIT_FAILURE;
  }

  unsigned int entries = 1000;
  if(is_large) {
    entries = 10000000; //to test >2Gbytes file.
  }

  tools::rgaussd rg(1,2);
  tools::rbwf rbw(0,1);

  //////////////////////////////////////////////////////////
  /// create and write a ntuple : //////////////////////////
  //////////////////////////////////////////////////////////
  //tools::wcsv::ntuple ntu(writer,'\t');
  tools::wcsv::ntuple ntu(writer); //default sep is ','
  ntu.set_title("csv ntuple");

  // create some columns with basic types :
  tools::wcsv::ntuple::column<unsigned int>* col_uint = ntu.create_column<unsigned int>("name_uint");
  tools::wcsv::ntuple::column<double>* col_double = ntu.create_column<double>("name_double");
  std::vector<double> user_vec_double;
  ntu.create_column<double>("name_vec_double",user_vec_double);
  tools::wcsv::ntuple::column<float>* col_float = ntu.create_column<float>("name_float");
  std::vector<int> user_vec_int;
  ntu.create_column<int>("name_vec_int",user_vec_int);
  tools::wcsv::ntuple::column<std::string>* col_string = ntu.create_column<std::string>("name_string");

  //WARNING : strings must not contain the "sep" and "sep vec" character which are ',' and ';' by default.
  std::vector<std::string> user_vec_string;
  ntu.create_column<std::string>("name_vec_string",user_vec_string);

  //ntu.write_hippo_header();
  if(!ntu.write_commented_header(std::cout)) {
    std::cout << "problem when writing the commented header (column type unknow?)." << std::endl;
  }

  // fill :
  std::string stmp;
  for(unsigned int count=0;count<entries;count++) {    
    col_uint->fill(count);
    col_double->fill(rg.shoot());
    col_float->fill(rbw.shoot());
    if(!tools::num2s(count,stmp)){}
    col_string->fill("str "+stmp);

   {user_vec_double.clear();
    unsigned int number = count%5;
    if(is_large) number = 11;
    for(unsigned int i=0;i<number;i++) {
      user_vec_double.push_back(rg.shoot());
    }}

   {user_vec_int.clear();
    int number = count%5;
    if(is_large) number = 13;
    for(int i=0;i<number;i++) {
      user_vec_int.push_back(i);
    }}

   {user_vec_string.clear();
    std::string _v;
    int number = count%5;
    if(is_large) number = 17;
    for(int i=0;i<number;i++) {
      tools::num2s(i,_v);
      //WARNING : strings must not contain the "sep" and "sep vec" character which are ',' and ';' by default.
      user_vec_string.push_back("string_"+_v);
    }}

    ntu.add_row(); // it will write columns data as a row in the file.
  }

  //////////////////////////////////////////////////////////
  /// close file : /////////////////////////////////////////
  //////////////////////////////////////////////////////////
  writer.close();

  return EXIT_SUCCESS;
}
