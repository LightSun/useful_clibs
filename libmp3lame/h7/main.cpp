
#include <stdio.h>
#include <string.h>
#include "lame_dec_mp3.h"

static void _read_callback(void* ud, short* buf, int start, int size){
    printf("_read_callback:  start = %d, size = %d\n", start, size);
}

int main(){
    const char* filePath = "D:/study/res/test2.mp3";
    //const char* filePath = "D:/study/res/animal.mp3";
    MediaData md;
    void* opResult;
    if( (opResult = lame_OpenMedia(filePath, &md)) == NULL){
        printf("lame_OpenMedia failed.\n");
        return -1;
    }
    lame_ReadMediaData(opResult, 0, NULL, _read_callback);
    lame_ReleaseMedia(opResult);
    printf("read media done.\n");
    return 0;
}
