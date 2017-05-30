#ifndef _BOARDS_H_
#define _BOARDS_H_

#ifdef __cplusplus
extern "C" {
#endif


#if defined(BOARD_PCA10028)
  #include "pca10028/pca10028.h"
#else
  #error "Board is not defined"
#endif

#ifdef __cplusplus
}
#endif

#endif
