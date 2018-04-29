// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/histo/h1d>
#include <tools/histo/h2d>
#include "../sys/atime"
#include <tools/mathd>
#include <tools/test>

#include <ostream>

static bool test_h12d(std::ostream& a_out,bool a_verbose) {

 {if(a_verbose) a_out << "test_h12d : h1d ..." << std::endl;
  double tolerance = 1e-8;
  tools::histo::h1d h("title",40,-10,10);
  if(h.title()!="title") return false;
  if(h.axis().bins()!=40) return false;
  if(h.axis().lower_edge()!=-10) return false;
  if(h.axis().upper_edge()!=10) return false;
 {tools::atime start = tools::atime::now();
  for(unsigned int count=0;count<1000;count++) h.fill(0,1.1);
  if(a_verbose) {
    a_out << "test_h12d : h1d.fill(1000) " << tools::atime::elapsed(start) << std::endl;
  }}
  if(h.all_entries()!=1000) return false;
  if(h.bin_entries(20)!=1000) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.bin_Sw(20),1.1*1000,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),0,tolerance,tools::dfabs)) return false;
  h.reset();
  if(h.all_entries()!=0) return false;
  }

 {if(a_verbose) a_out << "test_h12d : h2d ..." << std::endl;
  double tolerance = 1e-8;
  tools::histo::h2d h("title",40,-10,10,20,-5,5);
  if(h.title()!="title") return false;
  if(h.axis_x().bins()!=40) return false;
  if(h.axis_x().lower_edge()!=-10) return false;
  if(h.axis_x().upper_edge()!=10) return false;
  if(h.axis_y().bins()!=20) return false;
  if(h.axis_y().lower_edge()!=-5) return false;
  if(h.axis_y().upper_edge()!=5) return false;
 {tools::atime start = tools::atime::now();
  for(unsigned int count=0;count<2000;count++) h.fill(1,2,1.2);
  if(a_verbose) {
    a_out << "test_h12d : h2d.fill(2000) " << tools::atime::elapsed(start) << std::endl;
  }}
  if(h.all_entries()!=2000) return false;
  if(h.bin_entries(22,14)!=2000) {
    a_out << "test_h12d : " << h.bin_entries(22,14) << std::endl;
    return false;
  }
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.bin_Sw(22,14),1.2*2000,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_x(),1,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_y(),2,tolerance,tools::dfabs)) return false;
  h.reset();
  if(h.all_entries()!=0) return false;
  }

 {if(a_verbose) a_out << "test_h12d : h1d(edges) ..." << std::endl;
  double tolerance = 1e-8;
  std::vector<double> edges(41);
 {for(unsigned int i=0;i<=40;i++) edges[i] = -10+0.5*i;}
  tools::histo::h1d h("title",edges);
  if(h.title()!="title") return false;
  if(h.axis().bins()!=40) return false;
  if(h.axis().lower_edge()!=-10) return false;
  if(h.axis().upper_edge()!=10) return false;
 {tools::atime start = tools::atime::now();
  for(unsigned int count=0;count<1000;count++) h.fill(0,1.1);
  if(a_verbose) {
    a_out << "test_h12d : h1d(edges).fill(1000) " << tools::atime::elapsed(start) << std::endl;
  }}
  if(h.all_entries()!=1000) return false;
  if(h.bin_entries(20)!=1000) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.bin_Sw(20),1.1*1000,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),0,tolerance,tools::dfabs)) return false;
  h.reset();
  if(h.all_entries()!=0) return false;
  }

  return true;
}

#define TOOLS_BYPASS
#ifdef TOOLS_BYPASS
#else //TOOLS_BYPASS

#include <tools/histo/sh1d>
#include <tools/histo/sh2d>

static bool test_sh12d(std::ostream& a_out,bool a_verbose) {

 {if(a_verbose) a_out << "test_sh12d : sh1d ..." << std::endl;
  double tolerance = 1e-8;
  tools::histo::sh1d h("title",40,-10,10);
  if(h.title()!="title") return false;
  if(h.axis().bins()!=40) return false;
  if(h.axis().lower_edge()!=-10) return false;
  if(h.axis().upper_edge()!=10) return false;
 {tools::atime start = tools::atime::now();
  for(unsigned int count=0;count<1000;count++) h.fill(0.1,1.1);
  if(a_verbose) {
    a_out << "test_sh12d : sh1d.fill(1000) " << tools::atime::elapsed(start) << std::endl;
  }}
 {for(unsigned int count=0;count<1000;count++) h.fill(-0.1,1.1);}
  if(h.all_entries()!=2000) return false;
  if(h.bin_entries(20)!=1000) return false;
  if(::fabs(h.bin_Sw(20)-(1.1*1000))>=tolerance) {
    a_out << "test_sh12d : " << h.bin_Sw(20) << std::endl;
    return false;
  }
  if(::fabs(h.mean()-0)>=tolerance) return false;
  h.reset();
  if(h.all_entries()!=0) return false;
  }

 {if(a_verbose) a_out << "test_sh12d : sh2d ..." << std::endl;
  double tolerance = 1e-8;
  tools::histo::sh2d h("title",40,-10,10,20,-5,5);
  if(h.title()!="title") return false;
  if(h.axis_x().bins()!=40) return false;
  if(h.axis_x().lower_edge()!=-10) return false;
  if(h.axis_x().upper_edge()!=10) return false;
  if(h.axis_y().bins()!=20) return false;
  if(h.axis_y().lower_edge()!=-5) return false;
  if(h.axis_y().upper_edge()!=5) return false;
 {tools::atime start = tools::atime::now();
  for(unsigned int count=0;count<2000;count++) h.fill(1,2,1.2);
  if(a_verbose) {
    a_out << "test_sh12d : sh2d.fill(2000) " << tools::atime::elapsed(start) << std::endl;
  }}
  if(h.all_entries()!=2000) return false;
  if(h.bin_entries(22,14)!=2000) {
    a_out << "test_sh12d : " << h.bin_entries(22,14) << std::endl;
    return false;
  }
  if(::fabs(h.bin_Sw(22,14)-(1.2*2000))>=tolerance) {
    a_out << "test_sh12d : " << h.bin_Sw(22,14) << std::endl;
    return false;
  }
  if(::fabs(h.mean_x()-1)>=tolerance) return false;
  if(::fabs(h.mean_y()-2)>=tolerance) return false;
  h.reset();
  if(h.all_entries()!=0) return false;
  }

  return true;
}

#endif //TOOLS_BYPASS

#include <tools/histo/p1d>
#include <tools/histo/sliced>
#include "../sys/atime"

#include <tools/randd>

template <class H1D>
bool test_h1d_rand(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) a_out << "test_h1d_rand : ..." << std::endl;
  unsigned int entries = 2000000;
  tools::rgaussd rg(1,0.5);
  H1D h("Gauss",100,-5,5);
 {for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),1.4);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),1,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),0.5,1e-3,tools::dfabs)) return false;}
  h.reset();
 {for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot()-1.0,2.4);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),0,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),0.5,1e-3,tools::dfabs)) return false;}
  return true;
}

static bool test_p1d_rand(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) a_out << "test_p1d_rand : ..." << std::endl;
  unsigned int entries = 2000000;
  tools::rgaussd rgx(1,0.5);
  tools::rgaussd rgy(2,0.4);
  tools::histo::p1d h("Profile",100,-5,5);
 {for(unsigned int count=0;count<entries;count++) h.fill(rgx.shoot(),rgy.shoot(),1);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),1,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),0.5,1e-3,tools::dfabs)) return false;}
  h.reset();
 {for(unsigned int count=0;count<entries;count++) h.fill(rgx.shoot()-1,rgy.shoot(),1);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),0,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),0.5,1e-3,tools::dfabs)) return false;}
  //if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.bin_rms_value(60),0.4,1e-3,tools::dfabs)) return false;
  return true;
}

template <class H2D>
bool test_h2d_rand(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) a_out << "test_h2d_rand : ..." << std::endl;
  unsigned int entries = 2000000;
  tools::rgaussd rgx(1,0.5);
  tools::rgaussd rgy(0.5,0.2);
  H2D h("Gauss_Gauss",100,-5,5,200,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rgx.shoot(),rgy.shoot(),0.8);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_x(),1,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_x(),0.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_y(),0.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_y(),0.2,1e-3,tools::dfabs)) return false;
  return true;
}

static bool test_h2d_proj(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) a_out << "test_h2d_proj : ..." << std::endl;
  unsigned int entries = 2000000;

  tools::rgaussd rgx(1,0.5);
  tools::rgaussd rgy(0.5,0.2);
  tools::histo::h2d h("Gauss_Gauss",100,-5,5,200,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rgx.shoot(),rgy.shoot(),0.8);

 {tools::histo::h1d* pj = tools::histo::projection_x(h,"ProjX");
  if(!pj) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->mean(),1,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->rms(),0.5,1e-3,tools::dfabs)) return false;
  delete pj;}
   
 {tools::histo::h1d* pj = tools::histo::projection_y(h,"ProjY");
  if(!pj) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->mean(),0.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->rms(),0.2,1e-3,tools::dfabs)) return false;
  delete pj;}

 {tools::histo::h1d* slice = tools::histo::slice_x(h,90,160,"SliceX");
  if(!slice) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,slice->mean(),1,1e-2,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,slice->rms(),0.5,1e-2,tools::dfabs)) return false;
  delete slice;}
 /*
 {tools::histo::h1d* slice = tools::histo::slice_x(h,tools::histo::axis_UNDERFLOW_BIN,tools::histo::axis_UNDERFLOW_BIN,"SliceX");
  if(!slice) return false;
  delete slice;}

 {tools::histo::h1d* slice = tools::histo::slice_x(h,tools::histo::axis_OVERFLOW_BIN,tools::histo::axis_OVERFLOW_BIN,"SliceX");
  if(!slice) return false;
  delete slice;}

 {tools::histo::h1d* slice = tools::histo::slice_y(h,30,50,"SliceY");
  if(!slice) return false;
  delete slice;}

 {using namespace tools::histo; //playing with namespaces.
  h1d* slice = slice_y(h,axis_UNDERFLOW_BIN,axis_UNDERFLOW_BIN,"SliceY");
  if(!slice) return false;
  delete slice;}

 {namespace tools = tools::histo; //playing with namespaces.
  tools::h1d* slice = slice_y(h,tools::axis_OVERFLOW_BIN,tools::axis_OVERFLOW_BIN,"SliceY");
  if(!slice) return false;
  delete slice;}
 */


 {tools::histo::p1d* _p = tools::histo::profile_x(h,"ProfileX");
  if(!_p) return false;
  //  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->mean(),1,1e-3)) return false;
  //  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->rms(),0.5,1e-3)) return false;
  delete _p;}
   
 {tools::histo::p1d* _p = tools::histo::profile_y(h,"ProfileY");
  if(!_p) return false;
  //  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->mean(),0.5,1e-3)) return false;
  //  if(!tools::equal<double>(a_out,__FILE__,__LINE__,pj->rms(),0.2,1e-3)) return false;
  delete _p;}

  return true;
}

static bool test_h3d_rand(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) a_out << "test_h3d_rand : ..." << std::endl;
  unsigned int entries = 2000000;

  tools::rgaussd rgx(1,0.5);
  tools::rgaussd rgy(0.5,0.2);
  tools::rgaussd rgz(1.5,0.3);
  tools::histo::h3d h("Gauss_Gauss_flat",100,-10,10,100,-2,2,100,-3,3);
  for(unsigned int count=0;count<entries;count++) h.fill(rgx.shoot(),rgy.shoot(),rgz.shoot());
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_x(),1,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_x(),0.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_y(),0.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_y(),0.2,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_z(),1.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_z(),0.3,1e-3,tools::dfabs)) return false;

  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
 {tools::histo::h2d* projection = tools::histo::projection_xy(h,"ProjXY");
  if(!projection) return false;
    
 {tools::histo::h1d* _pj = tools::histo::projection_x(*projection,"ProjX");
  if(!_pj) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,_pj->mean(),1.0,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,_pj->rms(),0.5,1e-3,tools::dfabs)) return false;
  delete _pj;}

 {tools::histo::h1d* _pj = tools::histo::projection_y(*projection,"ProjY");
  if(!_pj) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,_pj->mean(),0.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,_pj->rms(),0.2,1e-3,tools::dfabs)) return false;
  delete _pj;}

  delete projection;}
   
  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
 {tools::histo::h2d* projection = tools::histo::projection_yz(h,"ProjYZ");
  if(!projection) return false;
    
 {tools::histo::h1d* _pj = tools::histo::projection_y(*projection,"ProjZ");
  if(!_pj) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,_pj->mean(),1.5,1e-3,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,_pj->rms(),0.3,1e-3,tools::dfabs)) return false;
  delete _pj;}

  delete projection;}
   
  return true;
}

#include <tools/histo/htoc>

static bool test_htoc(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) a_out << "test_htoc : ..." << std::endl;

  double tolerance = 1e-8;

 {if(a_verbose) a_out << "test_htoc : h1d ..." << std::endl;
  tools::histo::h1d h("h1d",40,-10,10);
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"1.4*entries");
  tools::rgaussd rg(1,0.5);
 {for(unsigned int count=0;count<10000;count++) h.fill(rg.shoot(),1.4);}

  chisto_duiuid* ch = tools::histo::chisto_duiuid_alloc(h.dac());
  if(!ch) {a_out << "chisto_duiuid_alloc() failed." << std::endl;return false;}
  tools::histo::h1d::hd_t hdata;
  tools::histo::histo_data_duiuid_assign(hdata,*ch);
  tools::histo::chisto_duiuid_free(ch);
  tools::histo::h1d hc("",10,0,1);
  hc.copy_from_data(hdata);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),hc.mean(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),hc.rms(),tolerance,tools::dfabs)) return false;
  }

 {if(a_verbose) a_out << "test_htoc : h2d ..." << std::endl;
  tools::histo::h2d h("h2d",40,-10,10,20,-5,5);
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"rand bw");
  h.add_annotation(tools::histo::key_axis_z_title(),"1.4*entries");
  tools::rgaussd rg(1,0.5);
 {for(unsigned int count=0;count<10000;count++) h.fill(rg.shoot(),rg.shoot(),1.4);}

  chisto_duiuid* ch = tools::histo::chisto_duiuid_alloc(h.dac());
  if(!ch) {a_out << "chisto_duiuid_alloc() failed." << std::endl;return false;}
  tools::histo::h2d::hd_t hdata;
  tools::histo::histo_data_duiuid_assign(hdata,*ch);
  tools::histo::chisto_duiuid_free(ch);
  tools::histo::h2d hc("",10,0,1,10,0,1);
  hc.copy_from_data(hdata);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_x(),hc.mean_x(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_x(),hc.rms_x(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean_y(),hc.mean_y(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms_y(),hc.rms_y(),tolerance,tools::dfabs)) return false;
  }

 {if(a_verbose) a_out << "test_htoc : p1d ..." << std::endl;
  tools::rgaussd rgx(1,0.5);
  tools::rgaussd rgy(2,0.4);
  tools::histo::p1d h("p1d",100,-5,5,1,3);
  {for(unsigned int count=0;count<20000;count++) h.fill(rgx.shoot(),rgy.shoot(),1);}

  cprofile_duiuidd* cp = tools::histo::cprofile_duiuidd_alloc(h.get_histo_data());
  if(!cp) {a_out << "cprofile_duiuidd_alloc() failed." << std::endl;return false;}
  tools::histo::p1d::pd_t pdata;
  tools::histo::profile_data_duiuidd_assign(pdata,*cp);
  tools::histo::cprofile_duiuidd_free(cp);
  tools::histo::p1d pc("",10,0,1);
  pc.copy_from_data(pdata);
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),pc.mean(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),pc.rms(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.get_Svw(),pc.get_Svw(),tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.get_Sv2w(),pc.get_Sv2w(),tolerance,tools::dfabs)) return false;
  }

  return true;
}

bool test_histo(std::ostream& a_out,bool a_verbose) {
  if(!test_h12d(a_out,a_verbose)) return false;
  if(!test_h1d_rand<tools::histo::h1d>(a_out,a_verbose)) return false;
  if(!test_p1d_rand(a_out,a_verbose)) return false;
  if(!test_h2d_rand<tools::histo::h2d>(a_out,a_verbose)) return false;
  if(!test_h2d_proj(a_out,a_verbose)) return false;
  if(!test_h3d_rand(a_out,a_verbose)) return false;
  if(!test_htoc(a_out,a_verbose)) return false;
#ifdef TOOLS_BYPASS
#else //TOOLS_BYPASS
  if(!test_sh12d(a_out,a_verbose)) return false;
  if(!test_h1d_rand<tools::histo::sh1d>(a_out,a_verbose)) return false;
  if(!test_h2d_rand<tools::histo::sh2d>(a_out,a_verbose)) return false;
#endif  
  return true;
}
