#ifndef LAME_DEC_MP3_H
#define LAME_DEC_MP3_H

#include "lame_dec_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct lame_context{
    int sampleRate;
    int channelCount;
};

void *lame_OpenMedia(const char *file, MediaData *out);

/**
 * read the media data for target count. every is a float
 * @param openResult
 * @param buffer out buffer
 * @param count
 */
void lame_ReadMediaData(void *openResult, int blockSize, void* ud, DataCallback cb);

void lame_ReleaseMedia(void *openResult);

#if defined(__cplusplus)
}
#endif

#endif // LAME_DEC_MP3_H
