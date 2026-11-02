/* * * * * * * * * * * * *
Haydn Content
Assignment 3 - WAVE rw
CS 245
Spring 2026
* * * * * * * * * * * * */

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include "AudioData.h"


/*
AudioData(fname) — (class constructor) creates an audio data object by reading the
contents of the WAVE file named fname.
//
If the file is not a valid WAVE file, a runtime error exception is thrown
(this is a standard exception defined in the standard C++ header file stdexcept).
//
On success, an audio data object is created whose data is filled in with the WAVE file audio data
converted to floating point values in the range [−1, 1].
*//*
WAVE file validation. The AudioData constructor should throw an exception if the input
file is not a valid WAVE file. Specifically, your implementation should check for valid RIFF,
format, and data chunks, as well as the presence of the WAVE tag. For the format chunk,
format fields should be checked for valid and consistent values.
*/
AudioData::AudioData(const char* fname)
	: frame_count(0)
	, sampling_rate(0)
	, channel_count(0)
{
	try
	{
		std::fstream in(fname, std::ios_base::in | std::ios_base::binary);
		if (!in.is_open())
			throw std::runtime_error("unable to open file:");

		char header[44];
		in.read(header, 44);

		// RIFF label check
		char riff_label[4] = { 'R', 'I', 'F', 'F' };
		for (int i = 0; i < 4; ++i)
		{
			if (header[i] != riff_label[i])
				throw std::runtime_error("RIFF label missing");
		}

		// WAVE label check
		char wave_label[4] = { 'W', 'A', 'V', 'E' };
		for (int i = 0; i < 4; ++i)
		{
			if (header[i + 8] != wave_label[i])
				throw std::runtime_error("WAVE label missing");
		}

		// fmt_label check
		char fmt_label[4] = { 'f', 'm', 't', ' ' };
		for (int i = 0; i < 4; ++i)
		{
			if (header[i + 12] != fmt_label[i])
				throw std::runtime_error("fmt label missing");
		}

		// get integer data
		unsigned size = *reinterpret_cast<uint32_t*>(header + 40);
		uint32_t fmt_size = *reinterpret_cast<uint32_t*>(header + 16);
		uint16_t audio_format = *reinterpret_cast<uint16_t*>(header + 20);
		uint16_t channel_cnt = *reinterpret_cast<uint16_t*>(header + 22);
		uint32_t sampling_rt = *reinterpret_cast<uint32_t*>(header + 24);
		uint32_t bps = *reinterpret_cast<uint32_t*>(header + 28);
		uint16_t bytes_per_frame = *reinterpret_cast<uint16_t*>(header + 32);
		uint16_t bit_depth = *reinterpret_cast<uint16_t*>(header + 34);

		// data label check
		char data_label[4] = { 'd', 'a', 't', 'a' };
		for (int i = 0; i < 4; ++i)
		{
			if (header[i + 36] != data_label[i])
				throw std::runtime_error("data label missing");
		}

		// assign class data
		this->frame_count = size / bytes_per_frame;
		this->sampling_rate = sampling_rt;
		this->channel_count = channel_cnt;
		this->fdata = std::vector<float>(frame_count * channel_count);

		// check bit depth
		unsigned iterations = frame_count * channel_count;
		if (bit_depth == 8)
		{
			if (channel_count == 1 || channel_count == 2)
			{
				uint8_t* data = new uint8_t[size];
				in.read(reinterpret_cast<char*>(data), size);

				for (unsigned i = 0; i < iterations; ++i)
					fdata[i] = (static_cast<float>(data[i])-128.0f) / 128.0f;

				delete[] data;
			}
		}
		else if (bit_depth == 16)
		{
			if (channel_count == 1 || channel_count == 2)
			{
				int16_t* data = new int16_t[size / 2];
				in.read(reinterpret_cast<char*> (data), size);

				for (unsigned i = 0; i < iterations; ++i)
					fdata[i] = (static_cast<float>(data[i])) / (32768.0f);

				delete[] data;
			}
		}
	}
	catch (std::exception& exc)
	{
		std::cout << "invalid WAVE " << exc.what() << std::endl;
		throw;
	}
}

/*
waveWrite(fname,ad,bits) — write a WAVE file. Audio data from the AudioData object
ad is written to a WAVE file named fname. The data in ad are assumed to be floating
point values in the range [−1, 1], and are written to the output file using the bit
resolution specified by bits. The function caller is responsible for ensuring that the 1
passed–in data is in the range [−1, 1]: no clipping is performed. The function returns
true on success, and false otherwise; e.g., if the value of bits is not a recognized
value (see below).
*/
bool waveWrite(const char* fname, const AudioData& ad, unsigned bits)
{
	std::ofstream out(fname, std::ios_base::out | std::ios_base::binary);
	if (!out.is_open()) return false;

	const float * data = ad.data();
	unsigned frames = ad.frames();

	/*
	char riff_label[4]; // (00) = {’R’,’I’,’F’,’F’}
	uint32_t riff_size; // (04) = 36 + data_size
	char file_tag[4]; // (08) = {’W’,’A’,’V’,’E’}
	char fmt_label[4]; // (12) = {’f’,’m’,’t’,’ ’}
	uint32_t fmt_size; // (16) = 16
	uint16_t audio_format; // (20) = 1
	uint16_t channel_count; // (22) = 1 or 2
	uint32_t sampling_rate; // (24) = (anything)
	uint32_t bytes_per_second; // (28) = (see above)
	uint16_t bytes_per_frame; // (32) = (see above)
	uint16_t bits_per_sample; // (34) = 8 or 16
	char data_label[4]; // (36) = {’d’,’a’,’t’,’a’}
	uint32_t data_size; // (40) = # bytes of data
	*/

	char header[44] = {0};
	// labels
	char riff_label[4] = { 'R', 'I', 'F', 'F' };
	char file_tag[4] = { 'W', 'A', 'V', 'E' };
	char fmt_label[4] = { 'f', 'm', 't', ' ' };
	char data_label[4] = { 'd', 'a', 't', 'a' };
	for (int i = 0; i < 4; ++i) header[i] = riff_label[i];
	for (int i = 0; i < 4; ++i) header[i + 8] = file_tag[i];
	for (int i = 0; i < 4; ++i) header[i + 12] = fmt_label[i];
	for (int i = 0; i < 4; ++i) header[i + 36] = data_label[i];

	// data
	uint32_t sampling_rate = ad.rate();
	uint16_t channel_count = ad.channels();
	uint32_t data_size = (bits == 16) ? (2*frames*channel_count) /* 16-bit */ : (frames * channel_count) /* 8-bit */;
	uint32_t riff_size = 36+data_size;
	uint32_t fmt_size = 16;
	uint16_t audio_format = 1;
	uint32_t bytes_per_second = (sampling_rate * channel_count * (bits / 8));
	uint16_t bytes_per_frame = (channel_count * (bits / 8));

	*reinterpret_cast<uint32_t*>(header + 4) = riff_size; 
	*reinterpret_cast<uint32_t*>(header + 16) = fmt_size;
	*reinterpret_cast<uint16_t*>(header + 20) = audio_format;
	*reinterpret_cast<uint16_t*>(header + 22) = channel_count;
	*reinterpret_cast<uint32_t*>(header + 24) = sampling_rate;
	*reinterpret_cast<uint32_t*>(header + 28) = bytes_per_second;
	*reinterpret_cast<uint16_t*>(header + 32) = bytes_per_frame;
	*reinterpret_cast<uint16_t*>(header + 34) = bits;
	*reinterpret_cast<uint32_t*>(header + 40) = data_size;

	// write header
	out.write(header, 44);

	// raw audio data operations
	unsigned iterations = channel_count * frames;
	if (bits == 8)
	{
		uint8_t* outData = new uint8_t[data_size];

		if (channel_count == 1 || channel_count == 2)
		{
			for (unsigned i = 0; i < iterations; ++i)
				outData[i] = static_cast<uint8_t>((data[i] * 127.0f) + 128);
		}
		else
		{
			std::cout << "bit depth " << bits << " not supported" << std::endl;
			return false;
		}
		
		// write data
		out.write(reinterpret_cast<const char*>(outData), data_size);
		delete[] outData;
	}
	else if (bits == 16) 
	{
		int16_t* outData = new int16_t[iterations];
		const float multiplier = (1 << 15) - 1;

		if (channel_count == 1 || channel_count == 2)
		{
			for (unsigned i = 0; i < iterations; ++i)
				outData[i] = static_cast<int16_t>(data[i] * multiplier);
		}
		else
		{
			std::cout << "bit depth " << bits << " not supported" << std::endl;
			return false;
		}

		// write data
		out.write(reinterpret_cast<const char*>(outData), data_size);
		delete[] outData;
	}
	else
	{
		std::cout << "channel count " << channel_count << " not supported" << std::endl;
		return false;
	}

	return true;
}