// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/gl2ps>

#include <cstdio>

int main(int,char**) {
  FILE* file = ::fopen("out.ps","w");
  if(!file) return 0;
  /*
  static tools_gl2ps_gl_funcs_t s_OpenGL_funcs = {
    glIsEnabled,
    glBegin,
    glEnd,
    glGetFloatv,
    glVertex3f,
    glGetBooleanv,
    glGetIntegerv,
    glRenderMode,
    glFeedbackBuffer,
    glPassThrough
  };

  tools_gl2ps_set_gl_funcs(&s_OpenGL_funcs);
  */
  
  tools_GL2PScontext* gl2ps_context = tools_gl2psCreateContext();
  
  int options = TOOLS_GL2PS_OCCLUSION_CULL
    | TOOLS_GL2PS_BEST_ROOT
    | TOOLS_GL2PS_SILENT
    | TOOLS_GL2PS_DRAW_BACKGROUND;
  int sort = TOOLS_GL2PS_BSP_SORT;
  //sort = TOOLS_GL2PS_SIMPLE_SORT;

  tools_GLint vp[4];
  vp[0] = 0;
  vp[1] = 0;
  vp[2] = 400;
  vp[3] = 400;

  int bufsize = 0;
  if(tools_gl2psBeginPage(gl2ps_context,"title","exlib_QOpenGLArea",
                   vp,TOOLS_GL2PS_EPS,sort,options,
                   TOOLS_GL_RGBA,0, NULL,0,0,0,bufsize,
                   file,"out.ps")==TOOLS_GL2PS_ERROR) {
    ::fclose(file);
    return 0;
  }
    
  //paintGL();

  tools_gl2psEndPage(gl2ps_context);

  tools_gl2psDeleteContext(gl2ps_context);
   
  tools_gl2ps_reset_gl_funcs(gl2ps_context);

  ::fclose(file);

  return 0;
}
