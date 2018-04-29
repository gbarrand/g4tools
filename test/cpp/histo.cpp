// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#ifdef TOOLS_MEM
#include <tools/mem>
#endif //TOOLS_MEM

#include <tools/histo/h1d>
#include <tools/histo/h2d>
#include <tools/histo/p1d>
#include <tools/histo/sliced>

#include <tools/randd>

#include <iostream>

int main(int argc,char** argv) {
#ifdef TOOLS_MEM
  tools::mem::set_check_by_class(true);{
#endif //TOOLS_MEM

  bool print = true;
  if(argc==2) {
    std::string s = argv[1];
    if(s=="-noprint") print = false;
  }

  unsigned int entries = 1000000;

  ////////////////////////////////////////
  /// h1d ////////////////////////////////
  ////////////////////////////////////////
 {
   tools::rgaussd rg(1,2);
   tools::histo::h1d h("Gauss",100,-5,5);
   for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),1.4);
   if(print) h.hprint(std::cout);
   //std::cout << " mean " << h.mean() << ", rms " << h.rms() << std::endl;

   /*
  {unsigned int nbin = h.axis().bins();
   double mn = h.axis().lower_edge();
   double mx = h.axis().upper_edge();}
   */

   // to access all bins data from methods :
  {const std::vector<unsigned int>& _entries = h.bins_entries();
   const std::vector<double>& _bins_sum_w = h.bins_sum_w();
   const std::vector<double>& _bins_sum_w2 = h.bins_sum_w2();
   const std::vector< std::vector<double> >& _bins_sum_xw = h.bins_sum_xw();
   const std::vector< std::vector<double> >& _bins_sum_x2w = h.bins_sum_x2w();
   // dump bin 50 (= in range bin 49) :
   std::cout << "entries[50] = " << _entries[50] << std::endl;
   std::cout << "  sum_w[50] = " << _bins_sum_w[50] << std::endl;
   std::cout << " sum_w2[50] = " << _bins_sum_w2[50] << std::endl;
   std::cout << " sum_xw[50] = " << _bins_sum_xw[50][0] << std::endl;   //0 = xaxis
   std::cout << "sum_x2w[50] = " << _bins_sum_x2w[50][0] << std::endl;  //0 = xaxis
   }

   // to get all internal data of an histo :
  {const tools::histo::h1d::hd_t& hdata = h.dac();  //dac=data access.
   const std::vector<unsigned int>& _entries = hdata.m_bin_entries;
   const std::vector<double>& _bins_sum_w = hdata.m_bin_Sw;
   const std::vector<double>& _bins_sum_w2 = hdata.m_bin_Sw2;
   const std::vector< std::vector<double> >& _bins_sum_xw = hdata.m_bin_Sxw;
   const std::vector< std::vector<double> >& _bins_sum_x2w = hdata.m_bin_Sx2w;
   // dump bin 50 :
   std::cout << "entries[50] = " << _entries[50] << std::endl;
   std::cout << "  sum_w[50] = " << _bins_sum_w[50] << std::endl;
   std::cout << " sum_w2[50] = " << _bins_sum_w2[50] << std::endl;
   std::cout << " sum_xw[50] = " << _bins_sum_xw[50][0] << std::endl;   //0 = xaxis
   std::cout << "sum_x2w[50] = " << _bins_sum_x2w[50][0] << std::endl;  //0 = xaxis
  }   
   // change in range bin 49 weight :
  {std::cout << "bin_Sw(49) = " << h.bin_Sw(49) << std::endl;   
   tools::histo::h1d::hd_t hdata = h.dac();  //dac=data access.
   hdata.m_bin_Sw[50] *= 2;
   h.copy_from_data(hdata);
   std::cout << "modified(*2) bin_Sw(49) = " << h.bin_Sw(49) << std::endl;   
  }
  
 }

 {
   tools::rbwd rbw(0,1);
   tools::histo::h1d h("BW",100,-5,5);
   for(unsigned int count=0;count<entries;count++) h.fill(rbw.shoot(),2.3);
   if(print) h.hprint(std::cout);
 }

  ////////////////////////////////////////
  /// p1d ////////////////////////////////
  ////////////////////////////////////////
 {
   tools::rgaussd rg(1,2);
   tools::rbwd rbw(0,1);
   tools::histo::p1d h("Profile",100,-5,5,-2,2);
   for(unsigned int count=0;count<entries;count++) {
     h.fill(rg.shoot(),rbw.shoot(),1);
   }
   if(print) h.hprint(std::cout);

   // to get all internal data :
   typedef tools::histo::profile_data<double,unsigned int,unsigned int,double,double> pd_t;
   pd_t hdata = h.get_histo_data(); 
 }

  ////////////////////////////////////////
  /// h2d ////////////////////////////////
  ////////////////////////////////////////
 {
   tools::rgaussd rg(1,2);
   tools::rbwd rbw(0,1);
   tools::histo::h2d h("Gauss_BW",100,-5,5,100,-2,2);
   for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),0.8);
   if(print) h.hprint(std::cout);

   if(print){
     std::cout << "x mean " << h.mean_x()
               << " rms " << h.rms_x() << std::endl;
     std::cout << "y mean " << h.mean_y()
               << " rms " << h.rms_y() << std::endl;
   }

   /*
  {unsigned int nbin = h.axis_x().bins();
   double mn = h.axis_x().lower_edge();
   double mx = h.axis_x().upper_edge();}

  {unsigned int nbin = h.axis_y().bins();
   double mn = h.axis_y().lower_edge();
   double mx = h.axis_y().upper_edge();}
   */

  {
    tools::histo::h1d* projection = tools::histo::projection_x(h,"ProjX");
    if(!projection) return -1;
    projection->set_title("Gauss_BW_projectionX");
    if(print) projection->hprint(std::cout);
    delete projection;
  }
   
  {
    tools::histo::h1d* projection = tools::histo::projection_y(h,"ProjY");
    if(!projection) return -1;
    projection->set_title("Gauss_BW_projectionY");
    if(print) projection->hprint(std::cout);
    delete projection;
  }

  {
    tools::histo::h1d* slice = tools::histo::slice_x(h,40,60,"SliceX");
    if(!slice) return -1;
    slice->set_title("Gauss_BW_sliceX");
    if(print) slice->hprint(std::cout);
    delete slice;
  }

  typedef tools::histo::axis<double,unsigned int> axis_t;
  {
    tools::histo::h1d* slice = tools::histo::slice_x(h,axis_t::UNDERFLOW_BIN,axis_t::UNDERFLOW_BIN,"SliceX");
    if(!slice) return -1;
    slice->set_title("Gauss_BW_sliceX_UNDER");
    if(print) slice->hprint(std::cout);
    delete slice;
  }

  {
    tools::histo::h1d* slice = tools::histo::slice_x(h,axis_t::OVERFLOW_BIN,axis_t::OVERFLOW_BIN,"SliceX");
    if(!slice) return -1;
    slice->set_title("Gauss_BW_sliceX_OVER");
    if(print) slice->hprint(std::cout);
    delete slice;
  }

  {
    tools::histo::h1d* slice = tools::histo::slice_y(h,30,50,"SliceY");
    if(!slice) return -1;
    slice->set_title("Gauss_BW_sliceY");
    if(print) slice->hprint(std::cout);
    delete slice;
  }

  {
    using namespace tools::histo; //playing with namespaces.
    h1d* slice = slice_y(h,axis_t::UNDERFLOW_BIN,axis_t::UNDERFLOW_BIN,"SliceY");
    if(!slice) return -1;
    slice->set_title("Gauss_BW_sliceY_UNDER");
    if(print) slice->hprint(std::cout);
    delete slice;
  }

  {
    namespace my_histo = tools::histo; //playing with namespaces.
    my_histo::h1d* slice = slice_y(h,tools::histo::axis_OVERFLOW_BIN,tools::histo::axis_OVERFLOW_BIN,"SliceY");
    if(!slice) return -1;
    slice->set_title("Gauss_BW_sliceY_OVER");
    if(print) slice->hprint(std::cout);
    delete slice;
  }

  {
    tools::histo::p1d* projection = tools::histo::profile_x(h,"ProfileX");
    if(!projection) return -1;
    projection->set_title("Gauss_BW_profileX");
    if(print) projection->hprint(std::cout);
    delete projection;
  }

  {
    tools::histo::p1d* projection = tools::histo::profile_y(h,"ProfileY");
    if(!projection) return -1;
    projection->set_title("Gauss_BW_profileY");
    if(print) projection->hprint(std::cout);
    delete projection;
  }

 }

  ////////////////////////////////////////
  /// h3d ////////////////////////////////
  ////////////////////////////////////////
 {
   tools::rgaussd rg(1,2);
   tools::rbwd rbw(0,1);
   tools::rtausmed rflat;
   tools::histo::h3d h("Gauss_BW_flat",100,-10,10,100,-2,2,100,-2,2);
   for(unsigned int count=0;count<entries;count++) {
     h.fill(rg.shoot(),rbw.shoot(),rflat.shoot());
   }
   if(print) h.hprint(std::cout);

   if(print){
     std::cout << "x mean " << h.mean_x()
               << " rms " << h.rms_x() << std::endl;
     std::cout << "y mean " << h.mean_y()
               << " rms " << h.rms_y() << std::endl;
     std::cout << "z mean " << h.mean_z()
               << " rms " << h.rms_z() << std::endl;
   }

  {
    tools::histo::h2d* projection = tools::histo::projection_xy(h,"ProjXY");
    if(!projection) return -1;
    projection->set_title("Gauss_BW_flat_projectionXY");
    if(print) projection->hprint(std::cout);
    
   {tools::histo::h1d* _projection =
      tools::histo::projection_x(*projection,"ProjX");
    if(!_projection) return -1;
    _projection->set_title("Gauss_BW_flat_projectionX");
    if(print) _projection->hprint(std::cout);
    delete _projection;}

   {tools::histo::h1d* _projection =
      tools::histo::projection_y(*projection,"ProjY");
    if(!_projection) return -1;
    _projection->set_title("Gauss_BW_flat_projectionY");
    if(print) _projection->hprint(std::cout);
    delete _projection;}

    delete projection;
  }
   
  {
    tools::histo::h2d* projection = tools::histo::projection_yz(h,"ProjYZ");
    if(!projection) return -1;
    projection->set_title("Gauss_BW_flat_projectionYZ");
    if(print) projection->hprint(std::cout);
    
   {tools::histo::h1d* _projection =
      tools::histo::projection_y(*projection,"ProjZ");
    if(!_projection) return -1;
    _projection->set_title("Gauss_BW_flat_projectionZ");
    if(print) _projection->hprint(std::cout);
    delete _projection;}

    delete projection;
  }
   
 }

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

#ifdef TOOLS_MEM
  }tools::mem::balance(std::cout);
#endif //TOOLS_MEM

  return 0;
}
