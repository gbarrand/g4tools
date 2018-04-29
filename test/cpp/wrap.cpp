// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/srep>
#include <tools/num2s>
#include <tools/file>

#include <cstdlib> //EXIT_SUCCESS

#include <iostream>

int main(int a_argc,char** a_argv) {
  std::vector<std::string> args;
 {for(int index=0;index<a_argc;index++) args.push_back(a_argv[index]);}
  
  if(args.size()!=3) {
    std::cout << "Two arguments expected." << std::endl;
    return EXIT_FAILURE;
  }

  std::string in_file = args[1];
  std::string out_file = args[2];

  if(in_file==out_file) {
    std::cout << "out file is the same than in file." << std::endl;
    return EXIT_FAILURE;
  }
  
  std::vector<std::string> in_txt;
  if(!tools::file::read(in_file,in_txt)) {
    std::cout << "Can't read input file \"" << in_file << "\"." << std::endl;
    return EXIT_FAILURE;
  }

  std::string s_in_file = tools::base_name(in_file);
  std::string s_out_file = tools::base_name(out_file);

  std::vector<std::string> out_txt;
  out_txt.push_back("#ifndef "+s_out_file);
  out_txt.push_back("#define "+s_out_file);
  out_txt.push_back("");
  out_txt.push_back("//");
  out_txt.push_back("//  This file had been produced automatically");
  out_txt.push_back("// by the inlib/wrap program from the file :");
  out_txt.push_back("//    "+s_in_file);
  out_txt.push_back("//");

  tools::replace(s_in_file,".","_");
  tools::replace(s_in_file,"-","_");

  out_txt.push_back("");
  out_txt.push_back("inline const char** "+s_in_file+"(unsigned int& a_linen){");

 {size_t number = in_txt.size();
  for(unsigned int index=0;index<number;index++) {
    std::string s = "  static const char ";
    s += "_";
    if(!tools::numas(index,s)) {}
    s += "[] = \"";
    std::string ss = in_txt[index];
    tools::replace(ss,"\"","@@double_quote@@");
    tools::replace(ss,"\\","@@back_slash@@");
    s += ss;
    s += "\";";
    out_txt.push_back(s);
  }}

 {out_txt.push_back("  static const char* _lines[] = {");
  size_t number = in_txt.size();
  for(unsigned int index=0;index<number;index++) {
    std::string s;
    if(index) s += ",";
    s += "_";
    if(!tools::numas(index,s)) {}
    out_txt.push_back(s);
  }
  out_txt.push_back("};");}

  std::string stmp;
  if(!tools::num2s((tools::uint64)in_txt.size(),stmp)){}
  out_txt.push_back(" a_linen = "+stmp+";");
  out_txt.push_back(" return _lines;");
  out_txt.push_back("}");
  out_txt.push_back("");
  out_txt.push_back("#endif");

  if(!tools::file::write(out_file,out_txt)) {
    std::cout << "Can't write output file \"" << out_file << "\"." << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
