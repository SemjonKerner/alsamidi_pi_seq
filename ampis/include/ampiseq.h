/* author: Semjon Kerner <semjon.kerner@fu-berlin.de> */

#if (DEBUG_ENABLE == 1)
    #define DEBUG(...) printf(__VA_ARGS__);
#else
    #define DEBUG(...)
#endif
