// fourier_envelope.cpp
// -- compute amplitude envelope
// jsh 9/11
//
// usage:
//   fourier_envelope <freq> <file>
// where:
//   <freq> -- frequency of partial (in Hz)
//   <file> -- input file (WAVE, 16bit mono)
// output:
//   'fourier_envelope.wav'

#include <fstream>
// #include <algorithm>
#include <complex>
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 3)
    return 0;
  fstream in(argv[2],ios_base::binary|ios_base::in);
  if (!in)
    return 0;

  char header[44];
  in.read(header,44);
  unsigned size = *reinterpret_cast<unsigned*>(header+40),
           rate = *reinterpret_cast<unsigned*>(header+24),
           count = size/2;
  char *data = new char[size];
  in.read(data,size);

  cout << "The sample rate is:  " << rate << endl;
  cout << "The data size is:  " << size << endl;

  // window/hop size
  const float PI = 4.0f*atan(1.0f),
              TWOPI = 2.0f*PI;
  const int N = 1 << 11,
            H = N >> 1;

  // Han window (raised cosine window function)
  float *W = new float[N];
  for (int i=0; i < N; ++i)
    W[i] = 0.5f*(1 - cos(i*TWOPI/(N-1)));

  // find coefficient for each window
  float frequency = float(atof(argv[1])),
        DT = TWOPI*frequency/float(rate);
  unsigned wcount = (count - N)/H;
  float *coef = new float[wcount];
  short *samples = reinterpret_cast<short*>(data);
  // This is the window loop
  for (unsigned j=0; j < wcount; ++j) {
    complex<float> z = 0;
    for (int t=0; t < N; ++t) {
      complex<float> itx(0,t*DT);
      z += W[t]*float(samples[j*H+t])*exp(-itx);
    }
    coef[j] = abs(z);
  }

  // convert to WAVE file
  float const norm = 2.0f/float(N),
              MAX = float((1<<15)-1);
  for (unsigned i=0; i < count; ++i) {
    int k = 0.5f*(2*i - N)/H,
        kp1 = k + 1;
    if (k < 0)
      k = kp1 = 0;
    if (kp1 >= int(wcount))
      k = kp1 = int(wcount) - 1;
    float x = (i - 0.5f*N)/H - k,
          y = norm*(coef[k] + (coef[kp1] - coef[k])*x);
    samples[i] = short(min(MAX,abs(y)));
  }

  fstream out("fourier_envelope.wav",ios_base::binary|ios_base::out);
  out.write(header,44);
  out.write(data,size);

  delete[] coef;
  delete[] data;
  return 0;
}

