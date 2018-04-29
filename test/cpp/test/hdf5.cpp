// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file tools.license for terms.

#include <tools/hdf5/T_tools>
#include <tools/hdf5/tools>

// Compound datatype :
#define LDS 10
#define LSTRING 5
struct S_A {
  unsigned int a;
  int b;
  float c;
  double ds[LDS];
  char s[LSTRING+1];
};
struct S_B {
  unsigned int d;
  S_A e;
};

bool build_S_B_file_type(hid_t& aDataType){
  hid_t S_A_type = H5Tcreate(H5T_COMPOUND,sizeof(S_A));
  if(S_A_type<0) return false;

  if(H5Tinsert(S_A_type,"a",HOFFSET(S_A,a),H5T_STD_U32LE)<0) {
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(S_A_type,"b",HOFFSET(S_A,b),H5T_STD_I32LE)<0) {
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(S_A_type,"c",HOFFSET(S_A,c),H5T_IEEE_F32LE)<0) {
    H5Tclose(S_A_type);
    return false;
  }

  hsize_t ds_dim[] = {LDS};
  hid_t ds_type = tools_H5Tarray_create(H5T_IEEE_F64LE,1,ds_dim,NULL);
  if(ds_type<0) {
    H5Tclose(S_A_type);
    return false;
  }

  if(H5Tinsert(S_A_type,"ds",HOFFSET(S_A,ds),ds_type)<0) {
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  hid_t s_type = tools::hdf5::string_datatype(LSTRING+1);
  if(s_type<0) {
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  if(H5Tinsert(S_A_type,"s",HOFFSET(S_A,s),s_type)<0) {
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  aDataType = H5Tcreate(H5T_COMPOUND,sizeof(S_B));
  if(aDataType<0) {
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(aDataType,"d",HOFFSET(S_B,d),H5T_STD_I32LE)<0) {
    H5Tclose(aDataType);
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(aDataType,"e",HOFFSET(S_B,e),S_A_type)<0) {
    H5Tclose(aDataType);
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  H5Tclose(s_type);
  H5Tclose(ds_type);
  H5Tclose(S_A_type);
  return true;
}

bool build_S_B_mem_type(hid_t& aDataType){
  hid_t S_A_type = H5Tcreate(H5T_COMPOUND,sizeof(S_A));
  if(S_A_type<0) return false;

  if(H5Tinsert(S_A_type,"a",HOFFSET(S_A,a),H5T_NATIVE_UINT)<0) {
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(S_A_type,"b",HOFFSET(S_A,b),H5T_NATIVE_INT)<0) {
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(S_A_type,"c",HOFFSET(S_A,c),H5T_NATIVE_FLOAT)<0) {
    H5Tclose(S_A_type);
    return false;
  }

  hsize_t ds_dim[] = {LDS};
  hid_t ds_type = tools_H5Tarray_create(H5T_NATIVE_DOUBLE,1,ds_dim,NULL);
  if(ds_type<0) {
    H5Tclose(S_A_type);
    return false;
  }

  if(H5Tinsert(S_A_type,"ds",HOFFSET(S_A,ds),ds_type)<0) {
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  hid_t s_type = tools::hdf5::string_datatype(LSTRING+1);
  if(s_type<0) {
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  if(H5Tinsert(S_A_type,"s",HOFFSET(S_A,s),s_type)<0) {
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  aDataType = H5Tcreate(H5T_COMPOUND,sizeof(S_B));
  if(aDataType<0) {
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(aDataType,"d",HOFFSET(S_B,d),H5T_NATIVE_INT)<0) {
    H5Tclose(aDataType);
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }
  if(H5Tinsert(aDataType,"e",HOFFSET(S_B,e),S_A_type)<0) {
    H5Tclose(aDataType);
    H5Tclose(s_type);
    H5Tclose(ds_type);
    H5Tclose(S_A_type);
    return false;
  }

  H5Tclose(s_type);
  H5Tclose(ds_type);
  H5Tclose(S_A_type);
  return true;
}

struct C_A {
  unsigned int ui;
  float f;
  unsigned int lds;
  double* ds;
  int i;
};

bool build_C_A_file_type(hid_t& aDataType){
  unsigned int sz = 4+4+4+LDS*8+4;
  hid_t C_A_type = H5Tcreate(H5T_COMPOUND,sz);
  if(C_A_type<0) return false;

  unsigned int offset = 0;

  if(H5Tinsert(C_A_type,"ui",offset,H5T_STD_U32LE)<0) {
    H5Tclose(C_A_type);
    return false;
  }
  offset += sizeof(unsigned int);

  if(H5Tinsert(C_A_type,"f",offset,H5T_IEEE_F32LE)<0) {
    H5Tclose(C_A_type);
    return false;
  }
  offset += sizeof(float);

  if(H5Tinsert(C_A_type,"lds",offset,H5T_STD_U32LE)<0) {
    H5Tclose(C_A_type);
    return false;
  }
  offset += sizeof(unsigned int);

  hsize_t ds_dim[] = {LDS};
  hid_t ds_type = tools_H5Tarray_create(H5T_IEEE_F64LE,1,ds_dim,NULL);
  if(ds_type<0) {
    H5Tclose(C_A_type);
    return false;
  }

  if(H5Tinsert(C_A_type,"ds",offset,ds_type)<0) {
    H5Tclose(ds_type);
    H5Tclose(C_A_type);
    return false;
  }
  offset += LDS*sizeof(double);

  if(H5Tinsert(C_A_type,"i",offset,H5T_STD_I32LE)<0) {
    H5Tclose(ds_type);
    H5Tclose(C_A_type);
    return false;
  }
  offset += sizeof(int);

  H5Tclose(ds_type);

  aDataType = C_A_type;

  return true;
}

class to_compare_hdf5 {
public:
  std::string title;
};

#include <tools/mathf>
#include <tools/mathd>

#include <tools/test>

//static int g_compress_level = 1;
//static int g_compress_level = 0;

static bool write_hdf5(std::ostream& a_out,bool a_verbose,to_compare_hdf5& a_cmp,const std::string& a_file) {
  if(a_verbose) a_out << "write_hdf5 ..." << std::endl;

  a_cmp.title = "My histo";

  // Data to store :
  unsigned int n = 1000000;
  int* array = new int[n];
  for(unsigned int index=0;index<n;index++) array[index] = index;
  S_B my_struct;
  my_struct.d = 316;
  my_struct.e.a = 314;
  my_struct.e.b = -315;
  my_struct.e.c = 3.14F;
  my_struct.e.ds[5] = 3.16;
  ::strcpy(my_struct.e.s,"hello"); //LSTRING = 5

  C_A my_object;
  my_object.ui = 314;
  my_object.f = 315.0F;
  my_object.lds = LDS;
  my_object.ds = new double[LDS];
  my_object.ds[5] = 316;
  my_object.i = 317;

  // Create the file :
  hid_t file = H5Fcreate(a_file.c_str(),H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,file,0)) return false;

  // Create directories (HDF5 groups) in the file :
  hid_t stat = tools_H5Gcreate(file,"stat",0);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,stat,0)) return false;

  hid_t histos = tools_H5Gcreate(stat,"histograms",0);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,histos,0)) return false;

  TOOLS_TEST_FUNC(tools::hdf5::write_atb(histos,"type","Directory"))
  int v = 314;
  TOOLS_TEST_FUNC(tools::hdf5::write_scalar_atb(histos,"version",v))

  hid_t shistos = tools_H5Gcreate(histos,"sub_histograms",0);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,shistos,0)) return false;
  
  hid_t tuples = tools_H5Gcreate(stat,"tuples",0);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,tuples,0)) return false;
  
  // Write data in directories :
  TOOLS_TEST_FUNC(tools::hdf5::write_array<int>(histos,"my_histo",n,array))
  TOOLS_TEST_FUNC(tools::hdf5::write_string(histos,"my_title",a_cmp.title))
  TOOLS_TEST_FUNC(tools::hdf5::write_array<int>(tuples,"my_tuple",n,array))

  std::vector<std::string> vs;
  vs.push_back("Line 1");
  vs.push_back("Line 2");
  vs.push_back("Line 3");
  vs.push_back("Line 4");
  TOOLS_TEST_FUNC(tools::hdf5::write_array_string(histos,"my_strings",vs))
  
  hid_t S_B_file_type;
  TOOLS_TEST_FUNC(build_S_B_file_type(S_B_file_type))

  hid_t S_B_mem_type;
  TOOLS_TEST_FUNC(build_S_B_mem_type(S_B_mem_type))

  TOOLS_TEST_FUNC(tools::hdf5::write_struct<S_B>(histos,"my_struct",S_B_file_type,S_B_mem_type,my_struct))
  H5Tclose(S_B_file_type);
  H5Tclose(S_B_mem_type);

  hid_t C_A_file_type;
  TOOLS_TEST_FUNC(build_C_A_file_type(C_A_file_type))

 {unsigned int sz = 4+4+4+LDS*8+4;
  std::vector<unsigned int> mszs(5); //Not used at writing.
  mszs[0] = 4;
  mszs[1] = 4;
  mszs[2] = 4;
  mszs[3] = LDS*8;
  mszs[4] = 4;

  char* buffer = new char[sz];

  //Fill buffer :
  char* p = buffer;
  ::memcpy(p,&my_object.ui,sizeof(unsigned int));
  p+=sizeof(unsigned int);

  ::memcpy(p,&my_object.f,sizeof(float)); 
  p+=sizeof(float);

  ::memcpy(p,&my_object.lds,sizeof(unsigned int)); 
  p+=sizeof(unsigned int);

  ::memcpy(p,my_object.ds,LDS*sizeof(double));  
  p += LDS*sizeof(double);

  ::memcpy(p,&my_object.i,sizeof(int));
  p += sizeof(int);

  TOOLS_TEST_FUNC(tools::hdf5::write_object(histos,"my_object",C_A_file_type,buffer))
  delete [] buffer;
  H5Tclose(C_A_file_type);}
  
  // Release HDF5 resources and close the file :
  H5Gclose(tuples);
  H5Gclose(shistos);
  H5Gclose(histos);
  H5Gclose(stat);
  H5Fclose(file);
  
  delete [] my_object.ds;
  delete [] array;

  if(a_verbose) a_out << "write_hdf5 : end." << std::endl;

  return true;
}

static bool read_hdf5(std::ostream& a_out,bool a_verbose,const to_compare_hdf5& a_cmp,const std::string& a_file) {
  if(a_verbose) a_out << "read_hdf5 ..." << std::endl;

  // Open the file :
  hid_t file = H5Fopen(a_file.c_str(),H5F_ACC_RDONLY, H5P_DEFAULT);
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,file,0)) return false;

  // Open a directory (group) :
  hid_t histos = tools_H5Gopen(file,"/stat/histograms");
  if(!tools::ge<hid_t>(a_out,__FILE__,__LINE__,histos,0)) return false;

  std::string stype;
  TOOLS_TEST_FUNC(tools::hdf5::read_atb(histos,"type",stype))
  if(!tools::equal(a_out,__FILE__,__LINE__,stype,"Directory")) return false;
 {int v;
  TOOLS_TEST_FUNC(tools::hdf5::read_atb(histos,"version",v))
  if(!tools::equal(a_out,__FILE__,__LINE__,v,314)) return false;}

  // Get a data in the directory :
  unsigned int n = 0;
  int* array = 0;
  TOOLS_TEST_FUNC(tools::hdf5::read_array<int>(histos,"my_histo",n,array))

  std::vector<std::string> vs;
  TOOLS_TEST_FUNC(tools::hdf5::read_array_string(histos,"my_strings",vs))

  std::string rtitle;
  TOOLS_TEST_FUNC(tools::hdf5::read_string(histos,"my_title",rtitle))
  
  S_B s;
  hid_t S_B_mem_type;
  TOOLS_TEST_FUNC(build_S_B_mem_type(S_B_mem_type))

  TOOLS_TEST_FUNC(tools::hdf5::read_struct<S_B>(histos,"my_struct",S_B_mem_type,s))
  H5Tclose(S_B_mem_type);

  C_A my_object;
  my_object.ds = 0;
  my_object.i = 0;
  
  size_t sz = 0;
  char* buffer = 0;
  TOOLS_TEST_FUNC(tools::hdf5::read_object(histos,"my_object",sz,buffer))

  char* p = buffer;

  ::memcpy(&my_object.ui,p,sizeof(unsigned int));    
  p += sizeof(unsigned int);

  ::memcpy(&my_object.f,p,sizeof(float));
  p += sizeof(float);
 
  ::memcpy(&my_object.lds,p,sizeof(unsigned int));    
  p += sizeof(unsigned int);

  my_object.ds = new double[my_object.lds];
  ::memcpy(my_object.ds,p,LDS*sizeof(double)); 
  p += LDS*sizeof(double);

  ::memcpy(&my_object.i,p,sizeof(int));
  p += sizeof(int);

  delete [] buffer;

  // Release HDF5 resources and close the file :
  H5Gclose(histos);
  H5Fclose(file);

  //////////////////////////////////////////
  //// Use the data ////////////////////////
  //////////////////////////////////////////
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,n,(unsigned int)1000000)) return false;

  int array_314 = array[314];
  if(!tools::equal(a_out,__FILE__,__LINE__,array_314,int(314))) return false;

  /*
 {char* ba = (char*)&array_314;
  int array_314_swapped;
  char* bsa = (char*)&array_314_swapped;
  bsa[0] = ba[3];
  bsa[1] = ba[2];
  bsa[2] = ba[1];
  bsa[3] = ba[0];
  //out << "main :"
  //    << " swapped is : "  << array_314_swapped
  //    << std::endl;
  }
  */

  if(!tools::equal(a_out,__FILE__,__LINE__,rtitle,a_cmp.title)) return false;
  if(!tools::equal<size_t>(a_out,__FILE__,__LINE__,vs.size(),size_t(4))) return false;
  if(!tools::equal(a_out,__FILE__,__LINE__,vs[2],"Line 3")) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,s.d,(unsigned int)316)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,s.e.a,(unsigned int)314)) return false;
  if(!tools::equal<int>(a_out,__FILE__,__LINE__,s.e.b,-315)) return false;
  if(!tools::equal<float>(a_out,__FILE__,__LINE__,s.e.c,3.14F,0.0001F,tools::ffabsc)) return false;
  if(!tools::equal<double>(a_out,__FILE__,__LINE__,s.e.ds[5],3.16,0.0001,tools::dfabs)) return false;
  if(!tools::equal<int>(a_out,__FILE__,__LINE__,::strcmp(s.e.s,"hello"),0)) return false;
  if(!tools::equal<unsigned int>(a_out,__FILE__,__LINE__,my_object.ui,(unsigned int)314)) return false;
  if(!tools::equal(a_out,__FILE__,__LINE__,my_object.f,315.0F,0.0001F,tools::ffabsc)) return false;
 {double v = my_object.ds?my_object.ds[5]:0;
  if(!tools::equal(a_out,__FILE__,__LINE__,v,316.0,0.0001,tools::dfabs)) return false;}
  if(!tools::equal(a_out,__FILE__,__LINE__,my_object.i,317)) return false;

  delete [] my_object.ds;
  delete [] array;

  if(a_verbose) a_out << "read_hdf5 : end." << std::endl;

  return true;
}

#include <tools/test>
#include "../sys/dir"
#include "../sys/process"
#include <tools/file>
#include <tools/num2s>
#include <tools/serrno>

bool test_hdf5(std::ostream& a_out,bool a_verbose) {
  if(a_verbose) {
    std::string pwd;
    tools::dir::pwd(pwd);
    a_out << "pwd : " << pwd << std::endl;
  }

  std::string file;
  if(!tools::num2s(tools::process_id(),file)) {}
  file = "utest_"+file+".hdf5";
  if(a_verbose) a_out << "file : " << file << std::endl;

  to_compare_hdf5 _cmp;

  if(!write_hdf5(a_out,a_verbose,_cmp,file)) return false;
  if(!read_hdf5(a_out,a_verbose,_cmp,file)) return false;

  TOOLS_TEST_FUNC(tools::file::std_remove(file))

  return true;
}
