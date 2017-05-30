#ifndef _NRF_H_
#define _NRF_H_



#if defined(_WIN32)         
    /* Do not include nrf51 specific files when building for PC host */
#elif defined(__unix)       
    /* Do not include nrf51 specific files when building for PC host */
#elif defined(__APPLE__)    
    /* Do not include nrf51 specific files when building for PC host */
#else

    /* Family selection for family includes. */
    #if defined (NRF51)
        #include "nrf51.h"
        #include "nrf51_bitfields.h"
        #include "nrf51_deprecated.h"
    #else
        #error "Device family is not Supported. See nrf.h."
    #endif /* NRF51, NRF52 */

    #include "compiler_abstraction.h"

#endif /* _WIN32 || __unix || __APPLE__ */

#endif /* NRF_H */

