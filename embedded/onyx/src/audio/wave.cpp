#include "wave.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

const char VALID_ID[]="RIFF";
const char VALID_WAVE_FMT[]="WAVEfmt ";

struct wave_header_s {
    char id[4]; 		//Should contain RIFF
    int32_t length;
    char wavefmt[8];
    int32_t format; 	// 16 for PCM
    int16_t pcm; 		// 1 for PCM
    int16_t channels;
    int32_t frequency;
    int32_t bytesPerSecond;
    int16_t bytesByCapture;
    int16_t bitsPerSample;
    char data[4];		 // "data"
    int32_t bytesInData;
}__attribute__((packed));



int check_wave_header(char *buff)
{
	if (sizeof(struct wave_header_s) != WAVE_HEADER_SIZE) {
		std :: cerr << "Size doesn't match" << std :: endl;
		return -1;
	}
	struct wave_header_s *h = reinterpret_cast<struct wave_header_s *>(buff);
	if (memcmp(h->id, VALID_ID, strlen(VALID_ID)))
		return -1;
	if (memcmp(h->wavefmt, VALID_WAVE_FMT, strlen(VALID_WAVE_FMT)))
		return -1;
	return 0;
}

int wave_get_channel(char *buff)
{
	struct wave_header_s *h = reinterpret_cast<struct wave_header_s *>(buff);
	std :: cout << "Channel: " << h->channels << std :: endl;
	return h->channels;
}

int wave_get_bps(char *buff)
{
	struct wave_header_s *h = reinterpret_cast<struct wave_header_s *>(buff);
	return h->bytesPerSecond;
}

int wave_get_bits_per_sample(char *buff)
{
	struct wave_header_s *h = reinterpret_cast<struct wave_header_s *>(buff);
	std :: cout << "Bit per sample: " << h->bitsPerSample << std :: endl;
	return h->bitsPerSample;
}

int wave_get_audio_len(char *buff)
{
	struct wave_header_s *h = reinterpret_cast<struct wave_header_s *>(buff);
	return h->bytesInData;
}

int wave_get_freq(char *buff)
{
	struct wave_header_s *h = reinterpret_cast<struct wave_header_s *>(buff);
	std :: cout << "Freq: " << h->frequency << std :: endl;
	return h->frequency;
}

#ifdef __MAIN__
int main()
{
	return 0;
}
#endif
