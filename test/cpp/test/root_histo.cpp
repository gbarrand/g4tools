// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/histo/h1d>
#include <tools/histo/h2d>
#include <tools/histo/h3d>
#include <tools/histo/p1d>
#include <tools/histo/p2d>
#include <tools/histo/h1df>

#include <tools/randd>
#include <tools/randf>
#include <tools/mathd>

//#include <tools/zlib>

class to_compare_cern_root {
public:
  tools::histo::h1d m_rg;
  std::string rg_title;
  unsigned int rg_entries;
  unsigned int rg_all_entries;
  double rg_mean;
  double rg_rms;
  //
  tools::histo::p1d m_prof;
  std::string prof_title;
  unsigned int prof_all_entries;
  double prof_mean;
  double prof_rms;
  double prof_Svw;
  double prof_Sv2w;
  //
  tools::histo::h2d m_rgbw;
  std::string rgbw_title;
  unsigned int rgbw_all_entries;
  double rgbw_mean_x;
  double rgbw_rms_x;
  double rgbw_mean_y;
  double rgbw_rms_y;
  double rgbw_Sxyw;
  //
  tools::histo::p2d m_prof2D;
  std::string prof2D_title;
  unsigned int prof2D_all_entries;
  double prof2D_mean_x;
  double prof2D_rms_x;
  double prof2D_mean_y;
  double prof2D_rms_y;
  double prof2D_Svw;
  double prof2D_Sv2w;
  //
  tools::histo::h3d m_rggbw;
  std::string rggbw_title;
  unsigned int rggbw_all_entries;
  double rggbw_mean_x;
  double rggbw_rms_x;
  double rggbw_mean_y;
  double rggbw_rms_y;
  double rggbw_mean_z;
  double rggbw_rms_z;
  double rggbw_Sxyw;
  double rggbw_Syzw;
  double rggbw_Szxw;
};

#include <tools/wroot/file>
#include <tools/wroot/to>

#include <tools/test>

static bool write_test_root(std::ostream& a_out,bool a_verbose,to_compare_cern_root& a_cmp,const std::string& a_file) {
  if(a_verbose) a_out << "write_test_root ..." << std::endl;

  tools::wroot::file rfile(a_out,a_file);

  //rfile.add_ziper('Z',exlib::compress_buffer);
  //rfile.set_compression(9);

  tools::wroot::directory* dir = rfile.dir().mkdir("histo");
  if(!dir) {
    a_out << "write_test_root : can't create diectory." << std::endl;
    return false;
  }

  unsigned int entries = 1000000;

 {tools::histo::h1d h("rg",100,-5,5);
 {tools::rgaussd rg(1,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),1.4);}
  // plotting hints :
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"1.4*entries");
  if(a_verbose) {
    a_out << "h1d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << std::endl;
  }
  a_cmp.m_rg = h;
  a_cmp.rg_title = h.title();
  a_cmp.rg_all_entries = h.all_entries();
  a_cmp.rg_entries = h.entries();
  a_cmp.rg_mean = h.mean();
  a_cmp.rg_rms = h.rms();
  // write :
  if(!tools::wroot::to(*dir,h,"rg")) return false;}

 {tools::rgaussf rf(1,2);
  tools::histo::h1df h("rf",100,-5,5);
  for(unsigned int count=0;count<entries;count++) h.fill(rf.shoot(),1.4F);
  // plotting hints :
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"1.4*entries");
  if(a_verbose) {
    a_out << "h1df : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << std::endl;
  }
  // write :
  if(!tools::wroot::to(*dir,h,"rf")) return false;}

 {tools::histo::p1d h("prof",100,-5,5,-2,2);
 {tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),1);}
  if(a_verbose) {
    a_out << "p1d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << ", Svw " << h.get_Svw() << ", Sv2w " << h.get_Sv2w()
              << std::endl;
  }
  a_cmp.m_prof = h;
  a_cmp.prof_title = h.title();
  a_cmp.prof_all_entries = h.all_entries();
  a_cmp.prof_mean = h.mean();
  a_cmp.prof_rms = h.rms();
  a_cmp.prof_Svw = h.get_Svw();
  a_cmp.prof_Sv2w = h.get_Sv2w();
  if(!tools::wroot::to(*dir,h,"prof")) return false;}

 {tools::histo::h2d h("rgbw",20,-5,5,20,-2,2);
 {tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),0.8);}
  //plotting hints :
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"rand bw");
  h.add_annotation(tools::histo::key_axis_z_title(),"0.8*entries");
  if(a_verbose) {
    a_out << "h2d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean_x " << h.mean_x() << ", rms_x " << h.rms_x()
              << ", mean_y " << h.mean_y() << ", rms_y " << h.rms_y()
              << std::endl;
  }
  a_cmp.m_rgbw = h;
  a_cmp.rgbw_title = h.title();
  a_cmp.rgbw_all_entries = h.all_entries();
  a_cmp.rgbw_mean_x = h.mean_x();
  a_cmp.rgbw_rms_x = h.rms_x();
  a_cmp.rgbw_mean_y = h.mean_y();
  a_cmp.rgbw_rms_y = h.rms_y();
  a_cmp.rgbw_Sxyw = h.Sxyw();
  // write :
  if(!tools::wroot::to(*dir,h,"rgbw")) return false;}

 {tools::histo::p2d h("prof2D",100,-5,5,100,-5,5,-2,2);
 {tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rg.shoot(),rbw.shoot(),1);}
  if(a_verbose) {
    a_out << "p2d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean_x " << h.mean_x() << ", rms_x " << h.rms_x()
              << ", mean_y " << h.mean_y() << ", rms_y " << h.rms_y()
              << ", Svw " << h.get_Svw() << ", Sv2w " << h.get_Sv2w()
              << std::endl;
  }
  a_cmp.m_prof2D = h;
  a_cmp.prof2D_title = h.title();
  a_cmp.prof2D_all_entries = h.all_entries();
  a_cmp.prof2D_mean_x = h.mean_x();
  a_cmp.prof2D_rms_x = h.rms_x();
  a_cmp.prof2D_mean_y = h.mean_y();
  a_cmp.prof2D_rms_y = h.rms_y();
  a_cmp.prof2D_Svw = h.get_Svw();
  a_cmp.prof2D_Sv2w = h.get_Sv2w();
  //::printf("debug : %.10g %.10g\n",h.get_Svw(),h.get_Sv2w());
  if(!tools::wroot::to(*dir,h,"prof2D")) return false;}

 {tools::histo::h3d h("rggbw",20,-5,5,20,-5,5,20,-2,2);
 {tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rg.shoot(),rbw.shoot(),0.8);}
  //plotting hints :
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_z_title(),"rand bw");
  if(a_verbose) {
    a_out << "h3d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean_x " << h.mean_x() << ", rms_x " << h.rms_x()
              << ", mean_y " << h.mean_y() << ", rms_y " << h.rms_y()
              << ", mean_z " << h.mean_z() << ", rms_z " << h.rms_z()
              << std::endl;
  }
  a_cmp.m_rggbw = h;
  a_cmp.rggbw_title = h.title();
  a_cmp.rggbw_all_entries = h.all_entries();
  a_cmp.rggbw_mean_x = h.mean_x();
  a_cmp.rggbw_rms_x = h.rms_x();
  a_cmp.rggbw_mean_y = h.mean_y();
  a_cmp.rggbw_rms_y = h.rms_y();
  a_cmp.rggbw_mean_z = h.mean_z();
  a_cmp.rggbw_rms_z = h.rms_z();
  a_cmp.rggbw_Sxyw = h.Sxyw();
  a_cmp.rggbw_Syzw = h.Syzw();
  a_cmp.rggbw_Szxw = h.Szxw();
  // write :
  if(!tools::wroot::to(*dir,h,"rggbw")) return false;}

  //////////////////////////////////////////////////////////
  /// write and close file : ///////////////////////////////
  //////////////////////////////////////////////////////////
 {unsigned int n;
  if(!rfile.write(n)) {
    a_out << "file write failed." << std::endl;
    rfile.close();
    return false;
  }}
  
  rfile.close();

  return true;
}

#include <tools/fileis>
#include <tools/rroot/file>
#include <tools/rroot/rall>

static bool read_test_root(std::ostream& a_out,bool a_verbose,const to_compare_cern_root& a_cmp,const std::string& a_file) {
  if(a_verbose) a_out << "read_test_root ..." << std::endl;

  double tolerance = 1e-5;

 {bool is;
  tools::file::is_root(a_file,is);
  if(!is) {
    a_out << "file is not a root file." << std::endl;
    return false;
  }}

  tools::rroot::file rfile(a_out,a_file,a_verbose);
  //rfile.add_unziper('Z',tools::decompress_buffer);
  if(!rfile.is_open()) {
    a_out << "file " << a_file << " not open." << std::endl;
    return false;
  }

  ////////////////////////////////////////////////////////
  /// histos /////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  tools::rroot::TDirectory* dir = tools::rroot::find_dir(rfile.dir(),"histo");
  if(!dir) {
    a_out << "directory histo not found." << std::endl;
    return false;
  }

 {tools::rroot::key* key = dir->find_key("rg");
  if(!key) {
    a_out << "key rg not found." << std::endl;
    return false;
  }
  tools::histo::h1d* h = tools::rroot::key_to_h1d(rfile,*key);
  if(!h) {
    a_out << "key rg is not an h1d." << std::endl;
    return false;
  }
  if(a_verbose) {
    a_out << "h1d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean " << h->mean() << ", rms " << h->rms()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->equals_TH(a_cmp.m_rg,tolerance,::fabs),true)) return false;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rg_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rg_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rg_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.rg_mean,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.rg_rms,tolerance,tools::dfabs)) return false;
  delete h;}

 {tools::rroot::key* key = dir->find_key("rf");
  if(!key) {
    a_out << "key rf not found." << std::endl;
    return false;
  }
  tools::histo::h1d* h = tools::rroot::key_to_h1d(rfile,*key);
  if(!h) {
    a_out << "key rf is not a h1d." << std::endl;
    return false;
  }
  if(a_verbose) {
    a_out << "h1d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean " << h->mean() << ", rms " << h->rms()
          << std::endl;
  }
  /*
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rg_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rg_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rg_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.rg_mean,tolerance)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.rg_rms,tolerance)) return false;
  */
  delete h;}

 {tools::rroot::key* key = dir->find_key("prof");
  if(!key) {
    a_out << "key prof not found." << std::endl;
    return false;
  }
  tools::histo::p1d* h = tools::rroot::key_to_p1d(rfile,*key);
  if(!h) {
    a_out << "key prof not a p1d." << std::endl;
    return false;
  }
  if(a_verbose) {
    a_out << "p1d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean " << h->mean() << ", rms " << h->rms()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->equals_TH(a_cmp.m_prof,tolerance,::fabs),true)) return false;

  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.prof_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.prof_all_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.prof_mean,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.prof_rms,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Svw(),a_cmp.prof_Svw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Sv2w(),a_cmp.prof_Sv2w,tolerance,tools::dfabs)) return false;
  delete h;}

 {tools::rroot::key* key = dir->find_key("rgbw");
  if(!key) {
    a_out << "key rgbw not found." << std::endl;
    return false;
  }
  tools::histo::h2d* h = tools::rroot::key_to_h2d(rfile,*key);
  if(!h) {
    a_out << "key rgbw is not an h1d." << std::endl;
    return false;
  }
  if(a_verbose) {
    a_out << "h2d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
          << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->equals_TH(a_cmp.m_rgbw,tolerance,::fabs),true)) return false;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rgbw_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rgbw_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rgbw_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_x(),a_cmp.rgbw_mean_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_x(),a_cmp.rgbw_rms_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_y(),a_cmp.rgbw_mean_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_y(),a_cmp.rgbw_rms_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->Sxyw(),a_cmp.rgbw_Sxyw,tolerance,tools::dfabs)) return false;
  delete h;}

 {tools::rroot::key* key = dir->find_key("prof2D");
  if(!key) {
    a_out << "key prof2D not found." << std::endl;
    return false;
  }
  tools::histo::p2d* h = tools::rroot::key_to_p2d(rfile,*key);
  if(!h) {
    a_out << "key prof2D is not an h1d." << std::endl;
    return false;
  }
  if(a_verbose) {
    a_out << "p2d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
          << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->equals_TH(a_cmp.m_prof2D,tolerance,::fabs),true)) return false;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.prof2D_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.prof2D_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.prof2D_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_x(),a_cmp.prof2D_mean_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_x(),a_cmp.prof2D_rms_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_y(),a_cmp.prof2D_mean_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_y(),a_cmp.prof2D_rms_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Svw(),a_cmp.prof2D_Svw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Sv2w(),a_cmp.prof2D_Sv2w,tolerance,tools::dfabs)) return false;
  delete h;}

 {tools::rroot::key* key = dir->find_key("rggbw");
  if(!key) {
    a_out << "key rggbw not found." << std::endl;
    return false;
  }
  tools::histo::h3d* h = tools::rroot::key_to_h3d(rfile,*key);
  if(!h) {
    a_out << "key rggbw is not an h1d." << std::endl;
    return false;
  }
  if(a_verbose) {
    a_out << "h3d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
          << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
          << ", mean_z " << h->mean_z() << ", rms_z " << h->rms_z()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->equals_TH(a_cmp.m_rggbw,tolerance,::fabs),true)) return false;
  
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rggbw_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rggbw_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rggbw_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_x(),a_cmp.rggbw_mean_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_x(),a_cmp.rggbw_rms_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_y(),a_cmp.rggbw_mean_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_y(),a_cmp.rggbw_rms_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_z(),a_cmp.rggbw_mean_z,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_z(),a_cmp.rggbw_rms_z,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->Sxyw(),a_cmp.rggbw_Sxyw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->Syzw(),a_cmp.rggbw_Syzw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->Szxw(),a_cmp.rggbw_Szxw,tolerance,tools::dfabs)) return false;
  delete h;}

  delete dir; 

  return true;
}

#include <tools/file>
#include "../sys/dir"
#include "../sys/process"
//#include <tools/args>
//#include <tools/sargs>

bool test_root_histo(std::ostream& a_out,bool a_verbose) {
  //bool a_verbose = a_args.is_arg(tools::s_arg_verbose());

  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string file;
  TOOLS_TEST_FUNC(tools::num2s(tools::process_id(),file))
  file = "utest_"+file+".root";
  if(a_verbose) a_out << "file : " << file << std::endl;

  to_compare_cern_root _cmp;
  if(!write_test_root(a_out,a_verbose,_cmp,file)) return false;
  if(!read_test_root(a_out,a_verbose,_cmp,file)) return false;

  TOOLS_TEST_FUNC(tools::file::std_remove(file))

  return true;
}
