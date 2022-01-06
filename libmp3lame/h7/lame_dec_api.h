#ifndef LAME_DEC_API_H
#define LAME_DEC_API_H

#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct MediaData{
    int sampleRate;
    int channelCount;
} MediaData;

typedef void (*DataCallback)(void* ud, short* data, int start, int size);

//typedef void (*__log)(const char* str,...);
typedef void *(*OpenMedia)(const char *file, MediaData *out);
/**
 * read the media data for target count. every is a float
 * @param openResult
 * @param filebuf
 * @param count
 * @return the count of read
 */
typedef void (*ReadMediaData)(void *openResult, int blockSize, void* ud, DataCallback cb);
typedef void (*ReleaseMedia)(void *openResult);

struct MediaFormat {
    OpenMedia openMedia;
    ReadMediaData readMediaData;
    ReleaseMedia releaseMedia;
    const char* formats[2];
    int formatCount;
};

#if defined(__cplusplus)
}
#endif

#endif // LAME_DEC_API_H
