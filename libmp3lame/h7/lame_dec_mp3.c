
#include <stdlib.h>
#include <string.h>
#include "lame.h"

#include "lame_dec_mp3.h"
//#include <util.h>
#include <stdio.h>

#define _LAME_MP3_CLIPPED_

#ifdef _LAME_MP3_CLIPPED_
#define decodeAudioData(hip_context, mp3_buf, size, bufferData, mp3data)  \
        hip_decode1_headersB2(hip_context, (unsigned char *) mp3_buf, size, \
            bufferData, mp3data);
#else
#define decodeAudioData(hip_context, mp3_buf, size, bufferData, mp3data)  \
            hip_decode1_unclipped3(hip_context, (unsigned char *) mp3_buf, size, \
            bufferData, mp3data);
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef nullptr
#define nullptr 0
#endif

static int _debugMp3Data = 0;
static mp3data_struct *mp3data;
static hip_t hip_context = NULL;
static int enc_delay, enc_padding;
int _channelCount = 0;
//----- buffer ----------
short* bufferData = NULL;
int bufferDataSize = 0;
short* _pcms = NULL;
//----------- debug --------


static int lame_config(char* buf, int size);

static int isId3Header(char*buf){
    return buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3';
}

static int isAIDHeader(char*buf){
    return buf[0] == 'A' && buf[1] == 'I' && buf[2] == 'D' && buf[3] == '\1';
}

static int isMp123SyncWord(char* buf) {
    // function taken from LAME to identify MP3 syncword
    char abl2[] = { 0, 7, 7, 7, 0, 7, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8 };
    if ((buf[0] & 0xFF) != 0xFF) {
        return false;
    }
    if ((buf[1] & 0xE0) != 0xE0) {
        return false;
    }
    if ((buf[1] & 0x18) == 0x08) {
        return false;
    }
    if ((buf[1] & 0x06) == 0x00) {
        // not layer I/II/III
        return false;
    }
    if ((buf[2] & 0xF0) == 0xF0) {
        // bad bitrate
        return false;
    }
    if ((buf[2] & 0x0C) == 0x0C) {
        // bad sample frequency
        return false;
    }
    if ((buf[1] & 0x18) == 0x18 &&
        (buf[1] & 0x06) == 0x04 &&
        (abl2[buf[2] >> 4] & (1 << (buf[3] >> 6))) != 0) {
        return false;
    }
    if ((buf[3] & 0x03) == 2) {
        // reserved emphasis mode (?)
        return false;
    }
    return true;
}
void *lame_OpenMedia(const char *filename, MediaData *out) {
    if (!hip_context) {
        hip_context = hip_decode_init();
        if (hip_context) {
            mp3data = (mp3data_struct *) malloc(sizeof(mp3data_struct));
            memset(mp3data, 0, sizeof(mp3data_struct));
            enc_delay = -1;
            enc_padding = -1;
        } else{
            return NULL;
        }
    }
//========================================
    static const size_t count = 100;
    int id3Length, aidLength;

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("lame_OpenMedia failed (may not exist). for file = '%s'\n", filename);
        return NULL;
    }
    char buf[100]; //count

    if (fread(buf, 1, 4, file) != 4) {
        fclose(file);
        return NULL;
    }
    if (isId3Header(buf)) {
        // ID3 header found, skip past it
        if (fread(buf, 1, 6, file) != 6) {
            printf("read id3 header error.\n");
            fclose(file);
            return NULL;
        }
        buf[2] &= 0x7F;
        buf[3] &= 0x7F;
        buf[4] &= 0x7F;
        buf[5] &= 0x7F;
        id3Length = (((((buf[2] << 7) + buf[3]) << 7) + buf[4]) << 7) + buf[5];
        //skip id3Length
        if ((int)fread(buf, 1, id3Length, file) != id3Length) {
            printf("skip id3Length error 1.\n");
            fclose(file);
            return NULL;
        }
        if (fread(buf, 1, 4, file) != 4) {
            printf("skip id3Length error 2.\n");
            fclose(file);
            return NULL;
        }
    }
    if (isAIDHeader(buf)) {
        //aid header
        if (fread(buf, 1, 2, file) != 2) {
            printf("read aid header error.\n");
            fclose(file);
            return NULL;
        }
        aidLength = buf[0] + 256 * buf[1];
        //skip adiLength
        if ((int)fread(buf, 1, aidLength, file) != aidLength) {
            printf("skip adiLength error 1.\n");
            fclose(file);
            return NULL;
        }
        if (fread(buf, 1, 4, file) != 4) {
            printf("skip adiLength error 2.\n");
            fclose(file);
            return NULL;
        }
    }
    //----------------
    char tmpBuf[1];
    while (!isMp123SyncWord(buf)) {
        // search for MP3 syncword one byte at a time
        for (int i = 0; i < 3; i++) {
            buf[i] = buf[i + 1];
        }
        int val = fread(tmpBuf, 1, 1, file);
        if (val == -1) {
            fclose(file);
            return NULL;
        }
        buf[3] = tmpBuf[0];
    }
    //============= prepare a buffer ============
    bufferData = (short*)malloc(1152*2 * sizeof (short));
    bufferDataSize = 0;
    //===========================================

    size_t size;
    do {
        size = fread(buf, 1, count, file);
        if (lame_config(buf, size) == 0) {
            break;
        }
    } while (size > 0);

    out->sampleRate = mp3data->samplerate;
    out->channelCount = mp3data->stereo;
    _channelCount = mp3data->stereo;

    //create a buffer
    _pcms = malloc(mp3data->framesize * mp3data->stereo * sizeof (short));
    return file;
}

//nativeConfigureDecoder: return 0 for prepared done.
static int lame_config(char *mp3_buf, int size) {
    int samples_read = decodeAudioData(hip_context, mp3_buf, (size_t)size, bufferData, mp3data);
    if(mp3data->samplerate == 0 || mp3data->stereo == 0){
        //not prepared
        if(samples_read > 0){
            printf("lame_config >>> wrong sample read. samples_read = %d\n", samples_read);
        }
        return -1;
    }
    if (mp3data->header_parsed) {
        mp3data->totalframes = mp3data->nsamp / mp3data->framesize;
    }
    printf("lame_config >>> config ok. buffered samples_read = %d\n", samples_read);
    if(samples_read > 0){
        bufferDataSize = samples_read * mp3data->stereo;
        samples_read = 0;
    }
    return samples_read;
}

static void readLeftAudioData(char buf[], size_t readSize, const char * tag, void* ud, DataCallback cb){
    //if readSize == 0, means read buffer data
    int samples_read;
    while (true){
        samples_read = decodeAudioData(hip_context, buf, readSize, _pcms, mp3data);
        //read until no buffer data.
        if (samples_read > 0) {
            (*cb)(ud, _pcms, 0, samples_read * _channelCount);
            //addAudioData(_pcms, 0, samples_read * _channelCount);
            if (_debugMp3Data) {
                printf("readLeftAudioData >>> %s. samples_read = %d\n", tag, samples_read);
            }
        } else{
            break;
        }
    }
}

/**
 * read the media data for target count.
 * @param openResult
 * @param filebuf
 * @param count
 */
void lame_ReadMediaData(void *openResult, int blockSize, void* ud, DataCallback cb) {
    //startPreProcessAudio(blockSize * _channelCount);
    //check buffer exist or not.
    if(bufferDataSize > 0){
        if (_debugMp3Data) {
            printf("lame_ReadMediaData >>> [ buffer sample ]. size = %d\n", bufferDataSize / _channelCount);
        }
        //addAudioData(bufferData, 0, bufferDataSize);
        (*cb)(ud, bufferData, 0, bufferDataSize);
    }
    //recycle
    if(bufferData != nullptr){
        free(bufferData);
        bufferData = nullptr;
        bufferDataSize = 0;
    }
    static size_t bufferSize = 1024;

    FILE *file = (FILE*)(openResult);
    char buf[1024]; //bufferSize
    size_t readSize = 0;
    int samples_read = 0;

    readLeftAudioData(buf, 0, "[ check-bufferData 1 ]", ud, cb);
    //check if data is enough. if enough direct return.
    while ((readSize = fread(buf, 1, bufferSize, file)) > 0) {
        // check for buffered data
        samples_read = decodeAudioData(hip_context, buf, readSize, _pcms, mp3data);
        if (_debugMp3Data) {
            printf("lame_ReadMediaData >>> read sample data. samples_read = %d, bufSize = %d\n",
                samples_read, bufferSize);
        }
        if (samples_read > 0) {
            //addAudioData(_pcms, 0, samples_read * _channelCount);
            (*cb)(ud, _pcms, 0, samples_read * _channelCount);

            readLeftAudioData(buf, 0, "[ check-bufferData 2 ]", ud, cb);
        } else if (samples_read < 0) {
            break;
        }
    }
    printf("lame_ReadMediaData done.\n");
}

void lame_ReleaseMedia(void *openResult) {
    FILE *file = (FILE*)(openResult);
    //close
    fclose(file);
    if(bufferData != nullptr){
        free(bufferData);
        bufferData = nullptr;
        bufferDataSize = 0;
    }
    if(_pcms){
        free(_pcms);
        _pcms = NULL;
    }

    if (hip_context) {
        int ret = hip_decode_exit(hip_context);
        hip_context = NULL;
        free(mp3data);
        mp3data = NULL;
        enc_delay = -1;
        enc_padding = -1;
    }
}
