/**
* @version    GrPPI v0.1
* @copyright    Copyright (C) 2017 Universidad Carlos III de Madrid. All rights reserved.
* @license    GNU/GPL, see LICENSE.txt
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License in LICENSE.txt
* also available in <http://www.gnu.org/licenses/gpl.html>.
*
* See COPYRIGHT.txt for copyright notices and details.
*/
// Standard library
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>
#include <numeric>
#include <stdexcept>
#include <cctype>
#include <cmath>
// grppi
#include "grppi.h"

// Samples shared utilities
#include "../../util/util.h"





void capitalize(grppi::parallel_execution_omp & e/*, 
                std::istream & in, std::ostream & out*/)
{
  using namespace std;
  using namespace experimental;
  
  //INIT FMRADIOCORE
  float rate = 250000000; // 250 MHz sampling rate is sensible
  float cutoff = 108000000; //guess... doesn't FM freq max at 108 Mhz? 
  int taps = 64;
  float max = 27000;
  float bandwidth = 10000;
  int decimation = 4;
  // determine where equalizer cuts.  Note that <eqBands> is the
  // number of CUTS; there are <eqBands>-1 bands, with parameters
  // held in slots 1..<eqBands> of associated arrays.
  int eqBands = 5;
  float eqCutoff[eqBands];
  float eqGain[eqBands];
  float low = 55;
  float high = 1760;
  float pi = 3.1416;

  for (int i=0; i<eqBands; i++) {
     // have exponentially spaced cutoffs
     eqCutoff[i] = (float)exp(i*(log(high)-log(low))/(eqBands-1) + log(low));
  }

  // first gain doesn't really correspond to a band
  eqGain[0] = 0;
  for (int i=1; i<eqBands; i++) {
     // the gain grows linearly towards the center bands
    float val = (((float)(i-1))-(((float)(eqBands-2))/2.0)) / 5.0;
    eqGain[i] = val > 0 ? 2.0-val : 2.0+val;
  }

  //INIT BANDLOWPASS
  std::vector<std::vector<float>> coeffs (eqBands, std::vector<float>(taps,0.0)); 
  for(int b = 0; b< eqBands; b++){
    int i;
    float m = taps - 1;
    float w = 2 * pi * eqCutoff[b] / rate;
    for (i = 0; i < taps; i++) {
      if (i - m/2 == 0)
        coeffs[b][i] = w/pi;
      else
        coeffs[b][i] = sin(w*(i-m/2)) / pi / (i-m/2) *
          (0.54 - 0.46 * cos(2*pi*i/m));
    }
  } 

  //INIT LOW PASS
  float coeff[taps];
  int i;
  float m = taps - 1;
  float w = 2 * pi * cutoff / rate;
  for (i = 0; i < taps; i++) {
    if (i - m/2 == 0)
      coeff[i] = w/pi;
    else
      coeff[i] = sin(w*(i-m/2)) / pi / (i-m/2) *
        (0.54 - 0.46 * cos(2*pi*i/m));
  }

  
  
  //DEMODULATOR INIT
  float mGain = max*(rate/(bandwidth*pi));


  int end = 4*2*64*10;
  float x = 0;


  grppi::pipeline(e,
    [&]() -> optional<std::vector<float>> {
       x++;
       if(x==end) return {};
       std::vector<float> o;
       o.push_back(x);
       return {o};
    },
//    grppi::window(grppi::count_based<float>(1,1)), //64,1
        //Band_pass_filter 
        grppi::split_join(grppi::duplicate{},
          /* grppi::pipeline(
           grppi::split_join(grppi::duplicate{},
           //Low pass filter
           [&coeffs](auto window){
//           [&coeffs](auto window)->float{
              float sum = window[0];
              return sum;
           },
           //Low pass filter
           [&coeffs](auto window){
              return window[0];
           }
           ),
           grppi::window(grppi::count_based<float>(1,1)), //64,1
           [](auto win){ return win[0]+win[1];}
           ),
           */
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];},
           [](auto win){return win[0];}
    ),
    [&](float a){
      std::cout<<a<<std::endl;
    }
  );
}

void print_message(const std::string & prog, const std::string & msg) {
  using namespace std;

  cerr << msg << endl;
  cerr << "Usage: " << prog << " mode " << endl;
/*  cerr << "  input: Input file name" << endl;
  cerr << "  output: Output file name" << endl;*/
  cerr << "  mode:" << endl;
  print_available_modes(cerr);
}


int main(int argc, char **argv) {
    
  using namespace std;

  if(argc < 2){
    print_message(argv[0], "Invalid number of arguments.");
    return -1;
  }

/*  ifstream in{argv[2]};
  if (!in) {
    print_message(argv[0], "Cannot open file "s + argv[2]);
    return -2;
  }

  ofstream out{argv[3]};
  if (!out) {
    print_message(argv[0], "Cannot open file "s + argv[3]);
    return -2;
  }*/
/*
  if (!run_test(argv[1], capitalize, in, out)) {
    print_message(argv[0], "Invalid policy.");
    return -1;
  }
*/
  //auto e = grppi::parallel_execution_native{};
  auto e = grppi::parallel_execution_omp{};
  capitalize(e);
  return 0;
}