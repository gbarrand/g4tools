
#include "Riostream.h"
#include <stdio.h>

void read_wcsv_csv() {

  ifstream in;
  in.open("wcsv.csv");

  TH1D* hrg = new TH1D("hrg","rgauss distribution",100,-4,4);

  std::string line;
  unsigned int index;
  double rgauss,rbw;
  char sep;
  unsigned int nlines = 0;
  while(!in.eof()) {
    std::getline(in,line);
    if(line.substr(0,1)=="#") continue;
    if(::sscanf(line.c_str(),"%u,%lg,%lg",&index,&rgauss,&rbw)==3) {
      if(nlines<10) printf("index=%d, rgauss=%g, rbw=%g\n",index,rgauss,rbw);
      hrg->Fill(rgauss);
    }
    nlines++;
  }

  in.close();

  /////////////////////////////////////////////////
  /// plotting : //////////////////////////////////
  /////////////////////////////////////////////////
  TCanvas* plotter = new TCanvas("canvas","",10,10,800,600);
  plotter->Divide(1,1);  
  plotter->cd(1);
  hrg->Draw();
  plotter->Update();

    
}
