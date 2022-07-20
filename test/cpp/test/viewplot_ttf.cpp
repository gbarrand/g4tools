// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/sg/plots>

#include <tools/viewplot>
#include <tools/put_env>

void plot_file(tools::viewplot& a_viewer,
               const std::string& a_filename,
               unsigned int a_columns, unsigned int a_rows,float a_plotter_scale,const std::string& a_style,
               int a_n,tools::histo::h1d& a_h1,bool a_verbose = false) {
  if(a_verbose)
    a_viewer.out() << "plot file : file name " << a_filename
                   << ", " << a_n << " plots in " << a_columns << "x" << a_rows << ", layout "
  	           << a_style << " ..." << std::endl;

  a_viewer.open_file(a_filename,true);

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
      if(a_verbose)
        a_viewer.out() << "plot file : file name " << a_filename
                       << ", apply HD_style(line_width=3), grid_style()"
                       << ", regions_style(" << a_plotter_scale << ") and write_page() ..."
                       << std::endl;
      a_viewer.plots().set_line_width(3);
      a_viewer.plots().set_grids_visibility();
      a_viewer.plots().adjust_scales(a_plotter_scale);

      a_viewer.write_page();

      a_viewer.plots().init_sg(); //it will recreate the sg::plotters and then reset the styles on new ones.

      isWriteNeeded = false;
    }
    a_viewer.plots().next();
  }

  if ( isWriteNeeded ) {
    if(a_verbose)
       a_viewer.out() << "plot file : file name " << a_filename
                      << ", apply HD_style(line_width=3), grid_style()"
	              << ", regions_style(" << a_plotter_scale << ") and write_page() ..."
                      << std::endl;
    a_viewer.plots().set_line_width(3);
    a_viewer.plots().set_grids_visibility();
    a_viewer.plots().adjust_scales(a_plotter_scale);
    a_viewer.write_page();
  }

  a_viewer.close_file();
}

#include <toolx/sg/text_freetype>
#include <toolx/xml/xml_style>
#include <tools/xml/wrap_viewplot_fonts_google_style> // xml/viewplot.style file embeded in an inline function.
inline bool load_embedded_styles(tools::xml::styles& a_styles) {
  std::string ss;
  unsigned int linen;
  const char** lines = viewplot_fonts_google_style(linen);
  for(unsigned int index=0;index<linen;index++) {
    std::string s = lines[index];
    tools::replace(s,"@@double_quote@@","\"");
    tools::replace(s,"@@back_slash@@","\\");
    ss += s + "\n";
  }
  return toolx::xml::load_style_string(a_styles,ss);
}

#include <tools/random>
#include <tools/viewplot>

#include <tools/test>

inline bool test_write_file(std::ostream& a_out,bool a_verbose,const std::string& a_file,
                        unsigned int a_cols, unsigned int a_rows,float a_plotter_scale,const std::string& a_style) {
  if(a_verbose) a_out << "test_write_file " << a_file << " ..." << std::endl;

  tools::histo::h1d h1("Random Gauss",100,-5,5);
  tools::random::gauss rg(0,1);
 {for(unsigned int index=0;index<10000;index++) h1.fill(rg.shoot(),1);}

  //Have vertical A4 :
  unsigned int ww = 700;
  float A4 = 29.7f/21.0f;
  unsigned int wh = (unsigned int)(float(ww)*A4);

  toolx::sg::text_freetype ttf;
  tools::viewplot viewer(a_out,ttf,1,1,ww,wh); //cols=1,rows=2 then width and height

  viewer.plots().view_border = false;

  load_embedded_styles(viewer.styles());

  viewer.styles().add_colormap("default",tools::sg::style_default_colormap());
  viewer.styles().add_colormap("ROOT",tools::sg::style_ROOT_colormap());

  //plot_file(viewer,a_file,a_cols,a_rows,a_plotter_scale,a_style,5,h1,a_verbose);
  // limit to one page per file, so that we can easily compare pngs :
  plot_file(viewer,a_file,a_cols,a_rows,a_plotter_scale,a_style,a_cols*a_rows,h1,a_verbose);

  return true;
}

#include "test_with_file"

#include <tools/sys/dir>

#include "wrap_utest_viewplot_out_1x2_r_ps"
#include "wrap_utest_viewplot_out_1x2_h_ps"
#include "wrap_utest_viewplot_out_1x2_d_ps"

TOOLS_GLOBAL_STRING(TOOLS_FONT_PATH)

bool test_viewplot_ttf(std::ostream& a_out,bool a_verbose) {
  //bool a_verbose = a_args.is_arg(tools::s_arg_verbose());

  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  if(!tools::is_env(s_TOOLS_FONT_PATH())) {
    TOOLS_TEST_FUNC(tools::put_env(s_TOOLS_FONT_PATH(),"."))
  }
  TOOLS_TEST_FUNC(tools::is_env(s_TOOLS_FONT_PATH()))

  bool _remove = true;

  bool stop_if_failed = false;

  if(!tools::test_with_file(a_out,a_verbose,"out_viewplot_1x2_r_",".ps",_remove,
                            utest_viewplot_out_1x2_r_ps,test_write_file,1,2,0.9f,"ROOT_default") && stop_if_failed) return false;
  if(!tools::test_with_file(a_out,a_verbose,"out_viewplot_1x2_h_",".ps",_remove,
                            utest_viewplot_out_1x2_h_ps,test_write_file,1,2,  1,"hippodraw") && stop_if_failed) return false;
  if(!tools::test_with_file(a_out,a_verbose,"out_viewplot_1x2_d_",".ps",_remove,
                            utest_viewplot_out_1x2_d_ps,test_write_file,1,2,  1,"inlib_default") && stop_if_failed) return false;

  return true;
}
