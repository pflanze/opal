/*
 * lpc10codec.h
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1999-2001 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: lpc10codec.h,v $
 * Revision 1.2001  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.4  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.3  2001/01/25 07:27:14  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.2  2000/06/10 09:04:56  rogerh
 * fix typo in a comment
 *
 * Revision 1.1  2000/06/05 04:45:02  robertj
 * Added LPC-10 2400bps codec
 *
 */

#ifndef __CODEC_LPC10CODEC_H
#define __CODEC_LPC10CODEC_H

#ifdef __GNUC__
#pragma interface
#endif


#include <opal/transcoders.h>
#include <h323/h323caps.h>


struct lpc10_encoder_state;
struct lpc10_decoder_state;


///////////////////////////////////////////////////////////////////////////////

class Opal_LPC10_PCM : public OpalFramedTranscoder {
  public:
    Opal_LPC10_PCM(const OpalTranscoderRegistration & registration);
    ~Opal_LPC10_PCM();
    virtual BOOL ConvertFrame(const BYTE * src, BYTE * dst);
  protected:
    struct lpc10_decoder_state * decoder;
};


///////////////////////////////////////////////////////////////////////////////

class Opal_PCM_LPC10 : public OpalFramedTranscoder {
  public:
    Opal_PCM_LPC10(const OpalTranscoderRegistration & registration);
    ~Opal_PCM_LPC10();
    virtual BOOL ConvertFrame(const BYTE * src, BYTE * dst);
  protected:
    struct lpc10_encoder_state * encoder;
};


///////////////////////////////////////////////////////////////////////////////

/**This class describes the LPC-10 (FS-1015) codec capability.
 */
class H323_LPC10Capability : public H323NonStandardAudioCapability
{
  PCLASSINFO(H323_LPC10Capability, H323NonStandardAudioCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new LPC-10 capability.
     */
    H323_LPC10Capability(
      H323EndPoint & endpoint   // Endpoint to get NonStandardInfo from.
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Create a copy of the object.
      */
    virtual PObject * Clone() const;
  //@}

  /**@name Identification functions */
  //@{
    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const;
  //@}
};


#endif // __CODEC_LPC10CODEC_H


/////////////////////////////////////////////////////////////////////////////

