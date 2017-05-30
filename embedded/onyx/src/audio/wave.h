#ifndef _WAVE_H_
#define _WAVE_H_

#define WAVE_HEADER_SIZE		44

int check_wave_header(char *buff);

int wave_get_channel(char *buff);
int wave_get_bps(char *buff);
int wave_get_bits_per_sample(char *buff);
int wave_get_audio_len(char *buff);
int wave_get_freq(char *buff);


#endif