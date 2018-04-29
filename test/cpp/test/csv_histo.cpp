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

class to_compare_csv {
public:
  std::string rg_title;
  unsigned int rg_entries;
  unsigned int rg_all_entries;
  double rg_mean;
  double rg_rms;
  //
  std::string prof_title;
  unsigned int prof_all_entries;
  double prof_mean;
  double prof_rms;
  double prof_Svw;
  double prof_Sv2w;
  //
  std::string prof_edges_title;
  unsigned int prof_edges_all_entries;
  double prof_edges_mean;
  double prof_edges_rms;
  double prof_edges_Svw;
  double prof_edges_Sv2w;
  //
  std::string rgbw_title;
  unsigned int rgbw_all_entries;
  double rgbw_mean_x;
  double rgbw_rms_x;
  double rgbw_mean_y;
  double rgbw_rms_y;
  double rgbw_Sxyw;
  //
  std::string prof2D_title;
  unsigned int prof2D_all_entries;
  double prof2D_mean_x;
  double prof2D_rms_x;
  double prof2D_mean_y;
  double prof2D_rms_y;
  double prof2D_Svw;
  double prof2D_Sv2w;
  //
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

#include <tools/wcsv_histo>
#include <fstream>

#include <tools/test>

static bool write_test_csv(std::ostream& a_out,bool a_verbose,to_compare_csv& a_cmp,const std::string& a_prefix) {
  if(a_verbose) a_out << "write_test_csv ..." << std::endl;

  char sep = ',';
  char hc = '#';

  unsigned int prec = 10;

  unsigned int entries = 1000000;

 {std::string file = a_prefix+"_h1d.csv";
  std::ofstream writer(file.c_str());
  if(writer.fail()) {a_out << "can't open " << file << "." << std::endl;return false;}
  writer.precision(prec);
  tools::histo::h1d h("rg",100,-5,5);
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
  a_cmp.rg_title = h.title();
  a_cmp.rg_all_entries = h.all_entries();
  a_cmp.rg_entries = h.entries();
  a_cmp.rg_mean = h.mean();
  a_cmp.rg_rms = h.rms();
  // write :
  if(!tools::wcsv::hto(writer,h.s_cls(),h,sep,hc)) return false;
  writer.close();}

 /*
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
 */

 {std::string file = a_prefix+"_p1d.csv";
  std::ofstream writer(file.c_str());
  if(writer.fail()) {a_out << "can't open " << file << "." << std::endl;return false;}
  writer.precision(prec);
  tools::histo::p1d h("prof",100,-5,5,-2,2);
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
  a_cmp.prof_title = h.title();
  a_cmp.prof_all_entries = h.all_entries();
  a_cmp.prof_mean = h.mean();
  a_cmp.prof_rms = h.rms();
  a_cmp.prof_Svw = h.get_Svw();
  a_cmp.prof_Sv2w = h.get_Sv2w();
  if(!tools::wcsv::pto(writer,h.s_cls(),h,sep,hc)) return false;
  writer.close();}

 {std::string file = a_prefix+"_p1d_edges.csv";
  std::ofstream writer(file.c_str());
  if(writer.fail()) {a_out << "can't open " << file << "." << std::endl;return false;}
  writer.precision(prec);
  std::vector<double> edges;
 {double step = (5.0-(-5.0))/100.0;
  for(unsigned int i=0;i<=100;i++) edges.push_back(-5+i*step);}
  tools::histo::p1d h("prof",edges,-2,2);
 {tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),1);}
  if(a_verbose) {
    a_out << "p1d_edges : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << ", Svw " << h.get_Svw() << ", Sv2w " << h.get_Sv2w()
              << std::endl;
  }
  a_cmp.prof_edges_title = h.title();
  a_cmp.prof_edges_all_entries = h.all_entries();
  a_cmp.prof_edges_mean = h.mean();
  a_cmp.prof_edges_rms = h.rms();
  a_cmp.prof_edges_Svw = h.get_Svw();
  a_cmp.prof_edges_Sv2w = h.get_Sv2w();
  if(!tools::wcsv::pto(writer,h.s_cls(),h,sep,hc)) return false;
  writer.close();}

 {std::string file = a_prefix+"_h2d.csv";
  std::ofstream writer(file.c_str());
  if(writer.fail()) {a_out << "can't open " << file << "." << std::endl;return false;}
  writer.precision(prec);
  tools::histo::h2d h("rgbw",20,-5,5,20,-2,2);
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
  a_cmp.rgbw_title = h.title();
  a_cmp.rgbw_all_entries = h.all_entries();
  a_cmp.rgbw_mean_x = h.mean_x();
  a_cmp.rgbw_rms_x = h.rms_x();
  a_cmp.rgbw_mean_y = h.mean_y();
  a_cmp.rgbw_rms_y = h.rms_y();
  a_cmp.rgbw_Sxyw = h.Sxyw();
  // write :
  if(!tools::wcsv::hto(writer,h.s_cls(),h,sep,hc)) return false;
  writer.close();}

 {std::string file = a_prefix+"_p2d.csv";
  std::ofstream writer(file.c_str());
  if(writer.fail()) {a_out << "can't open " << file << "." << std::endl;return false;}
  writer.precision(prec);
  tools::histo::p2d h("prof2D",100,-5,5,100,-5,5,-2,2);
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
  a_cmp.prof2D_title = h.title();
  a_cmp.prof2D_all_entries = h.all_entries();
  a_cmp.prof2D_mean_x = h.mean_x();
  a_cmp.prof2D_rms_x = h.rms_x();
  a_cmp.prof2D_mean_y = h.mean_y();
  a_cmp.prof2D_rms_y = h.rms_y();
  a_cmp.prof2D_Svw = h.get_Svw();
  a_cmp.prof2D_Sv2w = h.get_Sv2w();
  //::printf("debug : %.10g %.10g\n",h.get_Svw(),h.get_Sv2w());
  if(!tools::wcsv::pto(writer,h.s_cls(),h,sep,hc)) return false;
  writer.close();}

 {std::string file = a_prefix+"_h3d.csv";
  std::ofstream writer(file.c_str());
  if(writer.fail()) {a_out << "can't open " << file << "." << std::endl;return false;}
  prec = 20;
  writer.precision(prec);
  tools::histo::h3d h("rggbw",20,-5,5,20,-5,5,20,-2,2);
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
  if(!tools::wcsv::hto(writer,h.s_cls(),h,sep,hc)) return false;
  writer.close();}

  if(a_verbose) a_out << "write_test_csv : end." << std::endl;
  
  return true;
}

#include <tools/rcsv_histo>
#include <tools/file>

static bool read_test_csv(std::ostream& a_out,bool a_verbose,const to_compare_csv& a_cmp,const std::string& a_prefix) {
  if(a_verbose) a_out << "read_test_csv ..." << std::endl;

  double tolerance = 1e-5;
  unsigned int prec = 10;

  ////////////////////////////////////////////////////////
  /// histos /////////////////////////////////////////////
  ////////////////////////////////////////////////////////
 {std::string file = a_prefix+"_h1d.csv";
  std::ifstream reader(file.c_str());
  if(reader.fail()) {
    a_out << "can't open " << file << std::endl;
    return false;
  }
  reader.precision(prec);
  tools::rcsv::histo rh(reader);
  std::string _class;
  void* obj;
  if(!rh.read(a_out,_class,obj,a_verbose)) {
    a_out << "can't read histo." << std::endl;
    return false;
  }
  if(_class!=tools::histo::h1d::s_class()) {
    a_out << "class is not h1d." << std::endl;
    return false;
  }
  tools::histo::h1d* h = static_cast<tools::histo::h1d*>(obj);
  if(a_verbose) {
    a_out << "h1d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean " << h->mean() << ", rms " << h->rms()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rg_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rg_all_entries)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rg_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.rg_mean,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.rg_rms,tolerance,tools::dfabs)) return false;
  delete h;
  reader.close();
  TOOLS_TEST_FUNC(tools::file::std_remove(file))}

 /*
 {tools::rroot::key* key = dir->find_key("rf");
  if(!key) {
    a_out << "key rf not found." << std::endl;
    return false;
  }
  tools::histo::h1d* h = tools::rroot::key_to_h1d(*key);
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
  //if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rg_title)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rg_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rg_entries)) return false;
  //if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.rg_mean,tolerance)) return false;
  //if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.rg_rms,tolerance)) return false;
  delete h;}
 */

 {std::string file = a_prefix+"_p1d.csv";
  std::ifstream reader(file.c_str());
  if(reader.fail()) {
    a_out << "can't open " << file << std::endl;
    return false;
  }
  reader.precision(prec);
  tools::rcsv::histo rh(reader);
  std::string _class;
  void* obj;
  if(!rh.read(a_out,_class,obj,a_verbose)) {
    a_out << "can't read histo." << std::endl;
    return false;
  }
  if(_class!=tools::histo::p1d::s_class()) {
    a_out << "class is not h1d." << std::endl;
    return false;
  }
  tools::histo::p1d* h = static_cast<tools::histo::p1d*>(obj);
  if(a_verbose) {
    a_out << "p1d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean " << h->mean() << ", rms " << h->rms()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.prof_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.prof_all_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.prof_mean,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.prof_rms,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Svw(),a_cmp.prof_Svw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Sv2w(),a_cmp.prof_Sv2w,tolerance,tools::dfabs)) return false;
  delete h;
  reader.close();
  TOOLS_TEST_FUNC(tools::file::std_remove(file))}

 {std::string file = a_prefix+"_p1d_edges.csv";
  std::ifstream reader(file.c_str());
  if(reader.fail()) {
    a_out << "can't open " << file << std::endl;
    return false;
  }
  reader.precision(prec);
  tools::rcsv::histo rh(reader);
  std::string _class;
  void* obj;
  if(!rh.read(a_out,_class,obj,a_verbose)) {
    a_out << "can't read histo." << std::endl;
    return false;
  }
  if(_class!=tools::histo::p1d::s_class()) {
    a_out << "class is not h1d." << std::endl;
    return false;
  }
  tools::histo::p1d* h = static_cast<tools::histo::p1d*>(obj);
  if(a_verbose) {
    a_out << "p1d_edges : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean " << h->mean() << ", rms " << h->rms()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.prof_edges_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.prof_edges_all_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.prof_edges_mean,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.prof_edges_rms,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Svw(),a_cmp.prof_edges_Svw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Sv2w(),a_cmp.prof_edges_Sv2w,tolerance,tools::dfabs)) return false;
  delete h;
  reader.close();
  TOOLS_TEST_FUNC(tools::file::std_remove(file))}

 {std::string file = a_prefix+"_h2d.csv";
  std::ifstream reader(file.c_str());
  if(reader.fail()) {
    a_out << "can't open " << file << std::endl;
    return false;
  }
  reader.precision(prec);
  tools::rcsv::histo rh(reader);
  std::string _class;
  void* obj;
  if(!rh.read(a_out,_class,obj,a_verbose)) {
    a_out << "can't read histo." << std::endl;
    return false;
  }
  if(_class!=tools::histo::h2d::s_class()) {
    a_out << "class is not h2d." << std::endl;
    return false;
  }
  tools::histo::h2d* h = static_cast<tools::histo::h2d*>(obj);
  if(a_verbose) {
    a_out << "h2d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
          << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rgbw_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rgbw_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.rgbw_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_x(),a_cmp.rgbw_mean_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_x(),a_cmp.rgbw_rms_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_y(),a_cmp.rgbw_mean_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_y(),a_cmp.rgbw_rms_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->Sxyw(),a_cmp.rgbw_Sxyw,tolerance,tools::dfabs)) return false;
  delete h;
  reader.close();
  TOOLS_TEST_FUNC(tools::file::std_remove(file))}

 {std::string file = a_prefix+"_p2d.csv";
  std::ifstream reader(file.c_str());
  if(reader.fail()) {
    a_out << "can't open " << file << std::endl;
    return false;
  }
  reader.precision(prec);
  tools::rcsv::histo rh(reader);
  std::string _class;
  void* obj;
  if(!rh.read(a_out,_class,obj,a_verbose)) {
    a_out << "can't read histo." << std::endl;
    return false;
  }
  if(_class!=tools::histo::p2d::s_class()) {
    a_out << "class is not p2d." << std::endl;
    return false;
  }
  tools::histo::p2d* h = static_cast<tools::histo::p2d*>(obj);
  if(a_verbose) {
    a_out << "p2d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
          << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
          << std::endl;
  }
  if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.prof2D_title)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.prof2D_all_entries)) return false;
  //if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,h->entries(),a_cmp.prof2D_entries)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_x(),a_cmp.prof2D_mean_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_x(),a_cmp.prof2D_rms_x,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_y(),a_cmp.prof2D_mean_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_y(),a_cmp.prof2D_rms_y,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Svw(),a_cmp.prof2D_Svw,tolerance,tools::dfabs)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Sv2w(),a_cmp.prof2D_Sv2w,tolerance,tools::dfabs)) return false;
  delete h;
  reader.close();
  TOOLS_TEST_FUNC(tools::file::std_remove(file))}

 {std::string file = a_prefix+"_h3d.csv";
  std::ifstream reader(file.c_str());
  if(reader.fail()) {
    a_out << "can't open " << file << std::endl;
    return false;
  }
  reader.precision(prec);
  tools::rcsv::histo rh(reader);
  std::string _class;
  void* obj;
  if(!rh.read(a_out,_class,obj,a_verbose)) {
    a_out << "can't read histo." << std::endl;
    return false;
  }
  if(_class!=tools::histo::h3d::s_class()) {
    a_out << "class is not h3d." << std::endl;
    return false;
  }
  tools::histo::h3d* h = static_cast<tools::histo::h3d*>(obj);
  if(a_verbose) {
    a_out << "h3d : " << h->title()
          << ", all_entries " << h->all_entries()
          << ", entries " << h->entries()
          << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
          << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
          << ", mean_z " << h->mean_z() << ", rms_z " << h->rms_z()
          << std::endl;
  }
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
  delete h;
  reader.close();
  TOOLS_TEST_FUNC(tools::file::std_remove(file))}

  if(a_verbose) a_out << "read_test_csv : end." << std::endl;
  
  return true;
}

#include "../sys/dir"
#include "../sys/process"
#include <tools/num2s>
//#include <tools/args>
//#include <tools/sargs>

bool test_csv_histo(std::ostream& a_out,bool a_verbose) {
  //bool a_verbose = a_args.is_arg(tools::s_arg_verbose());

  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string prefix;
  TOOLS_TEST_FUNC(tools::num2s(tools::process_id(),prefix))
  prefix = "utest_"+prefix;
  if(a_verbose) a_out << "file prefix : " << prefix << std::endl;

  to_compare_csv _cmp;
  if(!write_test_csv(a_out,a_verbose,_cmp,prefix)) return false;
  if(!read_test_csv(a_out,a_verbose,_cmp,prefix)) return false;

  return true;
}
