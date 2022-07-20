// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

//inlib_build_bigobj

#include <tools/sg/plots>

#include <tools/random>
#include <tools/viewplot>

#include <tools/test>

inline bool write_out_one_page_two_regions(std::ostream& a_out,bool a_verbose,const std::string& a_file){
  if(a_verbose) a_out << "write_out_one_page_two_regions ..." << std::endl;

  //////////////////////////////////////////////////////////
  /// create and fill histogram : //////////////////////////
  //////////////////////////////////////////////////////////
  tools::histo::h1d h1("Random Gauss",100,-5,5);
  tools::random::gauss rg(0,1);
 {for(unsigned int index=0;index<10000;index++) h1.fill(rg.shoot(),1);}

  tools::random::bw rbw(0,1);

  tools::histo::h2d h2("BW/Gauss",100,-5,5,100,-5,5);
 {for(unsigned int index=0;index<10000;index++) h2.fill(rg.shoot(),rbw.shoot(),1);}

  ////////////////////////////////////////////////////////////////////////////
  /// plotting, low resolution with Hershey fonts and default style : ////////
  ////////////////////////////////////////////////////////////////////////////
  //Have vertical A4 :
  unsigned int ww = 700;
  float A4 = 29.7f/21.0f;
  unsigned int wh = (unsigned int)(float(ww)*A4);
  tools::viewplot viewer(a_out,1,2,ww,wh); //cols=1,rows=2 then width and height

  viewer.plots().view_border = false;

  ///////////////////////////////////////////
  /// one page with two plots : /////////////
  ///////////////////////////////////////////
  // up area :
 {viewer.plot(h1);
  tools::sg::plotter& plotter = viewer.plots().current_plotter();
  plotter.bins_style(0).color = tools::colorf_blue();}

  // down area :
 {viewer.plots().next();
  viewer.plot(h2);
  tools::sg::plotter& plotter = viewer.plots().current_plotter();
  plotter.bins_style(0).color = tools::colorf_red();}

  viewer.plots().adjust_scales();

  viewer.write(a_file,true); //true = anonymous = no creator, creation date, title in the file.

//#ifdef TOOLS_USE_PNG
//  viewer.write_inzb_png("out_1.png");
//#endif

  return true;
}

inline bool write_out_two_pages(std::ostream& a_out,bool a_verbose,const std::string& a_file) {
  if(a_verbose) a_out << "write_out_two_pages ..." << std::endl;

  //////////////////////////////////////////////////////////
  /// create and fill histogram : //////////////////////////
  //////////////////////////////////////////////////////////
  tools::histo::h1d h1("Random Gauss",100,-5,5);
  tools::random::gauss rg(0,1);
 {for(unsigned int index=0;index<10000;index++) h1.fill(rg.shoot(),1);}

  tools::random::bw rbw(0,1);

  tools::histo::p1d p1("Random Gauss",100,-5,5,-5,5);
 {for(unsigned int index=0;index<10000;index++) p1.fill(rg.shoot(),rbw.shoot());}

  tools::histo::h2d h2("BW/Gauss",100,-5,5,100,-5,5);
 {for(unsigned int index=0;index<10000;index++) h2.fill(rg.shoot(),rbw.shoot(),1);}

  ////////////////////////////////////////////////////////////////////////////
  /// plotting, low resolution with Hershey fonts and default style : ////////
  ////////////////////////////////////////////////////////////////////////////
  //Have vertical A4 :
  unsigned int ww = 700;
  float A4 = 29.7f/21.0f;
  unsigned int wh = (unsigned int)(float(ww)*A4);
  tools::viewplot viewer(a_out,1,2,ww,wh); //cols=1,rows=2 then width and height

  viewer.plots().view_border = false;

  //////////////////////////////////////////////////////////
  /// two pages with one plot per page : ///////////////////
  //////////////////////////////////////////////////////////
  viewer.open_file(a_file,true); //true = anonymous = no creator, creation date, title in the file.

  viewer.set_cols_rows(1,1);
  viewer.plot(h1);
  viewer.write_page();

  viewer.set_cols_rows(1,2);
  viewer.plots().clear();
  viewer.plot(p1);
  viewer.plots().next();
  viewer.plot(h2);
  viewer.plots().adjust_scales();
  viewer.write_page();

  viewer.close_file();
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

  return true;
}

#include "test_with_file"

#include <tools/sys/dir>

#include "wrap_utest_viewplot_1_ps"
#include "wrap_utest_viewplot_2_ps"

bool test_viewplot(std::ostream& a_out,bool a_verbose) {
  //bool a_verbose = a_args.is_arg(tools::s_arg_verbose());

  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  //bool _remove = false;
  bool _remove = true;

  bool stop_if_failed = true;

  if(!tools::test_with_file(a_out,a_verbose,"utest_1_",".ps",_remove,
                            utest_viewplot_1_ps,write_out_one_page_two_regions) && stop_if_failed) return false;
  if(!tools::test_with_file(a_out,a_verbose,"utest_2_",".ps",_remove,
                            utest_viewplot_2_ps,write_out_two_pages) && stop_if_failed) return false;

  return true;
}
