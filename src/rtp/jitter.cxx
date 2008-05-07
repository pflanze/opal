/*
 * jitter.cxx
 *
 * Jitter buffer support
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "jitter.h"
#endif

#include <rtp/jitter.h>

/*Number of consecutive attempts to add a packet to the jitter buffer while
  it is full before the system clears the jitter buffer and starts over
  again. */
#define MAX_BUFFER_OVERRUNS 20

/**How much time must elapse with lower jitter before jitter
   buffer size is reduced */
#define DECREASE_JITTER_PERIOD 5000 // milliseconds

/* Percentage of current jitter buffer size that constitutes a "genuinely" smaller
jitter */
#define LOWER_JITTER_MAX_PCNT   80

/* Minimum number of packets that constitute a reliable sample for setting a lower
jitter buffer target */
#define DECREASE_JITTER_MIN_PACKETS 50



#if PTRACING && !defined(NO_ANALYSER)

class RTP_JitterBufferAnalyser : public PObject
{
    PCLASSINFO(RTP_JitterBufferAnalyser, PObject);
  public:
    RTP_JitterBufferAnalyser();
    void In(DWORD time, unsigned depth, const char * extra);
    void Out(DWORD time, unsigned depth, const char * extra);
    void PrintOn(ostream & strm) const;

    struct Info {
      Info() { }
      DWORD         time;
      PTimeInterval tick;
      int           depth;
      const char *  extra;
    } in[1000], out[1000];
    PINDEX inPos, outPos;
};

#endif

#define new PNEW


/////////////////////////////////////////////////////////////////////////////

OpalJitterBuffer::OpalJitterBuffer(unsigned minJitterDelay,
                                   unsigned maxJitterDelay,
                                   unsigned time,
                                   PINDEX stackSize)
  : jitterThread(NULL), jitterStackSize(stackSize)
{
  // Jitter buffer is a queue of frames waiting for playback, a list of
  // free frames, and a couple of place holders for the frame that is
  // currently beeing read from the RTP transport or written to the codec.

  oldestFrame = newestFrame = currentFrame = NULL;
  // Calculate the maximum amount of timestamp units for the jitter buffer
  minJitterTime = minJitterDelay;
  maxJitterTime = maxJitterDelay;
  currentJitterTime = minJitterDelay;
  targetJitterTime = currentJitterTime;
  timeUnits = time;

  // Calculate number of frames to allocate, we make the assumption that the
  // smallest packet we can possibly get is 5ms long.
  bufferSize = maxJitterTime/(5*timeUnits)+1;

  // Nothing in the buffer so far
  currentDepth = 0;
  packetsTooLate = 0;
  bufferOverruns = 0;
  consecutiveBufferOverruns = 0;
  maxConsecutiveMarkerBits = 10;
  consecutiveMarkerBits = 0;
  consecutiveEarlyPacketStartTime = 0;
  doJitterReductionImmediately = false;
  doneFreeTrash = false;

  lastWriteTimestamp = 0;
  lastWriteTick = 0;
  jitterCalc = 0;
  jitterCalcPacketCount = 0;

  shuttingDown = false;
  preBuffering = true;
  firstReadData = true;

  // Allocate the frames and put them all into the free list
  freeFrames = new Entry;
  freeFrames->next = freeFrames->prev = NULL;

  for (PINDEX i = 0; i < bufferSize; i++) {
    Entry * frame = new Entry;
    frame->prev = NULL;
    frame->next = freeFrames;
    freeFrames->prev = frame;
    freeFrames = frame;
  }

  PTRACE(4, "RTP\tOpal jitter buffer created:" << *this << " obj=" << this);

#if PTRACING && !defined(NO_ANALYSER)
  analyser = new RTP_JitterBufferAnalyser;
#else
  analyser = NULL;
#endif
}


OpalJitterBuffer::~OpalJitterBuffer()
{
  shuttingDown = true;

  if (jitterThread != NULL) {
    PTRACE(3, "RTP\tRemoving jitter buffer " << this << ' ' << jitterThread->GetThreadName());
    PAssert(jitterThread->WaitForTermination(10000), "Jitter buffer thread did not terminate");
    delete jitterThread;
    jitterThread = NULL;
  } else {
    PTRACE(1, "RTP\tJitter buffer thread is null, when the OpalJitterBuffer destructor runs");
  }
  bufferMutex.Wait();

  // Free up all the memory allocated
  while (oldestFrame != NULL) {
    Entry * frame = oldestFrame;
    oldestFrame = oldestFrame->next;
    delete frame;
  }

  while (freeFrames != NULL) {
    Entry * frame = freeFrames;
    freeFrames = freeFrames->next;
    delete frame;
  }

  delete currentFrame;

  bufferMutex.Signal();


#if PTRACING && !defined(NO_ANALYSER)
  PTRACE(5, "RTP\tJitter buffer analysis: size=" << bufferSize
         << " time=" << currentJitterTime << '\n' << *analyser);
  delete analyser;
#endif

  PTRACE(6, "RTP\tFinished destruction of OpalJitterBuffer " << *this);

}

void OpalJitterBuffer::PrintOn(ostream & strm) const
{
    strm << " size=" << bufferSize 
	 << " delay=" << (minJitterTime/timeUnits) << '-' << (maxJitterTime/timeUnits) << " ms  /" 
	 << currentJitterTime << " (" << (currentJitterTime/timeUnits) << "ms)";
}

void OpalJitterBuffer::SetDelay(unsigned minJitterDelay, unsigned maxJitterDelay)
{
  if (shuttingDown)
    PAssert(WaitForTermination(10000), "Jitter buffer thread did not terminate");

  bufferMutex.Wait();

  minJitterTime = minJitterDelay;
  maxJitterTime = maxJitterDelay;
  currentJitterTime = minJitterDelay;
  targetJitterTime = currentJitterTime;

  // Calculate number of frames to allocate, we make the assumption that the
  // smallest packet we can possibly get is 5ms long.
  PINDEX newBufferSize = maxJitterTime/(5*timeUnits)+1;
  while (bufferSize < newBufferSize) {
    Entry * frame = new Entry;
    frame->prev = NULL;
    frame->next = freeFrames;
    if (freeFrames != NULL)
      freeFrames->prev = frame;
    freeFrames = frame;
    bufferSize++;
  }

  if (jitterThread != NULL) {
    if (jitterThread->IsTerminated()) {
      packetsTooLate = 0;
      bufferOverruns = 0;
      consecutiveBufferOverruns = 0;
      consecutiveMarkerBits = 0;
      consecutiveEarlyPacketStartTime = 0;

      // return used frames to free list
      while (oldestFrame != NULL) {
        Entry * frame = oldestFrame;
        oldestFrame = oldestFrame->next;

        frame->prev = NULL;
        frame->next = freeFrames;
        if (freeFrames != NULL)
          freeFrames->prev = frame;
        freeFrames = frame;
      }

      oldestFrame = newestFrame = currentFrame = NULL;

      shuttingDown = false;
      preBuffering = true;

      PTRACE(3, "RTP\tJitter buffer restarted:" << *this);
      jitterThread->Restart();
    }
  }

  bufferMutex.Signal();
}

void OpalJitterBuffer::Resume(PHandleAggregator * /*aggregator */)
{
  // otherwise create a seperate thread as per the old design
  jitterThread = PThread::Create(PCREATE_NOTIFIER(JitterThreadMain), "RTP Jitter");
  jitterThread->Resume();
}

void OpalJitterBuffer::JitterThreadMain(PThread &, INT)
{
  OpalJitterBuffer::Entry * currentReadFrame;
  PBoolean markerWarning;

  PTRACE(4, "RTP\tJitter RTP receive thread started: " << this);

  if (Init(currentReadFrame, markerWarning)) {

    while (!shuttingDown) {
      if (!PreRead(currentReadFrame, markerWarning))
        break;

      if (!OnRead(currentReadFrame, markerWarning, true))
        break;
    }

    DeInit(currentReadFrame, markerWarning);
  }

  PTRACE(4, "RTP\tJitter RTP receive thread finished: " << this);
}

//void OpalJitterBuffer::Main()
PBoolean OpalJitterBuffer::Init(Entry * & /*currentReadFrame*/, PBoolean & markerWarning)
{
  bufferMutex.Wait();
  markerWarning = false;
  return true;
}

void OpalJitterBuffer::DeInit(Entry * & /*currentReadFrame*/, PBoolean & /*markerWarning*/)
{
}

PBoolean OpalJitterBuffer::PreRead(OpalJitterBuffer::Entry * & currentReadFrame, PBoolean & /*markerWarning*/)
{
  // Get the next free frame available for use for reading from the RTP
  // transport. Place it into a parking spot.
  if (freeFrames != NULL) {
    // Take the next free frame and make it the current for reading
    currentReadFrame = freeFrames;
    freeFrames = freeFrames->next;
    if (freeFrames != NULL)
      freeFrames->prev = NULL;
    PTRACE_IF(2, consecutiveBufferOverruns > 1,
                "RTP\tJitter buffer full, threw away "
                << consecutiveBufferOverruns << " oldest frames");
    consecutiveBufferOverruns = 0;
  }
  else {
    // We have a full jitter buffer, need a new frame so take the oldest one
    currentReadFrame = oldestFrame;
    if (oldestFrame != NULL)
	    oldestFrame = oldestFrame->next;
    if (oldestFrame != NULL)
      oldestFrame->prev = NULL;
    currentDepth--;
    bufferOverruns++;
    consecutiveBufferOverruns++;
    if (consecutiveBufferOverruns > MAX_BUFFER_OVERRUNS) {
      PTRACE(2, "RTP\tJitter buffer continuously full, throwing away entire buffer.");
      freeFrames = oldestFrame;
      oldestFrame = newestFrame = NULL;
      preBuffering = true;
    }
    else {
      PTRACE_IF(2, consecutiveBufferOverruns == 1,
                "RTP\tJitter buffer full, throwing away oldest frame ("
                << currentReadFrame->GetTimestamp() << ')');
    }
  }

  if (currentReadFrame != NULL)
    currentReadFrame->next = NULL;

  bufferMutex.Signal();

  return true;
}

PBoolean OpalJitterBuffer::OnRead(OpalJitterBuffer::Entry * & currentReadFrame, PBoolean & markerWarning, PBoolean loop)
{
  do {
    // Keep reading from the RTP transport frames
    if (!OnReadPacket(*currentReadFrame, loop)) {
      if (currentReadFrame != NULL)
        delete currentReadFrame;  // Destructor won't delete this one, so do it here.
      shuttingDown = true; // Flag to stop the reading side thread
      PTRACE(3, "RTP\tJitter RTP receive thread ended");
      return false;
    }
  } while (currentReadFrame->GetSize() == 0);

  currentReadFrame->tick = PTimer::Tick();

  if (consecutiveMarkerBits < maxConsecutiveMarkerBits) {
    if (currentReadFrame != NULL && currentReadFrame->GetMarker()) {
      PTRACE(3, "RTP\tReceived start of talk burst: " << currentReadFrame->GetTimestamp());
      //preBuffering = true;
      consecutiveMarkerBits++;
    }
    else
      consecutiveMarkerBits = 0;
  }
  else {
    if (currentReadFrame != NULL && currentReadFrame->GetMarker())
      currentReadFrame->SetMarker(false);
    if (!markerWarning && consecutiveMarkerBits == maxConsecutiveMarkerBits) {
      markerWarning = true;
      PTRACE(2, "RTP\tEvery packet has Marker bit, ignoring them from this client!");
    }
  }
    
    
#if PTRACING && !defined(NO_ANALYSER)
  analyser->In(currentReadFrame->GetTimestamp(), currentDepth, preBuffering ? "PreBuf" : "");
#endif

  // Queue the frame for playing by the thread at other end of jitter buffer
  bufferMutex.Wait();

  // Have been reading a frame, put it into the queue now, at correct position
  if (newestFrame == NULL || oldestFrame == NULL)
    oldestFrame = newestFrame = currentReadFrame; // Was empty
  else {
    DWORD time = currentReadFrame->GetTimestamp();

    if (time > newestFrame->GetTimestamp() || (time == newestFrame->GetTimestamp() && currentReadFrame->GetSequenceNumber() >= newestFrame->GetSequenceNumber())) {
      // Is newer than newst, put at that end of queue
      currentReadFrame->prev = newestFrame;
      newestFrame->next = currentReadFrame;
      newestFrame = currentReadFrame;
    }
    else if (time < oldestFrame->GetTimestamp() || (time == oldestFrame->GetTimestamp() && currentReadFrame->GetSequenceNumber() < oldestFrame->GetSequenceNumber())) {
      // Is older than the oldest, put at that end of queue
      currentReadFrame->next = oldestFrame;
      oldestFrame->prev = currentReadFrame;
      oldestFrame = currentReadFrame;
    }
    else {
      // Somewhere in between, locate its position
      Entry * frame = newestFrame->prev;
      while (time < frame->GetTimestamp() || (time == frame->GetTimestamp() && currentReadFrame->GetSequenceNumber() < frame->GetSequenceNumber()))
        frame = frame->prev;

      currentReadFrame->prev = frame;
      currentReadFrame->next = frame->next;
      frame->next->prev = currentReadFrame;
      frame->next = currentReadFrame;
    }
  }

  currentDepth++;

  return true;
}


PBoolean OpalJitterBuffer::ReadData(RTP_DataFrame & frame)
{
  // Default response is an empty frame, ie silence
  frame.SetPayloadSize(0);

  PWaitAndSignal mutex(bufferMutex);

  if (shuttingDown)
    return false;

  /*Free the frame just written to codec, putting it back into
    the free list and clearing the parking spot for it.
   */
  if (currentFrame != NULL) {

    // Move frame from current to free list
    currentFrame->next = freeFrames;
    if (freeFrames != NULL)
      freeFrames->prev = currentFrame;
    freeFrames = currentFrame;

    currentFrame = NULL;
  }


  /*Get the next frame to send to the codec. Takes it from the oldest
    position in the queue, if it is time to do so, and parks it in the
    special member so can unlock the mutex while the writer thread has its
    way with the buffer.
   */
  if (oldestFrame == NULL) {
    /*No data to play! We ran the buffer down to empty, restart buffer by
      setting flag that will fill it again before returning any data.
     */
    preBuffering = true;
    currentJitterTime = targetJitterTime;

#if PTRACING && !defined(NO_ANALYSER)
    analyser->Out(0, currentDepth, "Empty");
#endif
    return true;
  }

  DWORD requestedTimestamp = frame.GetTimestamp();
  DWORD oldestTimestamp = oldestFrame->GetTimestamp();
  DWORD newestTimestamp = newestFrame->GetTimestamp();

  /* If there is an opportunity (due to silence in the buffer) to implement a desired 
  reduction in the size of the jitter buffer, effect it */

  if (targetJitterTime < currentJitterTime &&
      (newestTimestamp - oldestTimestamp) < currentJitterTime) {
    currentJitterTime = ( targetJitterTime > (newestTimestamp - oldestTimestamp)) ?
                          targetJitterTime : (newestTimestamp - oldestTimestamp);

    PTRACE(3, "RTP\tJitter buffer size decreased to "
           << currentJitterTime << " (" << (currentJitterTime/timeUnits) << "ms)");
  }

  /* See if time for this packet, if our oldest frame is older than the
     required age, then use it. If it is not time yet, make sure that the
     writer thread isn't falling behind (not enough MIPS). If the time
     between the oldest and the newest entries in the jitter buffer is
     greater than the size specified for the buffer, then return the oldest
     entry regardless, making the writer thread catch up.
  */

  if (preBuffering) {
    // Reset jitter baseline (should be handled by GetMarker() condition, but just in case...)
    lastWriteTimestamp = 0;
    lastWriteTick = 0;

    // If oldest frame has not been in the buffer long enough, don't return anything yet
    if ((PTimer::Tick() - oldestFrame->tick).GetInterval() * timeUnits < currentJitterTime/2) {
#if PTRACING && !defined(NO_ANALYSER)
      analyser->Out(oldestTimestamp, currentDepth, "PreBuf");
#endif
      return true;
    }

    preBuffering = false;
  }


  //Handle short silence bursts in the middle of the buffer
  // - if we think we're getting marker bit information, use that
  PBoolean shortSilence = false;
  if (consecutiveMarkerBits < maxConsecutiveMarkerBits) {
    if (oldestFrame->GetMarker() && (PTimer::Tick() - oldestFrame->tick).GetInterval()* timeUnits < currentJitterTime/2)
      shortSilence = true;
  }
  else if (requestedTimestamp < oldestTimestamp && requestedTimestamp > (newestTimestamp - currentJitterTime))
    shortSilence = true;
  
  if (shortSilence) {
    // It is not yet time for something in the buffer
#if PTRACING && !defined(NO_ANALYSER)
    analyser->Out(oldestTimestamp, currentDepth, "Wait");
#endif
    lastWriteTimestamp = 0;
    lastWriteTick = 0;
    return true;
  }

  // Detach oldest packet from the list, put into parking space
  currentDepth--;
#if PTRACING && !defined(NO_ANALYSER)
  analyser->Out(oldestTimestamp, currentDepth, requestedTimestamp >= oldestTimestamp ? "" : "Late");
#endif
  currentFrame = oldestFrame;
  oldestFrame = currentFrame->next;
  currentFrame->next = NULL;

  // Calculate the jitter contribution of this frame
  // - don't count if start of a talk burst
  if (currentFrame->GetMarker()) {
    lastWriteTimestamp = 0;
    lastWriteTick = 0;
  }

  if (lastWriteTimestamp != 0 && lastWriteTick != 0) {
    int thisJitter = 0;

    if (currentFrame->GetTimestamp() < lastWriteTimestamp) {
      //Not too sure how to handle this situation...
      thisJitter = 0;
    }
    else if (currentFrame->tick < lastWriteTick) {
      //Not too sure how to handle this situation either!
      thisJitter = 0;
    }
    else {  
      thisJitter = (currentFrame->tick - lastWriteTick).GetInterval()*timeUnits +
                   lastWriteTimestamp - currentFrame->GetTimestamp();
    }

    if (thisJitter < 0) thisJitter *=(-1);
    thisJitter *=2; //currentJitterTime needs to be at least TWICE the maximum jitter

    if (thisJitter > (int) currentJitterTime * LOWER_JITTER_MAX_PCNT / 100) {
      targetJitterTime = currentJitterTime;
      PTRACE(5, "RTP\tJitter buffer target realigned to current jitter buffer");
      consecutiveEarlyPacketStartTime = PTimer::Tick();
      jitterCalcPacketCount = 0;
      jitterCalc = 0;
    }
    else {
      if (thisJitter > (int) jitterCalc)
        jitterCalc = thisJitter;
      jitterCalcPacketCount++;

      //If it's bigger than the target we're currently trying to set, adapt that target.
      //Note: this will never make targetJitterTime larger than currentJitterTime due to
      //previous if condition
      DWORD newJitterTime = thisJitter * 100 / LOWER_JITTER_MAX_PCNT;
      if (newJitterTime > maxJitterTime)
        newJitterTime = maxJitterTime;
      if (newJitterTime > targetJitterTime) {
        targetJitterTime = newJitterTime;
        PTRACE(3, "RTP\tJitter buffer target size increased to "
                   << targetJitterTime << " (" << (targetJitterTime/timeUnits) << "ms)");
      }
    }
  }

  lastWriteTimestamp = currentFrame->GetTimestamp();
  lastWriteTick = currentFrame->tick;


  if (oldestFrame == NULL)
    newestFrame = NULL;
  else {
    oldestFrame->prev = NULL;

    // If exceeded current jitter buffer time delay:
    if ((newestTimestamp - currentFrame->GetTimestamp()) > currentJitterTime) {
      PTRACE(4, "RTP\tJitter buffer length exceeded");
      consecutiveEarlyPacketStartTime = PTimer::Tick();
      jitterCalcPacketCount = 0;
      jitterCalc = 0;
      lastWriteTimestamp = 0;
      lastWriteTick = 0;
      
      // If we haven't yet written a frame, we get one free overrun
      if (firstReadData) {
        PTRACE(4, "RTP\tJitter buffer length exceed was prior to first write. Not increasing buffer size");
        while ((newestTimestamp - currentFrame->GetTimestamp()) > currentJitterTime) {
          Entry * wastedFrame = currentFrame;
          currentFrame = oldestFrame;
          oldestFrame = oldestFrame->next;
          currentDepth--;

          currentFrame->next = NULL; //currentFrame should never be able to be NULL
          
          wastedFrame->next = freeFrames;
          if (freeFrames != NULL)
            freeFrames->prev = wastedFrame;
          freeFrames = wastedFrame;

          if (oldestFrame == NULL) {
            newestFrame = NULL;
            break;
          }

          oldestFrame->prev = NULL;
        }
        
        firstReadData = false;
        frame = *currentFrame;
        return true;
      }


      // See if exceeded maximum jitter buffer time delay, waste them if so
      while ((newestTimestamp - currentFrame->GetTimestamp()) > maxJitterTime) {
        PTRACE(4, "RTP\tJitter buffer oldest packet ("
               << currentFrame->GetTimestamp() << " < "
               << (newestTimestamp - maxJitterTime)
               << ") too late, throwing away");

        packetsTooLate++;

        currentJitterTime = maxJitterTime;
      
        //Throw away the oldest frame and move everything up
        Entry * wastedFrame = currentFrame;
        currentFrame = oldestFrame;
        oldestFrame = oldestFrame->next;
        currentDepth--;

        currentFrame->next = NULL; //currentFrame should never be able to be NULL
        
        wastedFrame->next = freeFrames;
        if (freeFrames != NULL)
          freeFrames->prev = wastedFrame;
        freeFrames = wastedFrame;

        if (oldestFrame == NULL) {
          newestFrame = NULL;
          break;
        }
      }

	// Now change the jitter time to cope with the new size
    // unless already set to maxJitterTime
      if (newestTimestamp - currentFrame->GetTimestamp() > currentJitterTime) 
          currentJitterTime = newestTimestamp - currentFrame->GetTimestamp();

      targetJitterTime = currentJitterTime;
      PTRACE(3, "RTP\tJitter buffer size increased to "
             << currentJitterTime << " (" << (currentJitterTime/timeUnits) << "ms)");
    }
  }

  if ((PTimer::Tick() - consecutiveEarlyPacketStartTime).GetInterval() > DECREASE_JITTER_PERIOD &&
       jitterCalcPacketCount >= DECREASE_JITTER_MIN_PACKETS){
    jitterCalc = jitterCalc * 100 / LOWER_JITTER_MAX_PCNT;
    if (jitterCalc < targetJitterTime / 2) jitterCalc = targetJitterTime / 2;
    if (jitterCalc < minJitterTime) jitterCalc = minJitterTime;
    targetJitterTime = jitterCalc;
    PTRACE(3, "RTP\tJitter buffer target size decreased to "
               << targetJitterTime << " (" << (targetJitterTime/timeUnits) << "ms)");
    jitterCalc = 0;
    jitterCalcPacketCount = 0;
    consecutiveEarlyPacketStartTime = PTimer::Tick();
  }

  /* If using immediate jitter reduction (rather than waiting for silence opportunities)
  then trash oldest frames as necessary to reduce the size of the jitter buffer */
  if (targetJitterTime < currentJitterTime &&
      doJitterReductionImmediately &&
      newestFrame != NULL) {
    while ((newestFrame->GetTimestamp() - currentFrame->GetTimestamp()) > targetJitterTime){
      // Throw away the newest entries
      Entry * wastedFrame = newestFrame;
      newestFrame = newestFrame->prev;
      if (newestFrame != NULL)
          newestFrame->next = NULL;
      wastedFrame->prev = NULL;

      // Put thrown away frame on free list
      wastedFrame->next = freeFrames;
      if (freeFrames != NULL)
        freeFrames->prev = wastedFrame;
      freeFrames = wastedFrame;

      // Reset jitter calculation baseline
      lastWriteTimestamp = 0;
      lastWriteTick = 0;

      currentDepth--;
      if (newestFrame == NULL) 
      {
          oldestFrame = NULL;
          break;
      }
    }

    currentJitterTime = targetJitterTime;
    PTRACE(3, "RTP\tJitter buffer size decreased to "
        << currentJitterTime << " (" << (currentJitterTime/timeUnits) << "ms)");

  }

  firstReadData = false;
  frame = *currentFrame;
  return true;
}


/////////////////////////////////////////////////////////////////////////////

RTP_JitterBuffer::RTP_JitterBuffer(RTP_Session & sess,
                                   unsigned minJitterDelay,
                                   unsigned maxJitterDelay,
                                   unsigned time,
                                   PINDEX stackSize)
  : OpalJitterBuffer(minJitterDelay, maxJitterDelay, time, stackSize),
    session(sess)
{
    PTRACE(6, "RTP_JitterBuffer\tConstructor" << *this);
}

RTP_JitterBuffer::~RTP_JitterBuffer() 
{
}

PBoolean RTP_JitterBuffer::OnReadPacket(RTP_DataFrame & frame,
				    PBoolean loop)
{
  PBoolean success = session.ReadData(frame, loop);
  PTRACE(8, "RTP\tOnReadPacket: Frame from network, timestamp " << frame.GetTimestamp());
  return success;
}

void RTP_JitterBuffer::Resume(PHandleAggregator * aggregator)
{
  OpalJitterBuffer::Resume(aggregator);
}


/////////////////////////////////////////////////////////////////////////////


#if PTRACING && !defined(NO_ANALYSER)

RTP_JitterBufferAnalyser::RTP_JitterBufferAnalyser()
{
  inPos = outPos = 1;
  in[0].time = out[0].time = 0;
  in[0].tick = out[0].tick = PTimer::Tick();
  in[0].depth = out[0].depth = 0;
}


void RTP_JitterBufferAnalyser::In(DWORD time, unsigned depth, const char * extra)
{
  if (inPos < PARRAYSIZE(in)) {
    in[inPos].tick = PTimer::Tick();
    in[inPos].time = time;
    in[inPos].depth = depth;
    in[inPos++].extra = extra;
  }
}


void RTP_JitterBufferAnalyser::Out(DWORD time, unsigned depth, const char * extra)
{
  if (outPos < PARRAYSIZE(out)) {
    out[outPos].tick = PTimer::Tick();
    if (time == 0 && outPos > 0)
      out[outPos].time = out[outPos-1].time;
    else
      out[outPos].time = time;
    out[outPos].depth = depth;
    out[outPos++].extra = extra;
  }
}


void RTP_JitterBufferAnalyser::PrintOn(ostream & strm) const
{
  strm << "Input samples: " << inPos << " Output samples: " << outPos << "\n"
          "Dir\tRTPTime\tInDiff\tOutDiff\tInMode\tOutMode\t"
          "InDepth\tOutDep\tInTick\tInDelay\tOutTick\tOutDel\tIODelay\n";
  PINDEX ix = 1;
  PINDEX ox = 1;
  while (ix < inPos || ox < outPos) {
    while (ix < inPos && (ox >= outPos || in[ix].time < out[ox].time)) {
      strm << "In\t"
           << in[ix].time << '\t'
           << (in[ix].time - in[ix-1].time) << "\t"
              "\t"
           << in[ix].extra << "\t"
              "\t"
           << in[ix].depth << "\t"
              "\t"
           << (in[ix].tick - in[0].tick) << '\t'
           << (in[ix].tick - in[ix-1].tick) << "\t"
              "\t"
              "\t"
              "\n";
      ix++;
    }

    while (ox < outPos && (ix >= inPos || out[ox].time < in[ix].time)) {
      strm << "Out\t"
           << out[ox].time << "\t"
              "\t"
           << (out[ox].time - out[ox-1].time) << "\t"
              "\t"
           << out[ox].extra << "\t"
              "\t"
           << out[ox].depth << "\t"
              "\t"
              "\t"
           << (out[ox].tick - out[0].tick) << '\t'
           << (out[ox].tick - out[ox-1].tick) << "\t"
              "\n";
      ox++;
    }

    while (ix < inPos && ox < outPos && in[ix].time == out[ox].time) {
      strm << "I/O\t"
           << in[ix].time << '\t'
           << (in[ix].time - in[ix-1].time) << '\t'
           << (out[ox].time - out[ox-1].time) << '\t'
           << in[ix].extra << '\t'
           << out[ox].extra << '\t'
           << in[ix].depth << '\t'
           << out[ox].depth << '\t'
           << (in[ix].tick - in[0].tick) << '\t'
           << (in[ix].tick - in[ix-1].tick) << '\t'
           << (out[ox].tick - out[0].tick) << '\t'
           << (out[ox].tick - out[ox-1].tick) << '\t'
           << (out[ox].tick - in[ix].tick)
           << '\n';
      ox++;
      ix++;
    }
  }
}


#endif


/////////////////////////////////////////////////////////////////////////////
