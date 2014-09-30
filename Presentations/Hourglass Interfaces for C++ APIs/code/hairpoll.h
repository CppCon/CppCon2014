#pragma once

#include <stdint.h>

#include "visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Error handling
//

typedef struct error* error_t;

HAIRPOLL_EXPORT
const char* error_message(error_t error);
HAIRPOLL_EXPORT
void error_destruct(error_t error);

//
// Hair polls
//

typedef struct hairpoll* hairpoll_t;

HAIRPOLL_EXPORT
hairpoll_t hairpoll_construct(const char* person, error_t* out_error);
HAIRPOLL_EXPORT
void hairpoll_destruct(hairpoll_t poll);

HAIRPOLL_EXPORT
int32_t hairpoll_add_option(hairpoll_t hairpoll, const char* name, const char* image_url, error_t* out_error);

HAIRPOLL_EXPORT
void hairpoll_vote(hairpoll_t hairpoll, int32_t option, error_t* out_error);

typedef void (*hairpoll_result_handler_t)(void* client_data, const char* name, int32_t votes, const char* html);

HAIRPOLL_EXPORT
void hairpoll_tally(const hairpoll_t hairpoll, hairpoll_result_handler_t handler, void* client_data,
                    error_t* out_error);

#ifdef __cplusplus
} // extern "C"
#endif
