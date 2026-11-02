// Haydn Content
// MAT320
// Fall 2026
// Project 4 - lowpass.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// y_t = x_t + a*x_{t-1}
class lowpass
{ 
public:
    lowpass(float a_) : a(a_), x1(0.f) {}
    float operator()(float x) 
    { 
        double y=x+(a*x1);
        x1=x;
        return float(y); 
    }
private:
    float a;
    float x1;
};

void LowPassFilter(vector<float> & signal, float coef)
{
    lowpass lpf(coef);
    for(unsigned i=0; i<signal.size(); ++i)
        signal[i]=lpf(signal[i]);
}

void Normalize(vector<float> & signal)
{
    float max=0.f;
    for(unsigned i=0; i<signal.size(); ++i)
        if (abs(signal[i])>max) max=signal[i];
    float factor=float(1<<15)/max;
    for(unsigned i=0; i<signal.size(); ++i)
        signal[i]*=factor;
}

// args:
// coeffcient       a1
// times to run     n
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
    unsigned size = *reinterpret_cast<unsigned*>(header+40),rate = *reinterpret_cast<unsigned*>(header+24);
    char *data = new char[size];
    in.read(data,size);

    cout << "The sample rate is:  " << rate << endl;
    cout << "The data size is:  "   << size << endl;

    // convert to floating point data
    vector<float> fdata(size);
    for(unsigned i=0; i<size; ++i)
	    fdata[i] = static_cast<float>(data[i]) / float(1<<15);

    // apply lowpass filter n times
    for(int i=0; i<N; ++i)
        LowPassFilter(fdata,filter_coef);

    // write output wav file
    Normalize(fdata);
    short *samples = new short[size];
    for (unsigned i=0; i<size; ++i) 
        samples[i]=static_cast<short>(fdata[i]*float(1<<15));

    //string out_filename = "lowpass_" + N + '_' + in_filename + ".wav";
    fstream out("output.wav",ios_base::binary|ios_base::out);
    out.write(header,44);
    out.write(reinterpret_cast<char*>(samples),size);

    delete[] data;
    delete[] samples;

    return 0;
}