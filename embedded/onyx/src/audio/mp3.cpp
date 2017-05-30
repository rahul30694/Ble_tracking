#include "mp3.h"

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <mpg123.h>

/*
 * Must Read: http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html
 * libmpg123 API Description: https://www.mpg123.de/api/modules.shtml
 * 
 */

 struct __mp3_handler_s {
 	mpg123_handle *h;
 	int err;
 };

 /*
  * mpg123_init(): Function to initialise the mpg123 library. This function is not thread-safe.
  * mpg123_new(const char *decoder, int *error ): Create a handle with optional choice of decoder and optional retrieval of an error code. 
  */


 int mp3_init(struct mp3_handler_s *m)
 {
 	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
 	mpg123_init();
    mh->h = mpg123_new(NULL, &(mh->err));
    if (!mh->h)
    	return -1;
    return 0;
 }

 int mp3_open_file(struct mp3_handler_s *m, const char *file)
 {
 	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
 	if (mpg123_open(mh->h, file) != MPG123_OK)
 	 	return -1;
 	return 0;
 }

 int mp3_get_format(struct mp3_handler_s *m, long *rate, int *chan)
 {
 	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
 	int enc;
 	if (mpg123_getformat(mh->h, rate, chan, &enc) != MPG123_OK)
 		return -1;
 	return 0;
 }

 int mp3_get_bytes_per_sample(struct mp3_handler_s *m)
 {
 	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
 	long rate;
 	int enc, chan;

 	if (mpg123_getformat(mh->h, &rate, &chan, &enc) != MPG123_OK)
 		return -1;

 	int bps = mpg123_encsize(enc);
 	return bps;
 }

int mp3_get_buffer_size(struct mp3_handler_s *m)
{
	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
	int s = mpg123_outblock(mh->h);	
	return s;
}

int mp3_get_decoded(struct mp3_handler_s *m, unsigned char *dest, size_t dest_size, size_t *decoded_size)
{
	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
	if (mpg123_read(mh->h, dest, dest_size, decoded_size) != MPG123_OK)
		return -1;
	return 0;
}

void mp3_change_vol_to(struct mp3_handler_s *m, double vol)
{
	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
	mpg123_volume(mh->h, vol);
}

int mp3_close_file(struct mp3_handler_s *m)
{
	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
	if (mpg123_close(mh->h) != MPG123_OK)
		return -1;
	return 0;
}

void mp3_stop(struct mp3_handler_s *m)
{
	struct __mp3_handler_s *mh = (struct __mp3_handler_s *)m;
	mpg123_delete(mh->h);
    mpg123_exit();
}

//#define __MAIN__


#ifdef __MAIN__
int main()
{
	return 0;
}
#endif




