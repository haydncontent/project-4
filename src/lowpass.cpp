// Haydn Content
// MAT320
// Fall 2026
// Project 4 - lowpass.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "complex.h"

using namespace std;
using namespace IMath;

void LowPassFilter(ComplexVector & signal)
{

}

// args:
// coeffcient       a1
// timest to run    n
// audio file       input.wav
int main(int argc, char* argv[])
{
    if (argc!=4) {cerr<<"incorrect number of args, expecting ./lowpass <a1> <n> <input.wav>"<<endl; return -1;}

    int filter_coef = atoi(argv[1]);
    int N = atoi(argv[2]);
    string in_filename(argv[3]);

    fstream in(in_filename,ios_base::binary|ios_base::in);
    if (!in) {cerr<<"bad file"<<endl; return -1;}

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

    // read input wav file



    // apply lowpass filter n times

    float *W = new float[N];
    for (int i=0; i < N; ++i)
        W[i] = 0.5f*(1 - cos(i*TWOPI/(N-1)));

    // find coefficient for each window
    float frequency = float(atof(argv[1])), DT = TWOPI*frequency/float(rate);
    unsigned wcount = (count - N)/H;
    float *coef = new float[wcount];
    short *samples = reinterpret_cast<short*>(data);
    // This is the window loop
    for (unsigned j=0; j < wcount; ++j) 
    {
        Complex z = 0;
        for (int t=0; t < N; ++t) 
        {
            Complex itx(0,t*DT);
            z += W[t]*float(samples[j*H+t])*exp(-itx);
        }
        coef[j] = abs(z);
    }

    // write output wav file
    float const norm = 2.0f/float(N), MAX = float((1<<15)-1);
    for (unsigned i=0; i < count; ++i) 
    {
        int k = 0.5f*(2*i - N)/H, kp1 = k + 1;
        if (k < 0)
            k = kp1 = 0;
        if (kp1 >= int(wcount))
            k = kp1 = int(wcount) - 1;
        
        float x = (i - 0.5f*N)/H - k, y = norm*(coef[k] + (coef[kp1] - coef[k])*x);
        samples[i] = short(min(MAX,abs(y)));
    }


    string out_filename = "lowpass_" + N + '_' + in_filename + ".wav";

    fstream out(out_filename,ios_base::binary|ios_base::out);
    out.write(header,44);
    out.write(data,size);

    delete[] data;
    
    return 0;
}