/*
 * silencedetect.cxx
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2001 Post Increment
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
 * The Original Code is Open Phone Abstraction Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: silencedetect.cxx,v $
 * Revision 1.1  2004/05/17 13:24:26  rjongbloed
 * Added silence suppression.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "silencedetect.h"
#endif

#include <codec/silencedetect.h>
#include <opal/patch.h>

#define new PNEW


extern "C" {
  unsigned char linear2ulaw(int pcm_val);
  int ulaw2linear(unsigned char u_val);
};


///////////////////////////////////////////////////////////////////////////////

OpalSilenceDetector::OpalSilenceDetector()
#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif
  : receiveHandler(PCREATE_NOTIFIER(ReceivedPacket))
#ifdef _MSC_VER
#pragma warning(default:4355)
#endif
{
  // Start off in silent mode
  inTalkBurst = FALSE;

  // Initialise the adaptive threshold variables.
  SetMode(AdaptiveSilenceDetection);

  PTRACE(3, "Silence\tHandler created");
}


void OpalSilenceDetector::SetMode(Mode newMode,
                                  unsigned threshold,
                                  unsigned signalDeadband,
                                  unsigned silenceDeadband,
                                  unsigned adaptivePeriod)
{
  mode = newMode;

  // The silence deadband is the period of low energy that have to
  // occur before the system stops sending data over the RTP link.
  signalDeadbandTime = signalDeadband;
  silenceDeadbandTime = silenceDeadband;

  // This is the period over which the adaptive algorithm operates
  adaptiveThresholdTime = adaptivePeriod;

  if (mode != AdaptiveSilenceDetection) {
    levelThreshold = threshold;
    return;
  }

  // Initials threshold levels
  levelThreshold = 0;

  // Initialise the adaptive threshold variables.
  signalMinimum = UINT_MAX;
  silenceMaximum = 0;
  signalReceivedTime = 0;
  silenceReceivedTime = 0;

  // Restart in silent mode
  inTalkBurst = FALSE;
  lastTimestamp = 0;
  receivedTime = 0;
}


OpalSilenceDetector::Mode OpalSilenceDetector::GetMode(BOOL * isInTalkBurst,
                                                       unsigned * currentThreshold) const
{
  if (isInTalkBurst != NULL)
    *isInTalkBurst = inTalkBurst;

  if (currentThreshold != NULL)
    *currentThreshold = ulaw2linear((BYTE)(levelThreshold ^ 0xff));

  return mode;
}


void OpalSilenceDetector::ReceivedPacket(RTP_DataFrame & frame, INT)
{
  // Already silent
  if (frame.GetPayloadSize() == 0)
    return;

  // Can never have silence if NoSilenceDetection
  if (mode == NoSilenceDetection)
    return;

  unsigned thisTimestamp = frame.GetTimestamp();
  if (lastTimestamp == 0) {
    lastTimestamp = thisTimestamp;
    return;
  }

  unsigned timeSinceLastFrame = thisTimestamp - lastTimestamp;
  lastTimestamp = thisTimestamp;

  // Can never have average signal level that high, this indicates that the
  // hardware cannot do silence detection.
  unsigned level = GetAverageSignalLevel(frame.GetPayloadPtr(), frame.GetPayloadSize());
  if (level == UINT_MAX)
    return;

  // Convert to a logarithmic scale - use uLaw which is complemented
  level = linear2ulaw(level) ^ 0xff;

  // Now if signal level above threshold we are "talking"
  BOOL haveSignal = level > levelThreshold;

  // If no change ie still talking or still silent, resent frame counter
  if (inTalkBurst == haveSignal)
    receivedTime = 0;
  else {
    receivedTime += timeSinceLastFrame;
    // If have had enough consecutive frames talking/silent, swap modes.
    if (receivedTime >= (inTalkBurst ? silenceDeadbandTime : signalDeadbandTime)) {
      inTalkBurst = !inTalkBurst;
      PTRACE(4, "Silence\tDetector transition: "
             << (inTalkBurst ? "Talk" : "Silent")
             << " level=" << level << " threshold=" << levelThreshold);

      // If we had talk/silence transition restart adaptive threshold measurements
      signalMinimum = UINT_MAX;
      silenceMaximum = 0;
      signalReceivedTime = 0;
      silenceReceivedTime = 0;
    }
  }

  if (mode == FixedSilenceDetection) {
    if (!inTalkBurst)
      frame.SetPayloadSize(0); // Not in talk burst so silence the frame
    return;
  }

  if (levelThreshold == 0) {
    if (level > 1) {
      // Bootstrap condition, use first frame level as silence level
      levelThreshold = level/2;
      PTRACE(4, "Silence\tThreshold initialised to: " << levelThreshold);
    }
    // inTalkBurst always FALSE here, so return silent
    frame.SetPayloadSize(0);
    return;
  }

  // Count the number of silent and signal frames and calculate min/max
  if (haveSignal) {
    if (level < signalMinimum)
      signalMinimum = level;
    signalReceivedTime++;
  }
  else {
    if (level > silenceMaximum)
      silenceMaximum = level;
    silenceReceivedTime++;
  }

  // See if we have had enough frames to look at proportions of silence/signal
  if ((signalReceivedTime + silenceReceivedTime) > adaptiveThresholdTime) {

    /* Now we have had a period of time to look at some average values we can
       make some adjustments to the threshold. There are four cases:
     */
    if (signalReceivedTime >= adaptiveThresholdTime) {
      /* If every frame was noisy, move threshold up. Don't want to move too
         fast so only go a quarter of the way to minimum signal value over the
         period. This avoids oscillations, and time will continue to make the
         level go up if there really is a lot of background noise.
       */
      int delta = (signalMinimum - levelThreshold)/4;
      if (delta != 0) {
        levelThreshold += delta;
        PTRACE(4, "Silence\tThreshold increased to: " << levelThreshold);
      }
    }
    else if (silenceReceivedTime >= adaptiveThresholdTime) {
      /* If every frame was silent, move threshold down. Again do not want to
         move too quickly, but we do want it to move faster down than up, so
         move to halfway to maximum value of the quiet period. As a rule the
         lower the threshold the better as it would improve response time to
         the start of a talk burst.
       */
      unsigned newThreshold = (levelThreshold + silenceMaximum)/2 + 1;
      if (levelThreshold != newThreshold) {
        levelThreshold = newThreshold;
        PTRACE(4, "Silence\tThreshold decreased to: " << levelThreshold);
      }
    }
    else if (signalReceivedTime > silenceReceivedTime) {
      /* We haven't got a definitive silent or signal period, but if we are
         constantly hovering at the threshold and have more signal than
         silence we should creep up a bit.
       */
      levelThreshold++;
      PTRACE(4, "Silence\tThreshold incremented to: " << levelThreshold
             << " signal=" << signalReceivedTime << ' ' << signalMinimum
             << " silence=" << silenceReceivedTime << ' ' << silenceMaximum);
    }

    signalMinimum = UINT_MAX;
    silenceMaximum = 0;
    signalReceivedTime = 0;
    silenceReceivedTime = 0;
  }

  if (!inTalkBurst)
    frame.SetPayloadSize(0); // Not in talk burst so silence the frame
}


/////////////////////////////////////////////////////////////////////////////

unsigned OpalPCM16SilenceDetector::GetAverageSignalLevel(const BYTE * buffer, PINDEX size)
{
  // Calculate the average signal level of this frame
  int sum = 0;
  PINDEX samples = size/2;
  const short * pcm = (const short *)buffer;
  const short * end = pcm + samples;
  while (pcm != end) {
    if (*pcm < 0)
      sum -= *pcm++;
    else
      sum += *pcm++;
  }

  return sum/samples;
}


/////////////////////////////////////////////////////////////////////////////
