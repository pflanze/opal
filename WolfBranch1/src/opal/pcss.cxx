/*
 * pcss.cxx
 *
 * PC Sound system support.
 *
 * Open Phone Abstraction Library (OPAL)
 *
 * Copyright (c) 2000 Equivalence Pty. Ltd.
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
 * $Revision$
 * $Author$
 * $Date$
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "pcss.h"
#endif

#include <opal/buildopts.h>

#include <opal/pcss.h>

#if OPAL_PTLIB_AUDIO

#if OPAL_VIDEO
#include <ptlib/videoio.h>
#include <codec/vidcodec.h>
#endif

#include <ptlib/sound.h>
#include <codec/silencedetect.h>
#include <codec/echocancel.h>
#include <opal/call.h>
#include <opal/patch.h>
#include <opal/manager.h>


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

OpalPCSSEndPoint::OpalPCSSEndPoint(OpalManager & mgr, const char * prefix)
  : OpalEndPoint(mgr, prefix, CanTerminateCall),
    soundChannelPlayDevice(PSoundChannel::GetDefaultDevice(PSoundChannel::Player)),
    soundChannelRecordDevice(PSoundChannel::GetDefaultDevice(PSoundChannel::Recorder))
{
#ifdef _WIN32
  // Windows MultMedia stuff seems to need greater depth due to enormous
  // latencies in its operation
  soundChannelBuffers = 6;
#else
  // Should only need double buffering for Unix platforms
  soundChannelBuffers = 2;
#endif

  PTRACE(3, "PCSS\tCreated PC sound system endpoint.\n" << setfill('\n')
         << "Players:\n"   << PSoundChannel::GetDeviceNames(PSoundChannel::Player)
         << "Recorders:\n" << PSoundChannel::GetDeviceNames(PSoundChannel::Recorder));
}


OpalPCSSEndPoint::~OpalPCSSEndPoint()
{
  PTRACE(4, "PCSS\tDeleted PC sound system endpoint.");
}

OpalMediaFormatList OpalPCSSEndPoint::GetMediaFormats() const
{
  OpalMediaFormatList list = OpalEndPoint::GetMediaFormats();
  list += OpalL16_STEREO_48KHZ;
  return list;
}

static PBoolean SetDeviceName(const PString & name,
                          PSoundChannel::Directions dir,
                          PString & result)
{
  if (name[0] == '#') {
    PStringArray devices = PSoundChannel::GetDeviceNames(dir);
    PINDEX id = name.Mid(1).AsUnsigned();
    if (id == 0 || id > devices.GetSize())
      return PFalse;
    result = devices[id-1];
  }
  else {
    PSoundChannel * pChannel = PSoundChannel::CreateChannelByName(name, dir);
    if (pChannel == NULL)
      return PFalse;
    delete pChannel;
    result = name;
  }

  return PTrue;
}


PBoolean OpalPCSSEndPoint::MakeConnection(OpalCall & call,
                                      const PString & remoteParty,
                                      void * userData,
                               unsigned int /*options*/,
                               OpalConnection::StringOptions *)
{
  // First strip of the prefix if present
  PINDEX prefixLength = 0;
  if (remoteParty.Find(GetPrefixName()+":") == 0)
    prefixLength = GetPrefixName().GetLength()+1;

  PString playDevice;
  PString recordDevice;
  PINDEX separator = remoteParty.FindOneOf("\n\t\\", prefixLength);
  if (separator == P_MAX_INDEX)
    playDevice = recordDevice = remoteParty.Mid(prefixLength);
  else {
    playDevice = remoteParty(prefixLength+1, separator-1);
    recordDevice = remoteParty.Mid(separator+1);
  }

  if (!SetDeviceName(playDevice, PSoundChannel::Player, playDevice))
    playDevice = soundChannelPlayDevice;
  if (!SetDeviceName(recordDevice, PSoundChannel::Recorder, recordDevice))
    recordDevice = soundChannelRecordDevice;

  // Make sure sound devices are available.
  PSoundChannel * soundChannel = PSoundChannel::CreateChannelByName(playDevice, PSoundChannel::Player);
  if (soundChannel == NULL) {
    PTRACE(2, "PCSS\tSound player device \"" << playDevice << "\" in use, call " << call << " aborted.");
    call.Clear(OpalConnection::EndedByLocalBusy);
    return false;
  }
  delete soundChannel;

  soundChannel = PSoundChannel::CreateChannelByName(recordDevice, PSoundChannel::Recorder);
  if (soundChannel == NULL) {
    PTRACE(2, "PCSS\tSound recording device \"" << recordDevice << "\" in use, call " << call << " aborted.");
    call.Clear(OpalConnection::EndedByLocalBusy);
    return false;
  }
  delete soundChannel;

  return AddConnection(CreateConnection(call, playDevice, recordDevice, userData));
}


OpalPCSSConnection * OpalPCSSEndPoint::CreateConnection(OpalCall & call,
                                                        const PString & playDevice,
                                                        const PString & recordDevice,
                                                        void * /*userData*/)
{
  return new OpalPCSSConnection(call, *this, playDevice, recordDevice);
}


PSoundChannel * OpalPCSSEndPoint::CreateSoundChannel(const OpalPCSSConnection & connection,
						                                            const OpalMediaFormat & mediaFormat,
                                                                      PBoolean isSource)
{
  PString deviceName;
  PSoundChannel::Directions dir;
  if (isSource) {
    deviceName = connection.GetSoundChannelRecordDevice();
    dir = PSoundChannel::Recorder;
  }
  else {
    deviceName = connection.GetSoundChannelPlayDevice();
    dir = PSoundChannel::Player;
  }

  PSoundChannel * soundChannel = PSoundChannel::CreateChannelByName(deviceName, dir);
  if (soundChannel == NULL) {
    PTRACE(1, "PCSS\tCould not create sound channel \"" << deviceName
           << "\" for " << (isSource ? "record" : "play") << "ing.");
    return NULL;
  }

  unsigned channels = mediaFormat.GetOptionInteger(OpalAudioFormat::ChannelsOption());

  if (soundChannel->Open(deviceName, dir, channels, mediaFormat.GetClockRate(), 16)) {
    PTRACE(3, "PCSS\tOpened " 
               << ((channels == 1) ? "mono" : ((channels == 2) ? "stereo" : "multi-channel")) 
               << " sound channel \"" << deviceName
               << "\" for " << (isSource ? "record" : "play") << "ing.");
    return soundChannel;
  }

  PTRACE(1, "PCSS\tCould not open sound channel \"" << deviceName
         << "\" for " << (isSource ? "record" : "play")
         << "ing: " << soundChannel->GetErrorText());

  delete soundChannel;
  return NULL;
}


PBoolean OpalPCSSEndPoint::AcceptIncomingConnection(const PString & token)
{
  PSafePtr<OpalPCSSConnection> connection = GetPCSSConnectionWithLock(token, PSafeReadOnly);
  if (connection == NULL) {
    PTRACE(2, "PCSS\tCould not find connection using token \"" << token << '"');
    return false;
  }

  connection->AcceptIncoming();
  return true;
}


PBoolean OpalPCSSEndPoint::RejectIncomingConnection(const PString & token)
{
  PSafePtr<OpalPCSSConnection> connection = GetPCSSConnectionWithLock(token, PSafeReadOnly);
  if (connection == NULL)
    return false;

  connection->Release(OpalConnection::EndedByAnswerDenied);
  return true;
}


PBoolean OpalPCSSEndPoint::OnShowUserInput(const OpalPCSSConnection &, const PString &)
{
  return PTrue;
}


void OpalPCSSEndPoint::OnPatchMediaStream(const OpalPCSSConnection & /*connection*/, PBoolean /*isSource*/, OpalMediaPatch & /*patch*/)
{
}


PBoolean OpalPCSSEndPoint::SetSoundChannelPlayDevice(const PString & name)
{
  return SetDeviceName(name, PSoundChannel::Player, soundChannelPlayDevice);
}


PBoolean OpalPCSSEndPoint::SetSoundChannelRecordDevice(const PString & name)
{
  return SetDeviceName(name, PSoundChannel::Recorder, soundChannelRecordDevice);
}


void OpalPCSSEndPoint::SetSoundChannelBufferDepth(unsigned depth)
{
  PAssert(depth > 1, PInvalidParameter);
  soundChannelBuffers = depth;
}


/////////////////////////////////////////////////////////////////////////////

static unsigned LastConnectionTokenID;

OpalPCSSConnection::OpalPCSSConnection(OpalCall & call,
                                       OpalPCSSEndPoint & ep,
                                       const PString & playDevice,
                                       const PString & recordDevice)
  : OpalConnection(call, ep, psprintf("%u", ++LastConnectionTokenID)),
    endpoint(ep),
    soundChannelPlayDevice(playDevice),
    soundChannelRecordDevice(recordDevice),
    soundChannelBuffers(ep.GetSoundChannelBufferDepth())
{
  silenceDetector = new OpalPCM16SilenceDetector(endpoint.GetManager().GetSilenceDetectParams());
  echoCanceler = new OpalEchoCanceler;

  PTRACE(4, "PCSS\tCreated PC sound system connection with token '" << callToken << "'");
}


OpalPCSSConnection::~OpalPCSSConnection()
{
  PTRACE(4, "PCSS\tDeleted PC sound system connection.");
}


PBoolean OpalPCSSConnection::SetUpConnection()
{
  originating = true;

  // Check if we are A-Party in thsi call, so need to do things differently
  if (ownerCall.GetConnection(0) == this) {
    SetPhase(SetUpPhase);
    if (!OnIncomingConnection(0, NULL)) {
      Release(EndedByCallerAbort);
      return PFalse;
    }

    PTRACE(3, "PCSS\tOutgoing call routed to " << ownerCall.GetPartyB() << " for " << *this);
    if (!ownerCall.OnSetUp(*this)) {
      Release(EndedByNoAccept);
      return PFalse;
    }

    return PTrue;
  }

  PTRACE(3, "PCSS\tSetUpConnection(" << remotePartyName << ')');
  SetPhase(AlertingPhase);
  OnAlerting();

  return endpoint.OnShowIncoming(*this);
}


PBoolean OpalPCSSConnection::SetAlerting(const PString & calleeName, PBoolean)
{
  PTRACE(3, "PCSS\tSetAlerting(" << calleeName << ')');
  SetPhase(AlertingPhase);
  remotePartyName = calleeName;
  return endpoint.OnShowOutgoing(*this);
}


OpalMediaStream * OpalPCSSConnection::CreateMediaStream(const OpalMediaFormat & mediaFormat,
                                                        unsigned sessionID,
                                                        PBoolean isSource)
{
  if (mediaFormat.GetMediaType() != OpalMediaType::Audio())
    return OpalConnection::CreateMediaStream(mediaFormat, sessionID, isSource);

  PSoundChannel * soundChannel = CreateSoundChannel(mediaFormat, isSource);
  if (soundChannel == NULL)
    return NULL;

  return new OpalAudioMediaStream(*this, mediaFormat, sessionID, isSource, soundChannelBuffers, soundChannel);
}


void OpalPCSSConnection::OnPatchMediaStream(PBoolean isSource,
					    OpalMediaPatch & patch)
{
  endpoint.OnPatchMediaStream(*this, isSource, patch);

  int clockRate;
  if (patch.GetSource().GetMediaFormat().GetMediaType() == OpalMediaType::Audio()) {
    PTRACE(3, "PCSS\tAdding filters to patch");
    if (isSource)
      patch.AddFilter(silenceDetector->GetReceiveHandler(), OpalPCM16);
    clockRate = patch.GetSource().GetMediaFormat().GetClockRate();
    echoCanceler->SetParameters(endpoint.GetManager().GetEchoCancelParams());
    echoCanceler->SetClockRate(clockRate);
    patch.AddFilter(isSource?echoCanceler->GetReceiveHandler():echoCanceler->GetSendHandler(), OpalPCM16);
  }
}


OpalMediaStreamPtr OpalPCSSConnection::OpenMediaStream(const OpalMediaFormat & mediaFormat, unsigned sessionID, bool isSource)
{
#if OPAL_VIDEO
  if ( isSource &&
       mediaFormat.GetMediaType() == OpalMediaType::Video() &&
      !ownerCall.IsEstablished() &&
      !endpoint.GetManager().CanAutoStartTransmitVideo()) {
    PTRACE(3, "PCSS\tOpenMediaStream auto start disabled, refusing video open");
    return NULL;
  }
#endif

  return OpalConnection::OpenMediaStream(mediaFormat, sessionID, isSource);
}


PBoolean OpalPCSSConnection::SetAudioVolume(PBoolean source, unsigned percentage)
{
  PSafePtr<OpalAudioMediaStream> stream = PSafePtrCast<OpalMediaStream, OpalAudioMediaStream>(GetMediaStream(OpalMediaType::Audio(), source));
  if (stream == NULL)
    return PFalse;

  PSoundChannel * channel = dynamic_cast<PSoundChannel *>(stream->GetChannel());
  if (channel == NULL)
    return PFalse;

  return channel->SetVolume(percentage);
}


unsigned OpalPCSSConnection::GetAudioSignalLevel(PBoolean source)
{
  PSafePtr<OpalAudioMediaStream> stream = PSafePtrCast<OpalMediaStream, OpalAudioMediaStream>(GetMediaStream(OpalMediaType::Audio(), source));
  if (stream == NULL)
    return UINT_MAX;

  return stream->GetAverageSignalLevel();
}


PSoundChannel * OpalPCSSConnection::CreateSoundChannel(const OpalMediaFormat & mediaFormat, PBoolean isSource)
{
  return endpoint.CreateSoundChannel(*this, mediaFormat, isSource);
}


PBoolean OpalPCSSConnection::SendUserInputString(const PString & value)
{
  PTRACE(3, "PCSS\tSendUserInputString(" << value << ')');
  return endpoint.OnShowUserInput(*this, value);
}


void OpalPCSSConnection::AcceptIncoming()
{
  if (LockReadWrite()) {
    OnConnectedInternal();
    UnlockReadWrite();
  }
}

#endif // OPAL_PTLIB_AUDIO


/////////////////////////////////////////////////////////////////////////////