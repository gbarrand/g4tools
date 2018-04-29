// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/columns>
#include <tools/test>

bool test_columns(std::ostream& a_out,bool /*a_verbose*/) {

 {
  //std::string in = "double x = 1,ITuple y = {bool z,float uuu},int z";
  //std::string in = "double x = 1,ITuple y = {bool z,ITuple z = {string u = \"aaa\"},float uuu},int z";
  //std::string in = "uint index,double rgauss,ITuple ntu = {double rbw,short short}";
  std::string in = "double x = 1,ITuple y = {bool z,ITuple z = {string u = \"aaa\"},float uuu},int z";

  tools::columns::parser pr;
  if(!tools::equal(a_out,__FILE__,__LINE__,pr.parse(in),true)) return false;
  //pr.dump(std::cout);

  tools::columns::finder f(a_out,in);
  if(!tools::equal(a_out,__FILE__,__LINE__,f.find_variables(),true)) return false;
  std::vector<tools::value> vars;
  f.result(vars);
  if(!tools::equal(a_out,__FILE__,__LINE__,vars.size(),size_t(3))) return false;
  //tools::columns::dump_columns(a_out,vars);

 {const tools::value& _val = vars[0];
  std::string stype;        
  _val.s_type(stype);
  std::string sval;        
  _val.tos(sval);
  std::string s = stype + " : " + _val.label() + " : " + sval;
  if(!tools::equal(a_out,__FILE__,__LINE__,s,"DOUBLE : x : 1")) return false;}

 {const tools::value& _val = vars[1];
  std::string stype;        
  _val.s_type(stype);
  std::string s = stype + " : " + _val.label();
  if(!tools::equal(a_out,__FILE__,__LINE__,s,"VOID_STAR : y")) return false;}

 {const tools::value& _val = vars[2];
  std::string stype;        
  _val.s_type(stype);
  std::string sval;        
  _val.tos(sval);
  std::string s = stype + " : " + _val.label() + " : " + sval;
  if(!tools::equal(a_out,__FILE__,__LINE__,s,"INT : z : 0")) return false;}

  tools::columns::delete_columns(vars);}

 {// MEMPHSY sim :
  std::string in = "int eventId, inputEvtId, interMode, vtxVol, "; 
  in += "ITuple vtxPos = { double x, y, z }, ";
  in += "int nPart, leptonIndex, protonIndex, ";
  in += "ITuple track = { int pId, parent, float timeStart, ";
  in +=                  "ITuple direction = { double dx, dy, dz }, ";
  in +=                  "double mass, pTot, ETot, ";
  in +=                  "ITuple momentum  = { double px, py, pz }, ";
  in +=                  "ITuple startPos  = { double  x,  y,  z }, ";
  in +=                  "ITuple stopPos   = { double  x,  y,  z }, ";
  in +=                  "int startVol, stopVol ";
  in +=                "}, ";
  in += "int nHits, ITuple hit = { int tubeId, int totalPE, ITuple pe = { float time} }, ";
  in += "int nDigits, double sumPE, ";
  in += "ITuple digit = { int tubeId, double pe, time }";
 
  tools::columns::parser pr;
  if(!tools::equal(a_out,__FILE__,__LINE__,pr.parse(in),true)) return false;
  //pr.dump(std::cout);

  tools::columns::finder f(a_out,in);
  if(!tools::equal(a_out,__FILE__,__LINE__,f.find_variables(),true)) return false;
  std::vector<tools::value> vars;
  f.result(vars);
  if(!tools::equal(a_out,__FILE__,__LINE__,vars.size(),size_t(14))) return false;
  tools::columns::delete_columns(vars);}

  return true;
}
