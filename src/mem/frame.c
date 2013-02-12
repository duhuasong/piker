#include "kernel.h"

#include "mem/frame.h"

/* frames: 131072
 * frames / 32 : 4096
 *
 * frame 0 is 0x0 phys, frame 1 is 0x1000 phys, etc.
 */

uint32_t frames_bitmap[4096] __attribute__((aligned (4096)));

void frame_init() {
    /* All frames are free to begin with */
    for(uint32_t i = 0; i < 4096; ++i) {
        frames_bitmap[i] = 0x00000000;
    }

    /* Now lets mark the kernel's currently used frames as taken */
    for(uint32_t i = _start; i < _end; i += 0x1000) {
        frame_set(i / 4096, 1);
    }
}

uint8_t frame_get(uint32_t frame) {
    /* call non-existant frames occupied */
    if(frame > 131071) {
        return 1;
    }

    uint32_t bits = frames_bitmap[frame / 32];

    uint32_t bit = bits & (1 << (frame % 32));

    if(bit > 0) {
        return 1;
    } else {
        return 0;
    }
}

void frame_set(uint32_t frame, uint8_t status) {
    /* cant set a non-existant frame */
    if(frame > 131071) {
        return;
    }

    if(status > 1) {
        status = 1;
    }

    uint32_t bits = frames_bitmap[frame / 32];

    bits &= ~(1 << (frame & 32));
    bits |= (status << (frame & 32));

    frames_bitmap[frame / 32] = bits;
}

uint8_t frame_alloc(uint32_t* frame) {
    for(uint32_t i = 0; i < 4096; ++i) {
        if(frames_bitmap[i] ^ 0xFFFFFFFF) {
            /* There's a free frame here */

            for(uint32_t f = i * 32; f < (i+1) * 32; ++f) {
                if(0 == frame_get(f)) {
                    frame_set(f, 1);
                    *frame = f;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void frame_free(uint32_t frame) {
    if( frame > 131071) {
        return;
    }

    frame_set(frame, 0);
}

