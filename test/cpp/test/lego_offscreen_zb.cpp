// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include "lego_offscreen_sg"

#include <tools/sg/render_zb>
#include <tools/wps>

#include <tools/test>

#include "wrap_utest_lego_offscreen_out_zb_ps"

inline bool lego_offscreen_zb(std::ostream& a_out,bool a_verbose,const std::string& a_file) {
  if(a_verbose) a_out << "lego_offscreen_zb " << a_file << " ..." << std::endl;

  unsigned int width = 400;
  unsigned int height = 200;
  
  tools::sg::manager_zb mgr; //must be deleted after sep.
  
  tools::sg::separator* sep = lego_offscreen_sg(width,height);

 {unsigned int factor = 2; //have greater size to have good freetype rendering.
  unsigned int _width = factor*width;
  unsigned int _height = factor*height;
  tools::sg::render_zb action(mgr,a_out,_width,_height);
  tools::colorf clear_color = tools::colorf_white();
  action.zbuffer().clear_color_buffer(0);
  action.add_color(clear_color.r(),clear_color.g(),clear_color.b());
  action.zbuffer().clear_depth_buffer();
  sep->render(action);
  tools::wps wps(a_out);
  TOOLS_TEST_FUNC(wps.open_file(a_file,true))
  wps.PS_BEGIN_PAGE();
  wps.PS_PAGE_SCALE(float(_width),float(_height));
  wps.PS_IMAGE(_width,_height,tools::wps::rgb_4,tools::sg::render_zb::get_rgb,&action);
  wps.PS_END_PAGE();
  wps.close_file();}

  delete sep;

  return true;
}

#include "test_with_file"

#include "../sys/dir"

TOOLS_GLOBAL_STRING(TOOLS_FONT_PATH)

inline bool filter_CreationDate(const std::string& a_line) {
  return a_line.find("CreationDate")==std::string::npos;
}

bool test_lego_offscreen_zb(std::ostream& a_out,bool a_verbose) {
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

  if(!tools::test_with_file(a_out,a_verbose,"out_lego_offscreen_zb",".ps",_remove,utest_lego_offscreen_out_zb_ps,
			    lego_offscreen_zb,filter_CreationDate)) {
    a_out << "test_lego_offscreen_zb :"
          << " failed. It may come from a version of freetype producing not the same results as the one"
          << " used to produce the reference file."
          << std::endl;
    return false;
  }
 
  return true;
}
