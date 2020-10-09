// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/viewplot>

#ifdef TOOLS_USE_PNG
#include <tools/png>
#endif


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
      if ( use_HD ) a_viewer.plots().set_line_width(5);
      a_viewer.plots().set_grids_visibility();
      a_viewer.plots().adjust_scales(a_plotter_scale);
      a_viewer.write_page();

      a_viewer.plots().init_sg(); //it will recreate the sg::plotters and then reset the styles on new ones.

      isWriteNeeded = false;
    }
    a_viewer.plots().next();
  }

  if ( isWriteNeeded ) {
    if ( use_HD ) a_viewer.plots().set_line_width(5);
    a_viewer.plots().set_grids_visibility();
    a_viewer.plots().adjust_scales(a_plotter_scale);
    a_viewer.write_page();
  }

  a_viewer.close_file();
}

#ifdef TOOLS_USE_FREETYPE
#include <tools/sg/text_freetype>
#endif

#ifdef TOOLS_USE_EXPAT
#include <tools/xml/xml_style>
#include <tools/xml/wrap_viewplot_fonts_google_style> // xml/viewplot.style file embeded in an inline function.
inline bool load_embeded_styles(tools::xml::styles& a_styles) {
  std::string ss;
  unsigned int linen;
  const char** lines = viewplot_fonts_google_style(linen);
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
  plotter.bins_style(0).color = tools::colorf_blue();
//plotter.x_axis().title.value("MeV");
//plotter.x_axis().title.value("[MeV");
//plotter.x_axis().labels_style().encoding = tools::sg::encoding_PAW();
//plotter.x_axis().labels_style().encoding = tools::sg::encoding_none();
 }

  // down area :
 {viewer.plots().next();
  viewer.plot(h2);
  tools::sg::plotter& plotter = viewer.plots().current_plotter();
  plotter.bins_style(0).color = tools::colorf_red();}

  viewer.plots().view_border = false;
  viewer.plots().adjust_scales();

  viewer.write("out_one_page_two_regions_hershey_font.ps");

#ifdef TOOLS_USE_PNG
  viewer.write_inzb_png(tools::png::write,"out_one_page_two_regions_hershey_font.png");
#endif
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
  viewer.plots().adjust_scales();
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
  //  tools::xml::load_style_file(std::cout,"viewplot.style",viewer.styles());
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
  viewer.plots().set_line_width(5);
  viewer.write_page();

  viewer.set_cols_rows(1,2);
  viewer.plots().clear();
  viewer.plot(p1);
  viewer.set_current_plotter_style("hippodraw");
  viewer.plots().next();
  viewer.plot(h2);
  viewer.set_current_plotter_style("hippodraw");
  viewer.plots().set_line_width(5);
  viewer.plots().adjust_scales();
  viewer.write_page();

  viewer.close_file();

  //////////////////////////////////////////////////////////////////////////////
  /// plotting 2x2 : ///////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  viewer.open_file("out_2x2.ps");
  viewer.plots().set_current_plotter(0);

  viewer.set_cols_rows(2,2);
  viewer.plots().clear();

  viewer.plot(h1);
  viewer.set_current_plotter_style("ROOT_default");
  viewer.plots().set_line_width(5);

  viewer.plots().next();
  viewer.plot(p1);
  viewer.set_current_plotter_style("ROOT_default");
  viewer.plots().set_line_width(5);

  viewer.plots().next();
  viewer.plot(p1);
  viewer.set_current_plotter_style("ROOT_default");
  viewer.plots().set_line_width(5);

  viewer.plots().next();
  viewer.plot(h1);
  viewer.set_current_plotter_style("ROOT_default");
  viewer.plots().set_line_width(5);

  viewer.plots().adjust_scales();
  viewer.write_page();

  viewer.close_file();

#ifdef TOOLS_USE_PNG
  viewer.write_inzb_png(tools::png::write,"out_2x2.png");
#endif

  //////////////////////////////////////////////////////////
  /// with a copy of viewer : //////////////////////////////
  //////////////////////////////////////////////////////////
 {tools::viewplot viewer_cp(viewer);
  viewer_cp.open_file("out_2x2_viewer_copy.ps");
  viewer_cp.write_page();
  viewer_cp.close_file();}

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
