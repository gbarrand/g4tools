// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.


#include <tools/waxml/begend>
#include <tools/waxml/histos>
#include <tools/waxml/ntuple>

#include <tools/randd>

#include <fstream>

class to_compare_axml {
public:
  std::string rg_title;
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
  std::string rgbw_title;
  unsigned int rgbw_all_entries;
  double rgbw_mean_x;
  double rgbw_rms_x;
  double rgbw_mean_y;
  double rgbw_rms_y;
  //double rgbw_Sxyw;
  /*
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
  */
  //
  std::vector<std::string> ntu_rg_rbw_strings;
  //
  unsigned int ntu_rg_rbw_2_all_entries;
  double ntu_rg_rbw_2_mean;
  double ntu_rg_rbw_2_rms;
};

static bool write_axml(std::ostream& a_out,bool a_verbose,to_compare_axml& a_cmp,const std::string& a_file) {
  if(a_verbose) a_out << "write_axml ..." << std::endl;

  //////////////////////////////////////////////////////////
  /// create a .aida file : ////////////////////////////////
  //////////////////////////////////////////////////////////
  std::ofstream writer(a_file.c_str());
  if(writer.fail()) {
    a_out << "can't open waxml.aida." << std::endl;
    return false;
  }
  tools::waxml::begin(writer);

  //////////////////////////////////////////////////////////
  /// create and write some histos : ///////////////////////
  //////////////////////////////////////////////////////////
  tools::rgaussd rg(1,2);
  tools::rbwd rbw(0,1);
  unsigned int entries = 1000000;

 {tools::histo::h1d h("rg",100,-5,5);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),1.4);
  // plotting hints :
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"1.4*entries");
  a_cmp.rg_title = h.title();
  a_cmp.rg_all_entries = h.all_entries();
  a_cmp.rg_mean = h.mean();
  a_cmp.rg_rms = h.rms();  // write :
  if(!tools::waxml::write(writer,h,"/histo","rg")) {
    a_out << "can't write h1d." << std::endl;
    return false;
  }}

 {tools::histo::p1d h("prof",100,-5,5,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),1);
  a_cmp.prof_title = h.title();
  a_cmp.prof_all_entries = h.all_entries();
  a_cmp.prof_mean = h.mean();
  a_cmp.prof_rms = h.rms();
  a_cmp.prof_Svw = h.get_Svw();
  a_cmp.prof_Sv2w = h.get_Sv2w();
  if(!tools::waxml::write(writer,h,"/histo","prof")) {
    a_out << "can't write prof." << std::endl;
    return false;
  }}

 {std::string title = "rgbw";  
  // have XML special characters in the title.
  title += " lower <";
  title += " greater >";
  title += " amp &";
  title += " quote '";
  title += " double quote \"";
  tools::histo::h2d h(title,20,-5,5,20,-2,2);
  for(unsigned int count=0;count<entries;count++) {
    h.fill(rg.shoot(),rbw.shoot(),0.8);
  }
  //plotting hints :
  h.add_annotation(tools::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(tools::histo::key_axis_y_title(),"rand bw");
  h.add_annotation(tools::histo::key_axis_z_title(),"0.8*entries");
  a_cmp.rgbw_title = h.title();
  a_cmp.rgbw_all_entries = h.all_entries();
  a_cmp.rgbw_mean_x = h.mean_x();
  a_cmp.rgbw_rms_x = h.rms_x();
  a_cmp.rgbw_mean_y = h.mean_y();
  a_cmp.rgbw_rms_y = h.rms_y();
  //a_cmp.rgbw_Sxyw = h.Sxyw(); //not writen
  // write :
  if(!tools::waxml::write(writer,h,"/histo","rgbw")) {
    a_out << "can't write h2d." << std::endl;
    return false;
  }}

  //////////////////////////////////////////////////////////
  /// create and write a flat ntuple : /////////////////////
  //////////////////////////////////////////////////////////
 {tools::waxml::ntuple ntu(writer);

  // create some columns with basic types :
  tools::waxml::ntuple::column<double>* col_rgauss = ntu.create_column<double>("rgauss");
  tools::waxml::ntuple::column<double>* col_rbw = ntu.create_column<double>("rbw");
  tools::waxml::ntuple::column<std::string>* col_str = ntu.create_column<std::string>("string");

  ntu.write_header("/tuple","rg_rbw","Randoms");

  // fill :
  std::string stmp;
  unsigned int ntu_entries = 10000;
  for(unsigned int count=0;count<ntu_entries;count++) {    
    col_rgauss->fill(rg.shoot());
    col_rbw->fill(rbw.shoot());
    if(!tools::num2s(count,stmp)) {}
    stmp = "str "+stmp;
    col_str->fill(stmp);
    if(count>=(ntu_entries-5)) a_cmp.ntu_rg_rbw_strings.push_back(stmp);
    ntu.add_row(); // it will write columns data as a <row> in the file.
  }
  ntu.write_trailer();}

  ////////////////////////////////////////////////////////////////
  /// create and write a flat ntuple by using ntuple_booking : ///
  ////////////////////////////////////////////////////////////////
 {tools::ntuple_booking nbk;
  nbk.add_column<double>("rgauss");
  nbk.add_column<double>("rbw");
  nbk.add_column<std::string>("string");
  std::vector<double> user_vec_d;
  nbk.add_column<double>("vec_d",user_vec_d);

  tools::waxml::ntuple ntu(writer,a_out,nbk);
  if(ntu.columns().size()) {

    tools::waxml::ntuple::column<double>* col_rgauss = ntu.find_column<double>("rgauss");
    tools::waxml::ntuple::column<double>* col_rbw = ntu.find_column<double>("rbw");
    tools::waxml::ntuple::column<std::string>* col_str = ntu.find_column<std::string>("string");

    ntu.write_header("/tuple","rg_rbw_2","Randoms");

    // fill :
    std::string stmp;
    tools::histo::h1d h("ntu_rg_rbw_2",100,-2,2);
    for(unsigned int count=0;count<100;count++) {    
      double rgauss = rg.shoot(); 
      col_rgauss->fill(rgauss);
      h.fill(rgauss);
      col_rbw->fill(rbw.shoot());

      if(!tools::num2s(count,stmp)) {}
      stmp = "str "+stmp;
      col_str->fill(stmp);

     {user_vec_d.clear();
      unsigned int number = count%5;
      for(unsigned int i=0;i<number;i++) {
        user_vec_d.push_back(rbw.shoot());
      }}
      ntu.add_row(); // it will write columns data as a <row> in the file.
    }
    a_cmp.ntu_rg_rbw_2_all_entries = h.all_entries();
    a_cmp.ntu_rg_rbw_2_mean = h.mean();
    a_cmp.ntu_rg_rbw_2_rms = h.rms();

    ntu.write_trailer();
  }}

  //////////////////////////////////////////////////////////
  /// close file : /////////////////////////////////////////
  //////////////////////////////////////////////////////////
  tools::waxml::end(writer);
  writer.close();

  return true;
}

#include <tools/raxml>
#include <tools/ntuple_binding>
#include <tools/test>

static bool read_axml(std::ostream& a_out,bool a_verbose,const to_compare_axml& a_cmp,const std::string& a_file) {
  if(a_verbose) a_out << "read_axml ..." << std::endl;

  double tolerance = 1e-5;

  tools::xml::default_factory fac;
  tools::raxml ml(fac,a_out,false/*a_verbose*/);

  //ml.set_compressed_reader(new exlib::gzip_reader());

  std::vector<tools::raxml_out>& objs = ml.objects();
  objs.clear();

  ml.load_file(a_file,false);

 {std::vector<tools::raxml_out>::const_iterator it;
  for(it=objs.begin();it!=objs.end();++it) {
    //    a_out << "---------------------------------" << std::endl;
    //    a_out << " obj = " << (*it).object() << std::endl;
    //    a_out << " class = " << (*it).cls() << std::endl;
    //    a_out << " path = " << (*it).path() << std::endl;
    //    a_out << " name = " << (*it).name() << std::endl;
  }}

 {std::vector<tools::raxml_out>::const_iterator it;
  for(it=objs.begin();it!=objs.end();++it) {
    const tools::raxml_out& raxml_out = *it;
    if(a_verbose) {
      a_out << "---------------------------------" << std::endl;
      a_out << "obj name = " << (*it).name() << std::endl;
    }
    if(raxml_out.cls()==tools::histo::h1d::s_class()) {
      tools::histo::h1d* h = (tools::histo::h1d*)raxml_out.object();
       //a_out << "h1d : title " << h->title() << std::endl;
      //a_out << "h1d : entries " << h->entries() << std::endl;
      //a_out << "h1d : mean " << h->mean() << " rms " << h->rms() << std::endl;
      if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rg_title)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rg_all_entries,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.rg_mean,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.rg_rms,tolerance,tools::dfabs)) return false;

    } else if(raxml_out.cls()==tools::histo::p1d::s_class()) {
      tools::histo::p1d* h = (tools::histo::p1d*)raxml_out.object();
      if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.prof_title)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.prof_all_entries,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean(),a_cmp.prof_mean,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms(),a_cmp.prof_rms,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Svw(),a_cmp.prof_Svw,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->get_Sv2w(),a_cmp.prof_Sv2w,tolerance,tools::dfabs)) return false;

    } else if(raxml_out.cls()==tools::histo::h2d::s_class()) {
      tools::histo::h2d* h = (tools::histo::h2d*)raxml_out.object();
      //a_out << "h2d : title " << h->title() << std::endl;
      //a_out << "h2d : entries " << h->entries() << std::endl;
      //a_out << "h2d : mean_x " << h->mean_x() << " rms_x " << h->rms_x() << std::endl;
      //a_out << "h2d : mean_y " << h->mean_y() << " rms_y " << h->rms_y() << std::endl;
      if(!tools::equal(a_out,__FILE__,__LINE__,h->title(),a_cmp.rgbw_title)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->all_entries(),a_cmp.rgbw_all_entries,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_x(),a_cmp.rgbw_mean_x,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_x(),a_cmp.rgbw_rms_x,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->mean_y(),a_cmp.rgbw_mean_y,tolerance,tools::dfabs)) return false;
      if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->rms_y(),a_cmp.rgbw_rms_y,tolerance,tools::dfabs)) return false;
      //if(!tools::equal<double>(a_out,__FILE__,__LINE__,h->Sxyw(),a_cmp.rgbw_Sxyw,tolerance,tools::dfabs)) return false;

    } else if(raxml_out.cls()==tools::aida::ntuple::s_class()) {
      tools::aida::ntuple* nt = (tools::aida::ntuple*)raxml_out.object();
      //a_out << "ntuple : title " << nt->title() << std::endl;
      //a_out << "ntuple : rows " << nt->rows() << std::endl;
      if((*it).name()=="rg_rbw") {
        if(a_verbose) a_out << "check ntu = " << (*it).name() << std::endl;

       {const std::vector<tools::aida::base_col*>& cols = nt->cols();
        std::vector<tools::aida::base_col*>::const_iterator itc;
        for(itc=cols.begin();itc!=cols.end();++itc) {
          tools::aida::aida_base_col* acol = tools::safe_cast<tools::aida::base_col,tools::aida::aida_base_col>(*(*itc));
          if(acol) {  
            //a_out << "ntuple : col name=" << (*itc)->name() << ", aida_type=" << acol->aida_type() << std::endl;
          } else {  
            //a_out << "ntuple : col name=" << (*itc)->name() << std::endl;
          }
        }}
       {tools::aida::aida_col<double>* col = nt->find_column<double>("rgauss");
        if(col) {
          nt->start();
          for(unsigned int row=0;row<5;row++) {
            if(!nt->next()) break;
            double v;
            if(!col->get_entry(v)) {}
            //a_out << " " << v << std::endl;
          }
        }}
       {tools::aida::aida_col<std::string>* col = nt->find_column<std::string>("string");
        if(col) {
          nt->start();
	  std::vector<std::string> vs;
          unsigned int ntu_entries = 10000;
          for(unsigned int row=0;row<ntu_entries;row++) {
            if(!nt->next()) break;
            std::string v;  
            if(!col->get_entry(v)) {}
            //a_out << "ro  w = " << row << ", string = " << v << std::endl;
            if(row>=(ntu_entries-5)) vs.push_back(v);
          }
          if(!tools::equal(a_out,__FILE__,__LINE__,vs,a_cmp.ntu_rg_rbw_strings)) return false;
       }}  
  
      } else if((*it).name()=="rg_rbw_2") {
        if(a_verbose) a_out << "check ntu = " << (*it).name() << std::endl;

        ///////////////////////////////////////////////////////
        /// read by using variable column binding : ///////////
        ///////////////////////////////////////////////////////
        tools::ntuple_binding nbd;
        double rgauss;
        nbd.add_column("rgauss",rgauss);
        std::string sval;
        nbd.add_column("string",sval);
        if(!nt->set_binding(a_out,nbd)) {
          a_out << "set ntuple binding failed." << std::endl;
          return false;
        }
        tools::histo::h1d h("ntu_rg_rbw_2",100,-2,2);
        nt->start();
        while(nt->next()){
          if(!nt->get_row()) {
            a_out << "get_row() failed." << std::endl;
            return false;
          }
          //a_out << energy << std::endl;
          //a_out << "string : " << sval << std::endl;
          h.fill(rgauss);
        }
        //h.hprint(a_out);
        if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.all_entries(),a_cmp.ntu_rg_rbw_2_all_entries,tolerance,tools::dfabs)) return false;
        if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.mean(),a_cmp.ntu_rg_rbw_2_mean,tolerance,tools::dfabs)) return false;
        if(!tools::equal<double>(a_out,__FILE__,__LINE__,h.rms(),a_cmp.ntu_rg_rbw_2_rms,tolerance,tools::dfabs)) return false;

      }
    }
  }}

  // to check compilation :
 {tools::ntuple_binding nbd;
  tools::aida::ntuple sub_ntuple(a_out,"sub_ntuple");
  nbd.add_column_cid("sub_ntuple",sub_ntuple);}
	
  return true;
}

#include "../sys/dir"
#include "../sys/process"
#include <tools/file>

bool test_axml(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string file;
  if(!tools::num2s(tools::process_id(),file)) {}
  file = "utest_"+file+".aida";
  if(a_verbose) a_out << "file : " << file << std::endl;

  to_compare_axml _cmp;
  if(!write_axml(a_out,a_verbose,_cmp,file)) return false;
  if(!read_axml(a_out,a_verbose,_cmp,file)) return false;

  TOOLS_TEST_FUNC(tools::file::std_remove(file))

  return true;
}
