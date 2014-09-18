#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef hairpoll_EXPORTS
    #ifdef __GNUC__
      #define HAIRPOLL_EXPORT __attribute__ ((dllexport))
    #else
      #define HAIRPOLL_EXPORT __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define HAIRPOLL_EXPORT __attribute__ ((dllimport))
    #else
      #define HAIRPOLL_EXPORT __declspec(dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define HAIRPOLL_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define HAIRPOLL_EXPORT
  #endif
#endif
