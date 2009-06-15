/*
 * precompile.cxx
 *
 * PWLib application source file for vidtest
 *
 * Precompiled header generation file.
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: precompile.h,v $
 * Revision 1.1  2007/08/20 06:28:15  rjongbloed
 * Added new application to test audio and video codecs by taking real media
 *   (camera/YUV file/microphone/WAV file etc) encoding it, decoding it and playing
 *   back on real output device (screen/speaker etc)
 *
 * Revision 1.1  2003/04/28 08:18:42  craigs
 * Initial version
 *
 * Revision 1.1  2003/02/20 01:10:52  robertj
 * Changed precompiled header so can precompile more than just ptlib.h
 *
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptlib/videoio.h>
#include <ptlib/sound.h>
#include <ptlib/video.h>
#include <ptclib/vsdl.h>
#include <opal/transcoders.h>
#include <codec/opalplugin.h>
#include <codec/opalpluginmgr.h>
#include <codec/vidcodec.h>


// End of File ///////////////////////////////////////////////////////////////