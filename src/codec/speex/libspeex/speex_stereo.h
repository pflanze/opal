/* Copyright (C) 2002 Jean-Marc Valin*/
/**
   @file speex_stereo.h
   @brief Describes the handling for intensity stereo
*/
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef STEREO_H
#define STEREO_H

#include "speex_bits.h"

/** State used for decoding (intensity) stereo information */
typedef struct SpeexStereoState {
   float balance;      /**< Left/right balance info */
   float e_ratio;      /**< Ratio of energies: E(left+right)/[E(left)+E(right)]  */
   float smooth_left;  /**< Smoothed left channel gain */
   float smooth_right; /**< Smoothed right channel gain */
   float reserved1;    /**< Reserved for future use */
   float reserved2;    /**< Reserved for future use */
} SpeexStereoState;

/** Initialization value for a stereo state */
#define SPEEX_STEREO_STATE_INIT {1,.5,1,1}

/** Transforms a stereo frame into a mono frame and stores intensity stereo info in 'bits' */
void speex_encode_stereo(float *data, int frame_size, SpeexBits *bits);

/** Transforms a mono frame into a stereo frame using intensity stereo info */
void speex_decode_stereo(float *data, int frame_size, SpeexStereoState *stereo);

/** Callback handler for intensity stereo info */
int speex_std_stereo_request_handler(SpeexBits *bits, void *state, void *data);

#endif
