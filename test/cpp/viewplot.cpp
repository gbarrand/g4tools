// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/sg/plots>

inline void HD_style(tools::sg::plots& a_plots,float a_line_width) {
  std::vector<tools::sg::plotter*> plotters;
  a_plots.plotters(plotters);
  tools_vforcit(tools::sg::plotter*,plotters,it) {
    tools::sg::plotter* _plotter = *it;
    _plotter->bins_style(0).line_width = a_line_width;
    _plotter->inner_frame_style().line_width = a_line_width;
    _plotter->grid_style().line_width = a_line_width;
    _plotter->x_axis().line_style().width = a_line_width;
    _plotter->x_axis().ticks_style().width = a_line_width;
    _plotter->y_axis().line_style().width = a_line_width;
    _plotter->y_axis().ticks_style().width = a_line_width;
    _plotter->z_axis().line_style().width = a_line_width;
    _plotter->z_axis().ticks_style().width = a_line_width;

    // needed if font is hershey :
    _plotter->title_style().line_width = a_line_width;
    _plotter->infos_style().line_width = a_line_width;
    _plotter->title_box_style().line_width = a_line_width;

    _plotter->x_axis().labels_style().line_width = a_line_width;
    _plotter->x_axis().mag_style().line_width = a_line_width;
    _plotter->x_axis().title_style().line_width = a_line_width;

    _plotter->y_axis().labels_style().line_width = a_line_width;
    _plotter->y_axis().mag_style().line_width = a_line_width;
    _plotter->y_axis().title_style().line_width = a_line_width;

    _plotter->z_axis().labels_style().line_width = a_line_width;
    _plotter->z_axis().mag_style().line_width = a_line_width;
    _plotter->z_axis().title_style().line_width = a_line_width;
  }    
}

/*
inline void border_style(tools::sg::plots& a_plots) {
  std::vector<tools::sg::plotter*> plotters;
  a_plots.plotters(plotters);
  tools_vforcit(tools::sg::plotter*,plotters,it) {
    tools::sg::plotter* _plotter = *it;
    _plotter->background_style().visible = true;
    _plotter->background_style().color = tools::colorf_black();
    _plotter->background_style().line_width = 0.003;
  }    
  a_plots.border_visible = true;
}
*/

inline void grid_style(tools::sg::plots& a_plots,bool a_visible = false) {
  std::vector<tools::sg::plotter*> plotters;
  a_plots.plotters(plotters);
  tools_vforcit(tools::sg::plotter*,plotters,it) {
    tools::sg::plotter* _plotter = *it;
    _plotter->grid_style().visible = a_visible;
  }    
  a_plots.border_visible = true;
}

inline void regions_style(tools::sg::plots& a_plots,float a_plotter_scale = 1) {
  // Rescale some plotter parameters (for example margins) according to the number of regions.
  // We assume that these parameters had been set previously according to one plot per page.
  // Then this function must be applied after all the styles had been applied (because
  // a plotting style may set these parameters).

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

#include <tools/viewplot>

void plot_file(tools::viewplot& a_viewer,
               const std::string& a_filename,
               unsigned int a_columns, unsigned int a_rows, float a_plotter_scale,
               const std::string& a_style,
               int a_n,tools::histo::h1d& a_h1,bool a_verbose = false, bool use_HD = true) 
{
  if(a_verbose)
    a_viewer.out() << "plot file : file name " << a_filename 
                   << ", " << a_n << " plots in " << a_columns << "x" << a_rows << ", layout "
  	           << a_style << " ..." << std::endl;

  a_viewer.open_file(a_filename);

  a_viewer.plots().init_sg(); //it will recreate the sg::plotters and then reset the styles on new ones.
  a_viewer.set_cols_rows(a_columns,a_rows);
  a_viewer.plots().set_current_plotter(0);

  unsigned int plots_per_page = a_columns * a_rows;
  bool isWriteNeeded = false;

  for (int i=0; i<a_n; i++) {
    a_viewer.plot(a_h1);
    a_viewer.set_current_plotter_style(a_style);
    isWriteNeeded = true;

    if( a_viewer.plots().current_index() == (plots_per_page-1) ) {
      if ( use_HD ) HD_style(a_viewer.plots(),5);
      grid_style(a_viewer.plots());
      regions_style(a_viewer.plots(),a_plotter_scale);
      a_viewer.write_page();

      a_viewer.plots().init_sg(); //it will recreate the sg::plotters and then reset the styles on new ones.

      isWriteNeeded = false;
    }
    a_viewer.plots().next();
  }  

  if ( isWriteNeeded ) {
    if ( use_HD ) HD_style(a_viewer.plots(),5);
    grid_style(a_viewer.plots());
    regions_style(a_viewer.plots(),a_plotter_scale);
    a_viewer.write_page();
  }

  a_viewer.close_file();
}  

#ifdef TOOLS_USE_FREETYPE
#include <tools/sg/text_freetype>
#endif

#ifdef TOOLS_USE_EXPAT
#include <tools/xml/xml_style>
#include <tools/xml/wrap_viewplot_style> // xml/viewplot.style file embeded in an inline function.
inline bool load_embeded_styles(tools::xml::styles& a_styles) {
  std::string ss;
  unsigned int linen;
  const char** lines = viewplot_style(linen);
  for(unsigned int index=0;index<linen;index++) {
    std::string s = lines[index];
    tools::replace(s,"@@double_quote@@","\"");
    tools::replace(s,"@@back_slash@@","\\");
    ss += s + "\n";
  }
  return tools::xml::load_style_string(a_styles,ss);
}
#endif

#include <tools/random>
#include <tools/args>
#include <tools/sargs>

#include <iostream>
#include <cstdlib>

#if defined(TOOLS_MEM) && defined(TOOLS_MEM_ATEXIT)
inline void at_exit() {std::cout << "at_exit..."<< std::endl;tools::mem::balance(std::cout);}
#endif

int main(int argc,char** argv) {

#ifdef TOOLS_MEM
  #ifdef TOOLS_MEM_ATEXIT
  ::atexit(at_exit);
  #endif  
  tools::mem::set_check_by_class(true);{
#endif

  tools::args args(argc,argv);
  bool verbose = args.is_arg(tools::s_arg_verbose());

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

 {////////////////////////////////////////////////////////////////////////////
  /// plotting, low resolution with Hershey fonts and default style : ////////
  ////////////////////////////////////////////////////////////////////////////
  //Have vertical A4 :
  unsigned int ww = 700;
  float A4 = 29.7f/21.0f;
  unsigned int wh = (unsigned int)(float(ww)*A4);
  tools::viewplot viewer(std::cout,1,2,ww,wh); //cols=1,rows=2 then width and height

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

  viewer.plots().view_border = false;
  regions_style(viewer.plots());

  viewer.write("out_one_page_two_regions_hershey_font.ps");

  //////////////////////////////////////////////////////////
  /// two pages with one plot per page : ///////////////////
  //////////////////////////////////////////////////////////
  viewer.open_file("out_two_pages_hershey_font.ps");

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
  /// Ivana tests : ////////////////////////////////////////
  /// supported layouts in analysis ////////////////////////
  //////////////////////////////////////////////////////////
  bool use_HD = false;
  plot_file(viewer, "out_1x1_nf.ps", 1, 1,    1, "inlib_default",  3, h1, verbose, use_HD);
  plot_file(viewer, "out_1x2_nf.ps", 1, 2,    1, "inlib_default",  5, h1, verbose, use_HD);
  plot_file(viewer, "out_1x3_nf.ps", 1, 3,    1, "inlib_default",  8, h1, verbose, use_HD);
  plot_file(viewer, "out_2x2_nf.ps", 2, 2,    1, "inlib_default", 10, h1, verbose, use_HD);
  plot_file(viewer, "out_2x3_nf.ps", 2, 3,    1, "inlib_default", 15, h1, verbose, use_HD);
  plot_file(viewer, "out_3x3_nf.ps", 3, 3,    1, "inlib_default", 23, h1, verbose, use_HD);

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
 }

#if defined(TOOLS_USE_FREETYPE) && defined(TOOLS_USE_EXPAT)
 {//////////////////////////////////////////////////////////////////////////////
  /// plotting, high resolution with freetype fonts and by using styles : //////
  //////////////////////////////////////////////////////////////////////////////
  //Have vertical A4 :
  unsigned int ww = 2000; //to have better antialising on freetype fonts.
  float A4 = 29.7f/21.0f;
  unsigned int wh = (unsigned int)(float(ww)*A4);

  tools::sg::text_freetype ttf;
  tools::viewplot viewer(std::cout,ttf,1,1,ww,wh); //cols=1,rows=2 then width and height
  viewer.set_current_plotter_style("xxx"/*,true*/); //to check that verbosity if off if xxx is not found.

  viewer.plots().view_border = false;

  //if(tools::file::exists("viewplot.style")) {
  //  tools::xml::load_style_file(viewer.styles(),"viewplot.style");
  //} else {
  load_embeded_styles(viewer.styles());
  //}

  viewer.styles().add_colormap("default",tools::sg::style_default_colormap());
  viewer.styles().add_colormap("ROOT",tools::sg::style_ROOT_colormap());

  //////////////////////////////////////////////////////////
  /// two pages with one plot per page : ///////////////////
  //////////////////////////////////////////////////////////
  viewer.open_file("out_two_pages_freetype_font.ps");

  viewer.set_cols_rows(1,1);
  viewer.plot(h1);
  viewer.set_current_plotter_style("ROOT_default");
  HD_style(viewer.plots(),5);
  viewer.write_page();

  viewer.set_cols_rows(1,2);
  viewer.plots().clear();
  viewer.plot(p1);
  viewer.set_current_plotter_style("hippodraw");
  viewer.plots().next();
  viewer.plot(h2); 
  viewer.set_current_plotter_style("hippodraw");
  HD_style(viewer.plots(),5);
  regions_style(viewer.plots());
  viewer.write_page();

  viewer.close_file();

  //////////////////////////////////////////////////////////////////////////////
  /// plotting 2x2 : ///////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  viewer.open_file("out_2x2.ps");
  viewer.plots().set_current_plotter(0);

  viewer.set_cols_rows(2,3);
  viewer.plots().clear();

  viewer.plot(h1);
  viewer.set_current_plotter_style("ROOT_default");
  HD_style(viewer.plots(),5);

  viewer.plots().next();
  viewer.plot(p1); 
  viewer.set_current_plotter_style("ROOT_default");
  HD_style(viewer.plots(),5);

  viewer.plots().next();
  viewer.plot(p1); 
  viewer.set_current_plotter_style("ROOT_default");
  HD_style(viewer.plots(),5);

  viewer.plots().next();
  viewer.plot(h1); 
  viewer.set_current_plotter_style("ROOT_default");
  HD_style(viewer.plots(),5);

  regions_style(viewer.plots());
  viewer.write_page();

  viewer.close_file();

  //////////////////////////////////////////////////////////
  /// Ivana tests : ////////////////////////////////////////
  /// supported layouts in analysis ////////////////////////
  //////////////////////////////////////////////////////////

  plot_file(viewer, "out_1x1_r.ps", 1, 1, 0.9f, "ROOT_default",   3, h1, verbose);
  plot_file(viewer, "out_1x1_h.ps", 1, 1,    1, "hippodraw",      3, h1, verbose);
  plot_file(viewer, "out_1x1_d.ps", 1, 1,    1, "inlib_default",  3, h1, verbose);
 
  plot_file(viewer, "out_1x2_r.ps", 1, 2, 0.9f, "ROOT_default",   5, h1, verbose);
  plot_file(viewer, "out_1x2_h.ps", 1, 2,    1, "hippodraw",      5, h1, verbose);
  plot_file(viewer, "out_1x2_d.ps", 1, 2,    1, "inlib_default",  5, h1, verbose);
 
  plot_file(viewer, "out_1x3_r.ps", 1, 3, 0.9f, "ROOT_default",   8, h1, verbose);
  plot_file(viewer, "out_1x3_h.ps", 1, 3,    1, "hippodraw",      8, h1, verbose);
  plot_file(viewer, "out_1x3_d.ps", 1, 3,    1, "inlib_default",  8, h1, verbose);
 
  plot_file(viewer, "out_2x2_r.ps", 2, 2, 0.9f, "ROOT_default",  10, h1, verbose);
  plot_file(viewer, "out_2x2_h.ps", 2, 2,    1, "hippodraw",     10, h1, verbose);
  plot_file(viewer, "out_2x2_d.ps", 2, 2,    1, "inlib_default", 10, h1, verbose);
 
  plot_file(viewer, "out_2x3_r.ps", 2, 3, 0.9f, "ROOT_default",  15, h1, verbose);
  plot_file(viewer, "out_2x3_h.ps", 2, 3,    1, "hippodraw",     15, h1, verbose);
  plot_file(viewer, "out_2x3_d.ps", 2, 3,    1, "inlib_default", 15, h1, verbose);
 
  plot_file(viewer, "out_2x4_r.ps", 2, 4, 0.9f, "ROOT_default",  20, h1, verbose);
  plot_file(viewer, "out_2x4_h.ps", 2, 4,    1, "hippodraw",     20, h1, verbose);
  plot_file(viewer, "out_2x4_d.ps", 2, 4,    1, "inlib_default", 20, h1, verbose);
 
  plot_file(viewer, "out_3x3_r.ps", 3, 3, 0.9f, "ROOT_default",  23, h1, verbose);
  plot_file(viewer, "out_3x3_h.ps", 3, 3,    1, "hippodraw",     23, h1, verbose);
  plot_file(viewer, "out_3x3_d.ps", 3, 3,    1, "inlib_default", 23, h1, verbose);
 
  plot_file(viewer, "out_3x4_r.ps", 3, 4, 0.9f, "ROOT_default",  30, h1, verbose);
  plot_file(viewer, "out_3x4_h.ps", 3, 4,    1, "hippodraw",     30, h1, verbose);
  plot_file(viewer, "out_3x4_d.ps", 3, 4,    1, "inlib_default", 30, h1, verbose);
 
  plot_file(viewer, "out_3x5_r.ps", 3, 5, 0.9f, "ROOT_default",  37, h1, verbose);
  plot_file(viewer, "out_3x5_h.ps", 3, 5,    1, "hippodraw",     37, h1, verbose);
  plot_file(viewer, "out_3x5_d.ps", 3, 5,    1, "inlib_default", 37, h1, verbose);
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  plot_file(viewer, "out_3x2_r.ps", 3, 2, 0.9f, "ROOT_default",  15, h1, verbose);
  plot_file(viewer, "out_3x2_h.ps", 3, 2,    1, "hippodraw",     15, h1, verbose);
  plot_file(viewer, "out_3x2_d.ps", 3, 2,    1, "inlib_default", 15, h1, verbose);

  plot_file(viewer, "out_4x2_r.ps", 4, 2, 0.9f, "ROOT_default",  20, h1, verbose);
  plot_file(viewer, "out_4x2_h.ps", 4, 2,    1, "hippodraw",     20, h1, verbose);
  plot_file(viewer, "out_4x2_d.ps", 4, 2,    1, "inlib_default", 20, h1, verbose);

  plot_file(viewer, "out_4x3_r.ps", 4, 3, 0.9f, "ROOT_default",  30, h1, verbose);
  plot_file(viewer, "out_4x3_h.ps", 4, 3,    1, "hippodraw",     30, h1, verbose);
  plot_file(viewer, "out_4x3_d.ps", 4, 3,    1, "inlib_default", 30, h1, verbose);
 }
#endif

#ifdef TOOLS_MEM
  }
  #ifndef TOOLS_MEM_ATEXIT
  tools::mem::balance(std::cout);
  #endif
  std::cout << "exit..." << std::endl;
#endif

  return EXIT_SUCCESS;
}
