// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/sg/plots>

inline void regions_style(tools::sg::plots& a_plots,float a_plotter_scale = 1) {
  // Rescale some plotter parameters (for example margins) according to the number of regions.
  // We assume that these parameters had been set previously according to one plot per page.
  // Then this function must be applied after all the styles had been applied (because
  // a plotting style may set these parameter
  float ww_wc = a_plots.width;
  float wh_wc = a_plots.height;
  float rw_wc = ww_wc/a_plots.cols;
  float rh_wc = wh_wc/a_plots.rows;

  float cooking = 1.2f; //if increased the data area is diminished.

  float wfac = (rw_wc/ww_wc)*cooking;
  float hfac = (rh_wc/wh_wc)*cooking;

  float label_cooking = 1.6f; //if increased the labels are bigger.

  if((a_plots.cols.value()>=4)&&(a_plots.cols.value()>a_plots.rows.value())) label_cooking = 0.9f;

  float title_cooking = 1.1f; //extra title cooking.

  a_plots.plotter_scale = a_plotter_scale;

  std::vector<tools::sg::plotter*> plotters;
  a_plots.plotters(plotters);
  tools_vforcit(tools::sg::plotter*,plotters,it) {
    tools::sg::plotter* _plotter = *it;

    _plotter->left_margin = _plotter->left_margin * wfac;
    _plotter->right_margin = _plotter->right_margin * wfac;
    _plotter->bottom_margin = _plotter->bottom_margin * hfac;
    _plotter->top_margin = _plotter->top_margin * hfac;

    _plotter->x_axis().tick_length = _plotter->x_axis().tick_length * wfac;
    _plotter->y_axis().tick_length = _plotter->y_axis().tick_length * hfac;

    _plotter->title_to_axis = _plotter->title_to_axis * hfac;
    _plotter->title_height = _plotter->title_height * hfac * title_cooking;

    _plotter->x_axis().label_height = _plotter->x_axis().label_height * hfac * label_cooking;
    _plotter->y_axis().label_height = _plotter->y_axis().label_height * hfac * label_cooking;

  }    
}

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

  regions_style(viewer.plots());

  viewer.write(a_file,true); //true = anonymous = no creator, creation date, title in the file.

//#ifdef TOOLS_USE_PNG
//  viewer.write_png("out_1.png");
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
  regions_style(viewer.plots());
  viewer.write_page();

  viewer.close_file();
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

  return true;
}

#include "test_with_file"

#include "../sys/dir"

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

  if(!tools::test_with_file(a_out,a_verbose,"utest_1_",".ps",_remove,
                            utest_viewplot_1_ps,write_out_one_page_two_regions)) return false;
  if(!tools::test_with_file(a_out,a_verbose,"utest_2_",".ps",_remove,
                            utest_viewplot_2_ps,write_out_two_pages)) return false;

  return true;
}
