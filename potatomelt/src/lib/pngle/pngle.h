#ifndef PNGLE_H
#define PNGLE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	PNGLE_STATE_ERROR = -2,
	PNGLE_STATE_EOF = -1,
	PNGLE_STATE_INITIALIZED = 0,
	PNGLE_STATE_DECODING_SIGNATURE,
	PNGLE_STATE_DECODING_IHDR,
	PNGLE_STATE_DECODING_CHUNK,
	PNGLE_STATE_DECODING_IDAT,
	PNGLE_STATE_VERIFYING_CRC,
} pngle_state_t;

typedef struct _pngle_ihdr_t {
	uint32_t width;
	uint32_t height;
	uint8_t depth;
	uint8_t color_type;
	uint8_t compression;
	uint8_t filter;
	uint8_t interlace;
} pngle_ihdr_t;

typedef struct _pngle_t pngle_t; // Incomplete type

// Callback function types
typedef void (*pngle_init_callback_t)(pngle_t *pngle, uint32_t w, uint32_t h);
typedef void (*pngle_draw_callback_t)(pngle_t *pngle, uint32_t x, uint32_t y, uint8_t
                                     rgba[4], uint8_t opacity); // rgba or actual pixel format
typedef void (*pngle_done_callback_t)(pngle_t *pngle);


// Public API
pngle_t *pngle_new(void);
void pngle_destroy(pngle_t *pngle);
void pngle_reset(pngle_t *pngle);
int pngle_feed(pngle_t *pngle, const uint8_t *buf, size_t len);
pngle_state_t pngle_get_state(pngle_t *pngle);
const char *pngle_error_msg(pngle_t *pngle);
int pngle_get_ihdr(pngle_t *pngle, pngle_ihdr_t *ihdr); // New function to get IHDR

// Set callback functions
void pngle_set_init_callback(pngle_t *pngle, pngle_init_callback_t callback);
void pngle_set_draw_callback(pngle_t *pngle, pngle_draw_callback_t callback);
void pngle_set_done_callback(pngle_t *pngle, pngle_done_callback_t callback);

void *pngle_get_user_data(pngle_t *pngle);
void pngle_set_user_data(pngle_t *pngle, void *user_data);


#ifdef __cplusplus
}
#endif

#endif /* PNGLE_H */
