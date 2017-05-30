#include "wave.h"

#include <iostream>
#include <fstream>
#include <stdint.h>

#include <ao/ao.h>

#define DISPLAY_ERROR_MESSAGE
#define BUFFER_SIZE                 1024

/*
 * Download: http://www.linuxfromscratch.org/blfs/view/svn/multimedia/libao.html
 * Wave Format: http://www.topherlee.com/software/pcm-tut-wavformat.html
 * Wave Format2: http://astrogno.me/wav-playback.html
 *
 *
 */


 struct audio_s {
    ao_device *dev;
    int driver_id;
 };

 int audio_init(struct audio_s *ao)
 {
    ao_initialize();
    int driver_id=ao_default_driver_id();

    ao->driver_id = driver_id;
    ao->dev = NULL;
    return 0;
 }

 int audio_set_fmt(struct audio_s *ao, int bps=16, int rate=44100, int channel=2)
 {
    ao_sample_format fmt;

    fmt.bits = bps;
    fmt.rate = rate;
    fmt.channels = channel;
    fmt.byte_format = AO_FMT_NATIVE;
    fmt.matrix = 0;

    ao_device *dev = ao_open_live(ao->driver_id, &fmt, NULL);

    if (!dev) {
 #ifdef DISPLAY_ERROR_MESSAGE
        std :: cerr << "Cannot open Live AO" << std :: endl;
 #endif
        return -1;
    }
    ao->dev = dev;
    return 0;
 }

 int audio_play(struct audio_s *ao, char *buf, int length)
 {
    if (!ao->dev)
        return -1;
    return !ao_play(ao->dev, buf, length);
 }

 void audio_close(struct audio_s *ao)
 {
    if (!ao->dev)
        return;
    ao_close(ao->dev);
    ao_shutdown();
 }

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;
    struct audio_s ao;
    std :: fstream ao_file;
    char buffer[1024];

    ao_file.open(argv[1], std :: ios :: in | std :: ios :: binary);

    if (!ao_file.is_open())
        return -1;

    if (audio_init(&ao))
        return -1;
    ao_file.seekg(0, std::ios::beg);
    ao_file.read(buffer, WAVE_HEADER_SIZE);
    if (check_wave_header(buffer)) {
        std :: cerr << "Incorrect Header" << std :: endl;
        return -1;
    }
    if (audio_set_fmt(&ao, wave_get_bits_per_sample(buffer), wave_get_freq(buffer), wave_get_channel(buffer)))
        return -1;

    int wave_file_size = wave_get_audio_len(buffer);
    int full_read_count = wave_file_size / BUFFER_SIZE;
   
    for (int i=0; i<full_read_count; i++) {
        if (ao_file.eof())
            break;
        ao_file.read(buffer, BUFFER_SIZE);
        if (audio_play(&ao, buffer, BUFFER_SIZE)) {
#ifdef DISPLAY_ERROR_MESSAGE
            std :: cerr << "Failed to play" << std :: endl;
#endif  
            break;
        }
    }
    int left_bytes = wave_file_size % BUFFER_SIZE;
    ao_file.read(buffer, left_bytes);
    audio_play(&ao, buffer, left_bytes);
    audio_close(&ao);
    return 0;
}