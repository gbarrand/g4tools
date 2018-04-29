// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

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

#include <tools/viewplot>

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
      HD_style(a_viewer.plots(),3);
      grid_style(a_viewer.plots());
      regions_style(a_viewer.plots(),a_plotter_scale);

      a_viewer.write_page();

      a_viewer.plots().init_sg(); //it will recreate the sg::plotters and then reset the styles on new ones.

      isWriteNeeded = false;
    }
    a_viewer.plots().next();
  }  

  if ( isWriteNeeded ) {
    HD_style(a_viewer.plots(),3);
    grid_style(a_viewer.plots());
    regions_style(a_viewer.plots(),a_plotter_scale);
    a_viewer.write_page();
  }

  a_viewer.close_file();
}  

#include <tools/sg/text_freetype>
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

  tools::sg::text_freetype ttf;
  tools::viewplot viewer(a_out,ttf,1,1,ww,wh); //cols=1,rows=2 then width and height

  viewer.plots().view_border = false;

  load_embeded_styles(viewer.styles());

  viewer.styles().add_colormap("default",tools::sg::style_default_colormap());
  viewer.styles().add_colormap("ROOT",tools::sg::style_ROOT_colormap());

  //plot_file(viewer,a_file,a_cols,a_rows,a_plotter_scale,a_style,5,h1,a_verbose);
  // limit to one page per file, so that we can easily compare pngs :
  plot_file(viewer,a_file,a_cols,a_rows,a_plotter_scale,a_style,a_cols*a_rows,h1,a_verbose);

  return true;
}

#include "test_with_file"

#include "../sys/dir"

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
    TOOLS_TEST_FUNC(tools::putenv(s_TOOLS_FONT_PATH(),"."))
  }
  TOOLS_TEST_FUNC(tools::is_env(s_TOOLS_FONT_PATH()))

  bool _remove = true;

  if(!tools::test_with_file(a_out,a_verbose,"out_viewplot_1x2_r_",".ps",_remove,
                            utest_viewplot_out_1x2_r_ps,test_write_file,1,2,0.9f,"ROOT_default")) return false;
  if(!tools::test_with_file(a_out,a_verbose,"out_viewplot_1x2_h_",".ps",_remove,
                            utest_viewplot_out_1x2_h_ps,test_write_file,1,2,  1,"hippodraw")) return false;
  if(!tools::test_with_file(a_out,a_verbose,"out_viewplot_1x2_d_",".ps",_remove,
                            utest_viewplot_out_1x2_d_ps,test_write_file,1,2,  1,"inlib_default")) return false;

  return true;
}
