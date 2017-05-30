#include "video.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <linux/videodev2.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>

enum camera_format_e {
	PIX_FMT_MJPEG = DEFAULT_PIX_FORMAT,
	FORMAT_SENTIMEL
};

/* 
 * Website: https://jwhsmith.net/2014/12/capturing-a-webcam-stream-using-v4l2/
 *
 */
int _vfd = -1;
void *_buffer_ptr=NULL;
int _buffer_offset=0;
int _buffer_length=0;
bool _capturing=false;

int camera_init(char *dev)
{
	if (_vfd < 0)
		_vfd = open(dev, O_RDWR);

	if (_vfd < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Invalid File Name: " << dev << std :: endl;
#endif
		return -1;
	}

	//Video Query Capabilities IOCTL System Call
	struct v4l2_capability cap;
	if (ioctl(_vfd, VIDIOC_QUERYCAP, &cap) < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Unable to get capabilities of the Device" << std :: endl;
#endif
		goto ERROR;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "The Device cannot handle video streaming" << std :: endl;
#endif
		goto ERROR;
	}

	return 0;

ERROR:
	close(_vfd);
	return -1;
}

int camera_set_format(enum camera_format_e pix_format=PIX_FMT_MJPEG, int width=DEFAULT_PIX_WIDTH, int height=DEFAULT_PIX_HEIGHT)
{
	if (_vfd < 0)
		return -1;
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	format.fmt.pix.pixelformat = pix_format;
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;

	//Set Format IOCTL System Call
	if (ioctl(_vfd, VIDIOC_S_FMT, &format) < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Format cannot be set" << std :: endl;
#endif
		return -1;
	}
	return 0;
}

int camera_buffer_req()
{
	struct v4l2_requestbuffers buf_request;
	buf_request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf_request.memory = V4L2_MEMORY_MMAP;
	buf_request.count = 1;

	// Request Buffer IOCTL System Call
	if (ioctl(_vfd, VIDIOC_REQBUFS, &buf_request) < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std  :: cerr << "Buffer Request Failed" << std :: endl;
#endif
		return -1;
	}

	struct v4l2_buffer bufferinfo;
	memset(&bufferinfo, 0, sizeof(bufferinfo));

	bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufferinfo.memory = V4L2_MEMORY_MMAP;
	bufferinfo.index = 0;

	// Buffer Query IOCTL System Call
	if (ioctl(_vfd, VIDIOC_QUERYBUF, &bufferinfo) < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Buffer can't be query" << std :: endl;
#endif
		return -1;
	}

	_buffer_ptr = mmap(NULL, bufferinfo.length, PROT_READ | PROT_WRITE, MAP_SHARED, _vfd, bufferinfo.m.offset);
	_buffer_offset = bufferinfo.m.offset;
	_buffer_length = bufferinfo.length;

	if (_buffer_ptr == MAP_FAILED) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Failed due to mmap" << std :: endl;
#endif
		return -1;
	}

	memset(_buffer_ptr, 0, _buffer_length);
	return 0;
}

void camera_get_format(int vfd)
{
	struct v4l2_fmtdesc fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for (fmt.index=0; ; fmt.index++) {
		if (ioctl(vfd, VIDIOC_ENUM_FMT, &fmt) < 0)
			break;
		std :: cout << fmt.description << std :: endl;
	}
	
}

void camera_get_frame_size(int vfd)
{
	struct v4l2_frmsizeenum fmtsize;
	memset(&fmtsize, 0, sizeof(fmtsize));
	fmtsize.index = 0;
	fmtsize.type = V4L2_FRMSIZE_TYPE_STEPWISE;
	
	if (ioctl(vfd, VIDIOC_ENUM_FRAMESIZES, &fmtsize) < 0) {
			std :: cout << "IOCTL Failed" << std :: endl;
			return;
	}
	if (fmtsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
		std :: cout << "Discrete Format" << std :: endl;
	else if (fmtsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS)
		std :: cout << "Continous Frame" << std :: endl;
	else if (fmtsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
		std :: cout << "Stepwise Frame" << std :: endl;
	
}

int camera_turn_on_streaming()
{
	struct v4l2_buffer bufferinfo;
	memset(&bufferinfo, 0, sizeof(bufferinfo));

	bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufferinfo.memory = V4L2_MEMORY_MMAP;
	bufferinfo.index = 0; 

#ifdef DISPLAY_MESSAGE
	std :: cout << "Streaming has started" << std :: endl;
#endif

	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef QUEUE_BEFORE_STREAMING_ON
	if (ioctl(_vfd, VIDIOC_QBUF, &bufferinfo) < 0) {
		std :: cerr << "Buffer queue failed " << std :: endl;
		return -1;
	}

	if (ioctl(_vfd, VIDIOC_STREAMON, &type) < 0) {
		std :: cerr << "Cannot open Streaming " << std :: endl;
		return -1;
	}
#else
	if (ioctl(_vfd, VIDIOC_STREAMON, &type) < 0) {
		std :: cerr << "Cannot open Streaming " << std :: endl;
		return -1;
	}
#endif
}

int camera_turn_off_streaming()
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(_vfd, VIDIOC_STREAMOFF, &type) < 0)
   		return -1;
   	return 0;
}

int camera_get_frame()
{
	struct v4l2_buffer bufferinfo;
	bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufferinfo.memory = V4L2_MEMORY_MMAP;
	bufferinfo.index = 0; 

	int type = bufferinfo.type;
#ifndef QUEUE_BEFORE_STREAMING_ON
	if (ioctl(_vfd, VIDIOC_QBUF, &bufferinfo) < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Buffer queue failed " << std :: endl;
#endif
		return -1;
	}
#endif

	if (ioctl(_vfd, VIDIOC_DQBUF, &bufferinfo) < 0) {
#ifdef DISPLAY_ERROR_MESSAGE
		std :: cerr << "Deque the Buffer " << std :: endl;
#endif
		return -1;
	}
	return 0;	
}

int camera_save_frame(char *file, void *frame, int length)
{
	int fd = open(file, O_WRONLY | O_CREAT, 0660);

	if (fd < 0)
		goto END;

	write(fd, frame, length);
	close(fd);

#ifdef DISPLAY_MESSAGE
   	std :: cout << "Frame is saved" << std :: endl;
#endif
   	return 0;

END:
#ifdef DISPLAY_ERROR_MESSAGE
	std :: cerr << "Frame couldn't be  saved" << std :: endl;
#endif
   	return -1;
}

void camera_off()
{
	if (_vfd < 0)
		return;
	close(_vfd);
}




int main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;
	if (camera_init(argv[1]))
		return -1;
	if (camera_set_format())
		return -1;
	if (camera_buffer_req())
		return -1;
	if (camera_turn_on_streaming())
		return -1;
	if (camera_get_frame())
		return -1;
	if (camera_save_frame("abhi.jpeg", _buffer_ptr, _buffer_length))
		return -1;
	if (camera_turn_off_streaming())
		return -1;
	camera_off();
	return 0;

}
