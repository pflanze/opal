/*
 * sdp.cxx
 *
 * Session Description Protocol support.
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
#include <opal/buildopts.h>

#if OPAL_SIP

#ifdef __GNUC__
#pragma implementation "sdp.h"
#endif

#include <sip/sdp.h>

#include <ptlib/socket.h>
#include <opal/transports.h>


#define SIP_DEFAULT_SESSION_NAME    "Opal SIP Session"
#define SDP_MEDIA_TRANSPORT_RTPAVP  "RTP/AVP"

static const char SDPBandwidthPrefix[] = "SDP-Bandwidth-";

#define new PNEW

//
// uncommment this to output a=fmtp lines before a=rtpmap lines. Useful for testing
//
//#define FMTP_BEFORE_RTPMAP 1


/////////////////////////////////////////////////////////
//
//  the following functions bind the media type factory to the SDP format types
//

OpalMediaType OpalMediaType::GetMediaTypeFromSDP(const std::string & sdp, const std::string & transport)
{
  OpalMediaTypeFactory::KeyList_T mediaTypes = OpalMediaTypeFactory::GetKeyList();
  OpalMediaTypeFactory::KeyList_T::iterator r;

  for (r = mediaTypes.begin(); r != mediaTypes.end(); ++r) {
    if (OpalMediaType::GetDefinition(*r)->GetSDPType() == sdp)
      return OpalMediaType(*r);
  }

  std::string s = sdp +"|" + transport;

  for (r = mediaTypes.begin(); r != mediaTypes.end(); ++r) {
    if (OpalMediaType::GetDefinition(*r)->GetSDPType() == s)
      return OpalMediaType(*r);
  }

  return OpalMediaType();
}

SDPMediaDescription * OpalAudioMediaType::CreateSDPMediaDescription(const OpalTransportAddress & localAddress)
{
  return new SDPAudioMediaDescription(localAddress);
}

#if OPAL_VIDEO
SDPMediaDescription * OpalVideoMediaType::CreateSDPMediaDescription(const OpalTransportAddress & localAddress)
{
  return new SDPVideoMediaDescription(localAddress);
}
#endif


/////////////////////////////////////////////////////////

static OpalTransportAddress ParseConnectAddress(const PStringArray & tokens, PINDEX offset, WORD port = 0)
{
  if (tokens.GetSize() == offset+3) {
    if (tokens[offset] *= "IN") {
      if (
        (tokens[offset+1] *= "IP4")
#if OPAL_PTLIB_IPV6
        || (tokens[offset+1] *= "IP6")
#endif
        ) {
        if (tokens[offset+2] == "255.255.255.255") {
          PTRACE(2, "SDP\tInvalid connection address 255.255.255.255 used, treating like HOLD request.");
        }
        else if (tokens[offset+2] == "0.0.0.0") {
          PTRACE(3, "SDP\tConnection address of 0.0.0.0 specified for HOLD request.");
        }
        else
          return OpalTransportAddress(tokens[offset+2], port, "udp");
      }
      else
      {
        PTRACE(1, "SDP\tConnect address has invalid address type \"" << tokens[offset+1] << '"');
      }
    }
    else {
      PTRACE(1, "SDP\tConnect address has invalid network \"" << tokens[offset] << '"');
    }
  }
  else {
    PTRACE(1, "SDP\tConnect address has invalid (" << tokens.GetSize() << ") elements");
  }

  return OpalTransportAddress();
}


static OpalTransportAddress ParseConnectAddress(const PString & str, WORD port = 0)
{
  PStringArray tokens = str.Tokenise(' ');
  return ParseConnectAddress(tokens, 0, port);
}


static PString GetConnectAddressString(const OpalTransportAddress & address)
{
  PStringStream str;

  PIPSocket::Address ip;
  if (!address.IsEmpty() && address.GetIpAddress(ip) && ip.IsValid())
    str << "IN IP" << ip.GetVersion() << ' ' << ip.AsString(PTrue);
  else
    str << "IN IP4 0.0.0.0";

  return str;
}


/////////////////////////////////////////////////////////

SDPMediaFormat::SDPMediaFormat(SDPMediaDescription & parent, RTP_DataFrame::PayloadTypes pt, const char * _name)
  : m_parent(parent) 
  , payloadType(pt)
  , clockRate(0)
  , encodingName(_name)
{
}


SDPMediaFormat::SDPMediaFormat(SDPMediaDescription & parent, const OpalMediaFormat & fmt)
  : mediaFormat(fmt)
  , m_parent(parent) 
  , payloadType(fmt.GetPayloadType())
  , clockRate(fmt.GetClockRate())
  , encodingName(fmt.GetEncodingName())
{
  if (fmt.GetMediaType() == OpalMediaType::Audio()) 
    parameters = PString(PString::Unsigned, fmt.GetOptionInteger(OpalAudioFormat::ChannelsOption()));
}

void SDPMediaFormat::SetFMTP(const PString & str)
{
  if (str.IsEmpty())
    return;

  fmtp = str;
  if (GetMediaFormat().IsEmpty()) // Use GetMediaFormat() to force creation of member
    return;

  mediaFormat.AddOption(new OpalMediaOptionString("RawFMTP", false, str), PTrue); // Save the 'fmtp=' line so it is available at the application level.

  // See if standard format OPT=VAL;OPT=VAL
  if (str.FindOneOf(";=") == P_MAX_INDEX) {
    // Nope, just save the whole string as is
    mediaFormat.SetOptionString("FMTP", str);
    return;
  }

  // guess at the seperator
  char sep = ';';
  if (str.Find(sep) == P_MAX_INDEX)
    sep = ' ';

  // Parse the string for option names and values OPT=VAL;OPT=VAL
  PINDEX sep1prev = 0;
  do {
    // find the next separator (' ' or ';')
    PINDEX sep1next = str.Find(sep, sep1prev);
    if (sep1next == P_MAX_INDEX)
      sep1next--; // Implicit assumption string is not a couple of gigabytes long ...

    // find the next '='. If past the next separator, then ignore it
    PINDEX sep2pos = str.Find('=', sep1prev);
    if (sep2pos > sep1next)
      sep2pos = sep1next;

    PCaselessString key = str(sep1prev, sep2pos-1).Trim();
    if (key.IsEmpty()) {
      PTRACE(2, "SDP\tBadly formed FMTP parameter \"" << str << '"');
      break;
    }

    OpalMediaOption * option = mediaFormat.FindOption(key);
    if (option == NULL || key != option->GetFMTPName()) {
      for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
        if (key == mediaFormat.GetOption(i).GetFMTPName()) {
          option = const_cast<OpalMediaOption *>(&mediaFormat.GetOption(i));
          break;
        }
      }
    }
    if (option != NULL) {
      PString value = str(sep2pos+1, sep1next-1);

      if (dynamic_cast< OpalMediaOptionOctets * >(option) != NULL) {
        if (str.GetLength() % 2 != 0)
          value = value.Trim();
      }
      else {
        // for non-octet string parameters, check for mixed separators
        PINDEX brokenSep = str.Find(' ', sep2pos);
        if (brokenSep < sep1next) {
          sep1next = brokenSep;
          value = str(sep2pos+1, sep1next-1);
        }
        value = value.Trim();
        if (value.IsEmpty())
          value = "1"; // Assume it is a boolean
      }

      if (dynamic_cast< OpalMediaOptionString * >(option) != NULL) {
        PString previous = option->AsString();
        if (!previous.IsEmpty())
          value = previous + ';' + value;
      }

      if (!option->FromString(value)) {
        PTRACE(2, "SDP\tCould not set FMTP parameter \"" << key << "\" to value \"" << value << '"');
      }
    }

    sep1prev = sep1next+1;
  } while (sep1prev != P_MAX_INDEX);
}


PString SDPMediaFormat::GetFMTP() const
{
  if (GetMediaFormat().IsEmpty()) // Use GetMediaFormat() to force creation of member
    return fmtp;

  PString str = mediaFormat.GetOptionString("FMTP");
  if (!str.IsEmpty())
    return str;

  for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
    const OpalMediaOption & option = mediaFormat.GetOption(i);
    const PString & name = option.GetFMTPName();
    if (!name.IsEmpty()) {
      PString value = option.AsString();
      if (value.IsEmpty() && value != option.GetFMTPDefault())
        str += name;
      else {
        PStringArray values = value.Tokenise(';', false);
        for (PINDEX v = 0; v < values.GetSize(); ++v) {
          value = values[v];
          if (value != option.GetFMTPDefault()) {
            if (!str.IsEmpty())
              str += ';';
            str += name + '=' + value;
          }
        }
      }
    }
  }

  return !str ? str : fmtp;
}


void SDPMediaFormat::PrintOn(ostream & strm) const
{
  PAssert(!encodingName.IsEmpty(), "SDPMediaFormat encoding name is empty");

  PINDEX i;
  for (i = 0; i < 2; ++i) {
    switch (i) {
#ifdef FMTP_BEFORE_RTPMAP
      case 1:
#else
      case 0:
#endif
        strm << "a=rtpmap:" << (int)payloadType << ' ' << encodingName << '/' << clockRate;
        if (!parameters.IsEmpty())
          strm << '/' << parameters;
        strm << "\r\n";
        break;
#ifdef FMTP_BEFORE_RTPMAP
      case 0:
#else
      case 1:
#endif
        {
          PString fmtpString = GetFMTP();
          if (!fmtpString.IsEmpty())
            strm << "a=fmtp:" << (int)payloadType << ' ' << fmtpString << "\r\n";
        }
    }
  }
}


const OpalMediaFormat & SDPMediaFormat::GetMediaFormat() const
{
  if (mediaFormat.IsEmpty())
    const_cast<SDPMediaFormat *>(this)->InitialiseMediaFormat();
  return mediaFormat;
}


OpalMediaFormat & SDPMediaFormat::GetWritableMediaFormat()
{
  if (mediaFormat.IsEmpty())
    InitialiseMediaFormat();
  return mediaFormat;
}


void SDPMediaFormat::InitialiseMediaFormat()
{
  mediaFormat = OpalMediaFormat(payloadType, clockRate, encodingName, "sip");
  if (mediaFormat.IsEmpty())
    mediaFormat = OpalMediaFormat(encodingName);
  if (mediaFormat.IsEmpty()) {
    PTRACE(2, "SDP\tCould not find media format for \""
           << encodingName << "\", pt=" << payloadType << ", clock=" << clockRate);
    return;
  }

  mediaFormat.MakeUnique();
  mediaFormat.SetPayloadType(payloadType);

  if (!parameters.IsEmpty() && (mediaFormat.GetMediaType() == OpalMediaType::Audio())) 
    mediaFormat.SetOptionInteger(OpalAudioFormat::ChannelsOption(), parameters.AsUnsigned());
  else
    mediaFormat.SetOptionInteger(OpalAudioFormat::ChannelsOption(), 1);

  // Fill in the default values for (possibly) missing FMTP options
  for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
    OpalMediaOption & option = const_cast<OpalMediaOption &>(mediaFormat.GetOption(i));
    if (!option.GetFMTPName().IsEmpty() && !option.GetFMTPDefault().IsEmpty())
      option.FromString(option.GetFMTPDefault());
  }

  for (SDPBandwidth::const_iterator r = m_parent.GetBandwidth().begin(); r != m_parent.GetBandwidth().end(); ++r) {
    if (r->second > 0)
      mediaFormat.AddOption(new OpalMediaOptionString(SDPBandwidthPrefix + r->first, false, r->second), true);
  }
}


bool SDPMediaFormat::PreEncode()
{
  mediaFormat.SetOptionString(OpalMediaFormat::ProtocolOption(), "SIP");
  return mediaFormat.ToCustomisedOptions();
}


bool SDPMediaFormat::PostDecode(unsigned bandwidth)
{
  if (GetMediaFormat().IsEmpty()) // Use GetMediaFormat() to force creation of member
    return false;

  // try to init encodingName from global list, to avoid PAssert when media has no rtpmap
  if (encodingName.IsEmpty())
    encodingName = mediaFormat.GetEncodingName();

  if (bandwidth > 0) {
    PTRACE(4, "SDP\tAdjusting format \"" << mediaFormat << "\" bandwidth to " << bandwidth);
    mediaFormat.SetOptionInteger(OpalMediaFormat::MaxBitRateOption(), bandwidth);
  }

  mediaFormat.SetOptionString(OpalMediaFormat::ProtocolOption(), "SIP");
  if (mediaFormat.ToNormalisedOptions())
    return true;

  PTRACE(2, "SDP\tCould not normalise format \"" << encodingName << "\", pt=" << payloadType << ", removing.");
  return false;
}


void SDPMediaFormat::SetPacketTime(const PString & optionName, unsigned ptime)
{
  if (mediaFormat.HasOption(optionName)) {
    unsigned newCount = (ptime*mediaFormat.GetTimeUnits())/mediaFormat.GetFrameTime();
    if (newCount < 1)
      newCount = 1;
    mediaFormat.SetOptionInteger(optionName, newCount);
    PTRACE(4, "SDP\tMedia format \"" << mediaFormat << "\" option \"" << optionName
           << "\" set to " << newCount << " packets from " << ptime << " milliseconds");
  }
}


//////////////////////////////////////////////////////////////////////////////

unsigned & SDPBandwidth::operator[](const PString & type)
{
  return std::map<PString, unsigned>::operator[](type);
}


unsigned SDPBandwidth::operator[](const PString & type) const
{
  const_iterator it = find(type);
  return it != end() ? it->second : UINT_MAX;
}


ostream & operator<<(ostream & out, const SDPBandwidth & bw)
{
  for (SDPBandwidth::const_iterator iter = bw.begin(); iter != bw.end(); ++iter)
    out << "b=" << iter->first << ':' << iter->second << "\r\n";
  return out;
}


bool SDPBandwidth::Parse(const PString & param)
{
  PINDEX pos = param.FindSpan("!#$%&'*+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz{|}~"); // Legal chars from RFC
  if (pos == P_MAX_INDEX || param[pos] != ':') {
    PTRACE(2, "SDP\tMalformed bandwidth attribute " << param);
    return false;
  }

  (*this)[param.Left(pos)] = param.Mid(pos+1).AsUnsigned();
  return true;
}


void SDPBandwidth::SetMin(const PString & type, unsigned value)
{
  iterator it = find(type);
  if (it == end())
    (*this)[type] = value;
  else if (it->second > value)
    it->second = value;
}


//////////////////////////////////////////////////////////////////////////////

SDPMediaDescription::SDPMediaDescription(const OpalTransportAddress & address)
  : transportAddress(address)
{
  direction = Undefined;
  port      = 0;
  portCount = 0;
}

PBoolean SDPMediaDescription::SetTransportAddress(const OpalTransportAddress &t)
{
  PIPSocket::Address ip;
  WORD port = 0;
  if (transportAddress.GetIpAndPort(ip, port)) {
    transportAddress = OpalTransportAddress(t, port);
    return PTrue;
  }
  return PFalse;
}

bool SDPMediaDescription::Decode(const PStringArray & tokens)
{
  if (tokens.GetSize() < 3) {
    PTRACE(1, "SDP\tUnknown SDP media type " << tokens[0]);
    return false;
  }

  // parse the media type
  mediaType = OpalMediaType::GetMediaTypeFromSDP(tokens[0], tokens[2]);
  if (mediaType.empty()) {
    PTRACE(1, "SDP\tUnknown SDP media type " << tokens[0]);
    return false;
  }
  OpalMediaTypeDefinition * defn = mediaType.GetDefinition();
  if (defn == NULL) {
    PTRACE(1, "SDP\tNo definition for SDP media type " << tokens[0]);
    return false;
  }

  // parse the port and port count
  PString portStr  = tokens[1];
  PINDEX pos = portStr.Find('/');
  if (pos == P_MAX_INDEX) 
    portCount = 1;
  else {
    PTRACE(3, "SDP\tMedia header contains port count - " << portStr);
    portCount = (WORD)portStr.Mid(pos+1).AsUnsigned();
    portStr   = portStr.Left(pos);
  }
  port = (WORD)portStr.AsUnsigned();

  // parse the transport
  PString transport = tokens[2];
  if (transport != GetSDPTransportType()) {
    PTRACE(2, "SDP\tMedia session transport " << transport << " not compatible with " << GetSDPTransportType());
    return false;
  }

  // check everything
  switch (port) {
    case 0 :
      PTRACE(3, "SDP\tIgnoring media session " << mediaType << " with port=0");
      direction = Inactive;
      break;

    case 65535 :
      PTRACE(2, "SDP\tIllegal port=65535 in media session " << mediaType << ", trying to continue.");
      port = 65534;
      // Do next case

    default :
      PTRACE(4, "SDP\tMedia session port=" << port);

      PIPSocket::Address ip;
      if (transportAddress.GetIpAddress(ip))
        transportAddress = OpalTransportAddress(ip, (WORD)port);
  }

  CreateSDPMediaFormats(tokens);

  return PTrue;
}

void SDPMediaDescription::CreateSDPMediaFormats(const PStringArray & tokens)
{
  // create the format list
  for (PINDEX i = 3; i < tokens.GetSize(); i++) {
    SDPMediaFormat * fmt = CreateSDPMediaFormat(tokens[i]);
    if (fmt != NULL) 
      formats.Append(fmt);
    else {
      PTRACE(2, "SDP\tCannot create SDP media format for port " << tokens[i]);
    }
  }
}


bool SDPMediaDescription::Decode(char key, const PString & value)
{
  PINDEX pos;

  switch (key) {
    case 'i' : // media title
    case 'k' : // encryption key
      break;

    case 'b' : // bandwidth information
      bandwidth.Parse(value);
      break;
      
    case 'c' : // connection information - optional if included at session-level
      SetTransportAddress(ParseConnectAddress(value, port));
      break;

    case 'a' : // zero or more media attribute lines
      pos = value.FindSpan("!#$%&'*+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz{|}~"); // Legal chars from RFC
      if (pos == P_MAX_INDEX)
        SetAttribute(value, "1");
      else if (value[pos] == ':')
        SetAttribute(value.Left(pos), value.Mid(pos+1));
      else {
        PTRACE(2, "SDP\tMalformed media attribute " << value);
      }
      break;

    default:
      PTRACE(1, "SDP\tUnknown media information key " << key);
  }

  return true;
}


bool SDPMediaDescription::PostDecode()
{
  unsigned bw = bandwidth[SDPSessionDescription::TransportIndependentBandwidthType()];
  if (bw == 0)
    bw = bandwidth[SDPSessionDescription::ApplicationSpecificBandwidthType()]*1000;

  SDPMediaFormatList::iterator format = formats.begin();
  while (format != formats.end()) {
    if (format->PostDecode(bw))
      ++format;
    else
      formats.erase(format++);
  }

  return true;
}


void SDPMediaDescription::SetAttribute(const PString & attr, const PString & value)
{
  // get the attribute type
  if (attr *= "sendonly") {
    direction = SendOnly;
    return;
  }

  if (attr *= "recvonly") {
    direction = RecvOnly;
    return;
  }

  if (attr *= "sendrecv") {
    direction = SendRecv;
    return;
  }

  if (attr *= "inactive") {
    direction = Inactive;
    return;
  }

  // handle fmtp attributes
  if (attr *= "fmtp") {
    PString params = value;
    SDPMediaFormat * format = FindFormat(params);
    if (format != NULL)
      format->SetFMTP(params);
    return;
  }

  // unknown attributes
  PTRACE(2, "SDP\tUnknown media attribute " << attr);
  return;
}


SDPMediaFormat * SDPMediaDescription::FindFormat(PString & params) const
{
  SDPMediaFormatList::const_iterator format;

  // extract the RTP payload type
  PINDEX pos = params.FindSpan("0123456789");
  if (pos == P_MAX_INDEX || isspace(params[pos])) {

    // find the format that matches the payload type
    RTP_DataFrame::PayloadTypes pt = (RTP_DataFrame::PayloadTypes)params.Left(pos).AsUnsigned();
    for (format = formats.begin(); format != formats.end(); ++format) {
      if (format->GetPayloadType() == pt)
        break;
    }
  }
  else {
    // Check for it a format name
    pos = params.Find(' ');
    PString encodingName = params.Left(pos);
    for (format = formats.begin(); format != formats.end(); ++format) {
      if (format->GetEncodingName() == encodingName)
        break;
    }
  }

  if (format == formats.end()) {
    PTRACE(2, "SDP\tMedia attribute found for unknown RTP type/name " << params.Left(pos));
    return NULL;
  }

  // extract the attribute argument
  if (pos != P_MAX_INDEX) {
    while (isspace(params[pos]))
      pos++;
    params.Delete(0, pos);
  }

  return const_cast<SDPMediaFormat *>(&*format);
}


void SDPMediaDescription::SetPacketTime(const PString & optionName, const PString & value)
{
  unsigned newTime = value.AsUnsigned();
  if (newTime < 10) {
    PTRACE(2, "SDP\tMalformed (max)ptime attribute value " << value);
    return;
  }

  for (SDPMediaFormatList::iterator format = formats.begin(); format != formats.end(); ++format)
   format->SetPacketTime(optionName, newTime);
}


bool SDPMediaDescription::PreEncode()
{
  for (SDPMediaFormatList::iterator format = formats.begin(); format != formats.end(); ++format) {
    if (!format->PreEncode())
      return false;
  }
  return true;
}


void SDPMediaDescription::Encode(const OpalTransportAddress & commonAddr, ostream & strm) const
{
  PString connectString;
  PIPSocket::Address commonIP, transportIP;
  if (transportAddress.GetIpAddress(transportIP) && commonAddr.GetIpAddress(commonIP) && commonIP != transportIP)
    connectString = GetConnectAddressString(transportAddress);
  if (connectString.IsEmpty())
    connectString = GetConnectAddressString(transportAddress);
  PrintOn(strm, connectString);
}


bool SDPMediaDescription::PrintOn(ostream & str, const PString & connectString) const
{
  //
  // if no media formats, then do not output the media header
  // this avoids displaying an empty media header with no payload types
  // when (for example) video has been disabled
  //
  if (formats.GetSize() == 0)
    return false;

  PIPSocket::Address ip;
  WORD port = 0;
  transportAddress.GetIpAndPort(ip, port);

  /* output media header, note the order is important according to RFC!
     Must be icbka */
  str << "m=" 
      << GetSDPMediaType() << ' '
      << port << ' '
      << GetSDPTransportType()
      << GetSDPPortList() << "\r\n";

  if (!connectString.IsEmpty())
    str << "c=" << connectString << "\r\n";

  // If we have a port of zero, then shutting down SDP stream. No need for anything more
  if (port == 0)
    return false;

  str << bandwidth;

  // media format direction
  switch (direction) {
    case SDPMediaDescription::RecvOnly:
      str << "a=recvonly" << "\r\n";
      break;
    case SDPMediaDescription::SendOnly:
      str << "a=sendonly" << "\r\n";
      break;
    case SDPMediaDescription::SendRecv:
      str << "a=sendrecv" << "\r\n";
      break;
    case SDPMediaDescription::Inactive:
      str << "a=inactive" << "\r\n";
      break;
    default:
      break;
  }

  return true;
}

OpalMediaFormatList SDPMediaDescription::GetMediaFormats() const
{
  OpalMediaFormatList list;

  for (SDPMediaFormatList::const_iterator format = formats.begin(); format != formats.end(); ++format) {
    OpalMediaFormat opalFormat = format->GetMediaFormat();
    if (opalFormat.IsEmpty())
      PTRACE(2, "SIP\tRTP payload type " << format->GetPayloadType() 
             << ", name=" << format->GetEncodingName() << ", not matched to supported codecs");
    else {
      if (opalFormat.GetMediaType() == mediaType && 
          opalFormat.IsValidForProtocol("sip") &&
          opalFormat.GetEncodingName() != NULL) {
        PTRACE(3, "SIP\tRTP payload type " << format->GetPayloadType() << " matched to codec " << opalFormat);
        list += opalFormat;
      }
    }
  }

  return list;
}


void SDPMediaDescription::AddSDPMediaFormat(SDPMediaFormat * sdpMediaFormat)
{
  formats.Append(sdpMediaFormat);
}


void SDPMediaDescription::RemoveSDPMediaFormat(const SDPMediaFormat & sdpMediaFormat)
{
  OpalMediaFormat mediaFormat = sdpMediaFormat.GetMediaFormat();
  const char * encodingName = mediaFormat.GetEncodingName();
  unsigned clockRate = mediaFormat.GetClockRate();

  if (!mediaFormat.IsValidForProtocol("sip") || encodingName == NULL || *encodingName == '\0')
    return;

  for (SDPMediaFormatList::iterator format = formats.begin(); format != formats.end(); ) {
    if ((format->GetEncodingName() *= encodingName) && (format->GetClockRate() == clockRate)) {
      PTRACE(3, "SDP\tRemoving format=" << encodingName << ", " << format->GetPayloadType());
      formats.erase(format++);
    }
    else
      ++format;
  }
}


void SDPMediaDescription::AddMediaFormat(const OpalMediaFormat & mediaFormat)
{
  if (!mediaFormat.IsTransportable() || !mediaFormat.IsValidForProtocol("sip")) {
    PTRACE(4, "SDP\tSDP not including " << mediaFormat << " as it is not a SIP transportable format");
    return;
  }

  RTP_DataFrame::PayloadTypes payloadType = mediaFormat.GetPayloadType();
  unsigned clockRate = mediaFormat.GetClockRate();

  for (SDPMediaFormatList::iterator format = formats.begin(); format != formats.end(); ++format) {
    if (format->GetPayloadType() == payloadType ||
        ((format->GetEncodingName() *= mediaFormat.GetEncodingName()) && format->GetClockRate() == clockRate)
        ) {
      PTRACE(4, "SDP\tSDP not including " << mediaFormat << " as it is already included");
      return;
    }
  }

  SDPMediaFormat * sdpFormat = new SDPMediaFormat(*this, mediaFormat);

  ProcessMediaOptions(*sdpFormat, mediaFormat);

  AddSDPMediaFormat(sdpFormat);
}

void SDPMediaDescription::ProcessMediaOptions(SDPMediaFormat & /*sdpFormat*/, const OpalMediaFormat & /*mediaFormat*/)
{
}


void SDPMediaDescription::AddMediaFormats(const OpalMediaFormatList & mediaFormats, const OpalMediaType & mediaType)
{
  for (OpalMediaFormatList::const_iterator format = mediaFormats.begin(); format != mediaFormats.end(); ++format) {
    if (format->GetMediaType() == mediaType && (format->IsTransportable()))
      AddMediaFormat(*format);
  }
}

//////////////////////////////////////////////////////////////////////////////

SDPRTPAVPMediaDescription::SDPRTPAVPMediaDescription(const OpalTransportAddress & address)
  : SDPMediaDescription(address)
{
}


PCaselessString SDPRTPAVPMediaDescription::GetSDPTransportType() const
{ 
  return SDP_MEDIA_TRANSPORT_RTPAVP; 
}


SDPMediaFormat * SDPRTPAVPMediaDescription::CreateSDPMediaFormat(const PString & portString)
{
  return new SDPMediaFormat(*this, (RTP_DataFrame::PayloadTypes)portString.AsUnsigned());
}


PString SDPRTPAVPMediaDescription::GetSDPPortList() const
{
  PStringStream str;

  SDPMediaFormatList::const_iterator format;

  // output RTP payload types
  for (format = formats.begin(); format != formats.end(); ++format)
    str << ' ' << (int)format->GetPayloadType();

  return str;
}

bool SDPRTPAVPMediaDescription::PrintOn(ostream & str, const PString & connectString) const
{
  // call ancestor
  if (!SDPMediaDescription::PrintOn(str, connectString))
    return false;

  // output attributes for each payload type
  SDPMediaFormatList::const_iterator format;
  for (format = formats.begin(); format != formats.end(); ++format)
    str << *format;

  return true;
}

void SDPRTPAVPMediaDescription::SetAttribute(const PString & attr, const PString & value)
{
  // handle rtpmap attribute
  if (attr *= "rtpmap") {
    PString params = value;
    SDPMediaFormat * format = FindFormat(params);
    if (format != NULL) {
      PStringArray tokens = params.Tokenise('/');
      if (tokens.GetSize() < 2) {
        PTRACE(2, "SDP\tMalformed rtpmap attribute for " << format->GetEncodingName());
        return;
      }

      format->SetEncodingName(tokens[0]);
      format->SetClockRate(tokens[1].AsUnsigned());
      if (tokens.GetSize() > 2)
        format->SetParameters(tokens[2]);
    }
    return;
  }

  return SDPMediaDescription::SetAttribute(attr, value);
}


//////////////////////////////////////////////////////////////////////////////

SDPAudioMediaDescription::SDPAudioMediaDescription(const OpalTransportAddress & address)
  : SDPRTPAVPMediaDescription(address)
{
}


PString SDPAudioMediaDescription::GetSDPMediaType() const 
{ 
  return "audio"; 
}


bool SDPAudioMediaDescription::PrintOn(ostream & str, const PString & connectString) const
{
  // call ancestor
  if (!SDPRTPAVPMediaDescription::PrintOn(str, connectString))
    return false;

  /* The ptime parameter is a recommendation to the remote that we want them
     to send that number of milliseconds on audio in each RTP packet. OPAL
     does not have an equivalent parameter anywhere, so we do not provide it
     on outgoing SDP. We do try to honour it on incoing SDP, however.

     The maxptime parameter can be represented by the RxFramesPerPacketOption,
     so we go through all the codecs offered and calculate a maxptime based on
     the smallest maximum rx packets of the codecs. Allowance must be made for
     maxptime to be at least big enough for 1 frame per packet for the largest
     frame size of those codecs.
     
     In practice this generally means if we mix GSM and G.723.1 then the
     maxptime cannot be smaller than 30ms even if GSM wants one frame per
     packet. That should still work as teh remote cannot send 2fpp as it would
     exceed the 30ms.

     However, certain combinations cannot be represented, e.g. if you want 2fpp
     of G.729 (20ms) and 1fpp of G.723.1 (30ms) then the G.729 codec COULD
     receive 3fpp. This is really a failing in SIP/SDP and the techniques for
     woking around the limitation are for too complicated to be worth doing for
     what should be rare cases.
    */

  unsigned largestFrameTime = 0;
  unsigned maxptime = UINT_MAX;

  // output attributes for each payload type
  for (SDPMediaFormatList::const_iterator format = formats.begin(); format != formats.end(); ++format) {
    const OpalMediaFormat & mediaFormat = format->GetMediaFormat();
    if (mediaFormat.HasOption(OpalAudioFormat::RxFramesPerPacketOption())) {
      unsigned frameTime = mediaFormat.GetFrameTime()/mediaFormat.GetTimeUnits();
      if (largestFrameTime < frameTime)
        largestFrameTime = frameTime;

      unsigned maxptime1 = mediaFormat.GetOptionInteger(OpalAudioFormat::RxFramesPerPacketOption())*frameTime;
      if (maxptime > maxptime1)
        maxptime = maxptime1;
    }
  }

  if (maxptime < UINT_MAX) {
    if (maxptime < largestFrameTime)
      maxptime = largestFrameTime;
    str << "a=maxptime:" << maxptime << "\r\n";
  }

  return true;
}


void SDPAudioMediaDescription::SetAttribute(const PString & attr, const PString & value)
{
  if (attr *= "ptime") {
    SetPacketTime(OpalAudioFormat::TxFramesPerPacketOption(), value);
    return;
  }

  if (attr *= "maxptime") {
    SetPacketTime(OpalAudioFormat::RxFramesPerPacketOption(), value);
    return;
  }

  return SDPRTPAVPMediaDescription::SetAttribute(attr, value);
}


//////////////////////////////////////////////////////////////////////////////

SDPVideoMediaDescription::SDPVideoMediaDescription(const OpalTransportAddress & address)
  : SDPRTPAVPMediaDescription(address)
{
}


PString SDPVideoMediaDescription::GetSDPMediaType() const 
{ 
  return "video"; 
}


static const char * const ContentRoleNames[] = { NULL, "slides", "main", "speaker", "sl" };


bool SDPVideoMediaDescription::PrintOn(ostream & str, const PString & connectString) const
{
  // call ancestor
  if (!SDPRTPAVPMediaDescription::PrintOn(str, connectString))
    return false;

  for (SDPMediaFormatList::const_iterator format = formats.begin(); format != formats.end(); ++format) {
    PINDEX role = format->GetMediaFormat().GetOptionEnum(OpalVideoFormat::ContentRoleOption());
    if (role > 0) {
      str << "a=content:" << ContentRoleNames[role] << "\r\n";
      break;
    }
  }

  return true;
}


void SDPVideoMediaDescription::SetAttribute(const PString & attr, const PString & value)
{
  if (attr *= "content") {
    PINDEX role = 0;
    PStringArray tokens = value.Tokenise(',');
    for (PINDEX i = 0; i < tokens.GetSize(); ++i) {
      for (role = PARRAYSIZE(ContentRoleNames)-1; role > 0; --role) {
        if (tokens[i] *= ContentRoleNames[role])
          break;
      }
      if (role > 0)
        break;
    }
    for (SDPMediaFormatList::iterator format = formats.begin(); format != formats.end(); ++format)
      format->GetWritableMediaFormat().SetOptionEnum(OpalVideoFormat::ContentRoleOption(), role);
    return;
  }

  return SDPRTPAVPMediaDescription::SetAttribute(attr, value);
}



bool SDPVideoMediaDescription::PreEncode()
{
  if (!SDPRTPAVPMediaDescription::PreEncode())
    return false;

  /* Even though the bandwidth parameter COULD be used for audio, we don't
     do it as it has very limited use. Variable bit rate audio codecs are
     not common, and usually there is an fmtp value to select the different
     bit rates. So, as it might cause interoperabity difficulties with the
     dumber implementations out there we just don't.

     As per RFC3890 we set both AS and TIAS parameters.
  */
  for (SDPMediaFormatList::iterator format = formats.begin(); format != formats.end(); ++format) {
    const OpalMediaFormat & mediaFormat = format->GetMediaFormat();

    for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); ++i) {
      const OpalMediaOption & option = mediaFormat.GetOption(i);
      PCaselessString name = option.GetName();
      if (name.NumCompare(SDPBandwidthPrefix, sizeof(SDPBandwidthPrefix)-1) == EqualTo)
        bandwidth.SetMin(name.Mid(sizeof(SDPBandwidthPrefix)-1), option.AsString().AsUnsigned());
    }

    unsigned bw = mediaFormat.GetBandwidth();
    bandwidth.SetMin(SDPSessionDescription::TransportIndependentBandwidthType(), bw);
    bandwidth.SetMin(SDPSessionDescription::ApplicationSpecificBandwidthType(), (bw+999)/1000);
  }

  return true;
}


//////////////////////////////////////////////////////////////////////////////

SDPApplicationMediaDescription::SDPApplicationMediaDescription(const OpalTransportAddress & address)
  : SDPMediaDescription(address)
{
}


PCaselessString SDPApplicationMediaDescription::GetSDPTransportType() const
{ 
  return "rtp/avp"; 
}


PString SDPApplicationMediaDescription::GetSDPMediaType() const 
{ 
  return "application"; 
}


SDPMediaFormat * SDPApplicationMediaDescription::CreateSDPMediaFormat(const PString & portString)
{
  return new SDPMediaFormat(*this, RTP_DataFrame::DynamicBase, portString);
}


PString SDPApplicationMediaDescription::GetSDPPortList() const
{
  PStringStream str;

  // output encoding names for non RTP
  SDPMediaFormatList::const_iterator format;
  for (format = formats.begin(); format != formats.end(); ++format)
    str << ' ' << format->GetEncodingName();

  return str;
}

//////////////////////////////////////////////////////////////////////////////

const PString & SDPSessionDescription::ConferenceTotalBandwidthType()      { static PString s = "CT";   return s; }
const PString & SDPSessionDescription::ApplicationSpecificBandwidthType()  { static PString s = "AS";   return s; }
const PString & SDPSessionDescription::TransportIndependentBandwidthType() { static PString s = "TIAS"; return s; }

SDPSessionDescription::SDPSessionDescription(time_t sessionId, unsigned version, const OpalTransportAddress & address)
  : sessionName(SIP_DEFAULT_SESSION_NAME)
  , ownerUsername('-')
  , ownerSessionId(sessionId)
  , ownerVersion(version)
  , ownerAddress(address)
  , defaultConnectAddress(address)
{
  protocolVersion  = 0;
  direction = SDPMediaDescription::Undefined;
}


void SDPSessionDescription::PrintOn(ostream & str) const
{
  OpalTransportAddress connectionAddress(defaultConnectAddress);

  // see common connect address is needed
  {
    OpalTransportAddress descrAddress;
    PINDEX matched = 0;
    PINDEX descrMatched = 0;
    PINDEX i;
    for (i = 0; i < mediaDescriptions.GetSize(); i++) {
      if (i == 0)
        descrAddress = mediaDescriptions[i].GetTransportAddress();
      if (mediaDescriptions[i].GetTransportAddress() == connectionAddress)
        ++matched;
      if (mediaDescriptions[i].GetTransportAddress() == descrAddress)
        ++descrMatched;
    }
    if (connectionAddress != descrAddress) {
      if ((descrMatched > matched))
        connectionAddress = descrAddress;
      else
        connectionAddress.MakeEmpty();
    }
  }

  /* encode mandatory session information, note the order is important according to RFC!
     Must be vosiuepcbzkatrm and within the m it is icbka */
  str << "v=" << protocolVersion << "\r\n"
         "o=" << ownerUsername << ' '
      << ownerSessionId << ' '
      << ownerVersion << ' '
      << GetConnectAddressString(ownerAddress)
      << "\r\n"
         "s=" << sessionName << "\r\n";

  if (!connectionAddress.IsEmpty())
    str << "c=" << GetConnectAddressString(connectionAddress) << "\r\n";
  
  str << bandwidth
      << "t=" << "0 0" << "\r\n";

  switch (direction) {
    case SDPMediaDescription::RecvOnly:
      str << "a=recvonly" << "\r\n";
      break;
    case SDPMediaDescription::SendOnly:
      str << "a=sendonly" << "\r\n";
      break;
    case SDPMediaDescription::SendRecv:
      str << "a=sendrecv" << "\r\n";
      break;
    case SDPMediaDescription::Inactive:
      str << "a=inactive" << "\r\n";
      break;
    default:
      break;
  }

  // encode media session information
  for (PINDEX i = 0; i < mediaDescriptions.GetSize(); i++) {
    if (mediaDescriptions[i].PreEncode())
      mediaDescriptions[i].Encode(connectionAddress, str);
  }
}


PString SDPSessionDescription::Encode() const
{
  PStringStream str;
  PrintOn(str);
  return str;
}


PBoolean SDPSessionDescription::Decode(const PString & str)
{
  bool ok = true;

  // break string into lines
  PStringArray lines = str.Lines();

  // parse keyvalue pairs
  SDPMediaDescription * currentMedia = NULL;
  PINDEX i;
  for (i = 0; i < lines.GetSize(); i++) {
    PString & line = lines[i];
    PINDEX pos = line.Find('=');
    if (pos == 1) {
      PString value = line.Mid(pos+1).Trim();

      /////////////////////////////////
      //
      // Session description
      //
      /////////////////////////////////
  
      if (currentMedia != NULL && line[0] != 'm') {

        // process all of the "a=rtpmap" lines first so that the media formats are 
        // created before any media description paramaters are processed
        int y = i;
        for (int pass = 0; pass < 2; ++pass) {
          y = i;
          for (y = i; y < lines.GetSize(); ++y) {
            PString & line2 = lines[y];
            PINDEX pos = line2.Find('=');
            if (pos == 1) {
              if (line2[0] == 'm') {
                --y;
                break;
              }
              PString value = line2.Mid(pos+1).Trim();
              bool isRtpMap = value.Left(6) *= "rtpmap";
              if (pass == 0 && isRtpMap)
                currentMedia->Decode(line2[0], value);
              else if (pass == 1 && !isRtpMap)
                currentMedia->Decode(line2[0], value);
            }
          }
        }
        i = y;
      }
      else {
        switch (line[0]) {
          case 'v' : // protocol version (mandatory)
            protocolVersion = value.AsInteger();
            break;

          case 'o' : // owner/creator and session identifier (mandatory)
            ParseOwner(value);
            break;

          case 's' : // session name (mandatory)
            sessionName = value;
            break;

          case 'c' : // connection information - not required if included in all media
            defaultConnectAddress = ParseConnectAddress(value);
            break;

          case 't' : // time the session is active (mandatory)
          case 'i' : // session information
          case 'u' : // URI of description
          case 'e' : // email address
          case 'p' : // phone number
            break;
          case 'b' : // bandwidth information
            bandwidth.Parse(value);
            break;
          case 'z' : // time zone adjustments
          case 'k' : // encryption key
          case 'r' : // zero or more repeat times
            break;
          case 'a' : // zero or more session attribute lines
            if (value *= "sendonly")
              SetDirection (SDPMediaDescription::SendOnly);
            else if (value *= "recvonly")
              SetDirection (SDPMediaDescription::RecvOnly);
            else if (value *= "sendrecv")
              SetDirection (SDPMediaDescription::SendRecv);
            else if (value *= "inactive")
              SetDirection (SDPMediaDescription::Inactive);
            break;

          case 'm' : // media name and transport address (mandatory)
            {
              if (currentMedia != NULL) {
                PTRACE(3, "SDP\tParsed media session with " << currentMedia->GetSDPMediaFormats().GetSize()
                                                            << " '" << currentMedia->GetSDPMediaType() << "' formats");
                if (!currentMedia->PostDecode())
                  ok = false;
              }

              PStringArray tokens = value.Tokenise(" ");
              if (tokens.GetSize() < 4) {
                PTRACE(1, "SDP\tMedia session has only " << tokens.GetSize() << " elements");
                currentMedia = NULL;
              }
              else
              {
                // parse the media type
                PString mt = tokens[0].ToLower();
                OpalMediaType mediaType = OpalMediaType::GetMediaTypeFromSDP(tokens[0], tokens[2]);
                if (mediaType.empty()) {
                  PTRACE(1, "SDP\tUnknown SDP media type " << tokens[0]);
                  currentMedia = NULL;
                }
                else
                {
                  OpalMediaTypeDefinition * defn = mediaType.GetDefinition();
                  if (defn == NULL) {
                    PTRACE(1, "SDP\tNo definition for SDP media type " << tokens[0]);
                    currentMedia = NULL;
                  }
                  else {
                    currentMedia = defn->CreateSDPMediaDescription(defaultConnectAddress);
                    if (currentMedia == NULL) {
                      PTRACE(1, "SDP\tCould not create SDP media description for SDP media type " << tokens[0]);
                    }
                    else if (currentMedia->Decode(tokens)) {
                      mediaDescriptions.Append(currentMedia);
                    }
                    else {
                      delete currentMedia;
                      currentMedia = NULL;
                    }
                  }
                }
              }
            }
            break;

          default:
            PTRACE(1, "SDP\tUnknown session information key " << line[0]);
        }
      }
    }
  }

  if (currentMedia != NULL) {
    PTRACE(3, "SDP\tParsed media session with " << currentMedia->GetSDPMediaFormats().GetSize()
                                                << " '" << currentMedia->GetSDPMediaType() << "' formats");
    if (!currentMedia->PostDecode())
      ok = false;
  }

  return ok;
}


void SDPSessionDescription::ParseOwner(const PString & str)
{
  PStringArray tokens = str.Tokenise(" ");

  if (tokens.GetSize() != 6) {
    PTRACE(2, "SDP\tOrigin has incorrect number of elements (" << tokens.GetSize() << ')');
  }
  else {
    ownerUsername    = tokens[0];
    ownerSessionId   = tokens[1].AsUnsigned();
    ownerVersion     = tokens[2].AsUnsigned();
    ownerAddress = defaultConnectAddress = ParseConnectAddress(tokens, 3);
  }
}


SDPMediaDescription * SDPSessionDescription::GetMediaDescriptionByType(const OpalMediaType & rtpMediaType) const
{
  // look for matching media type
  PINDEX i;
  for (i = 0; i < mediaDescriptions.GetSize(); i++) {
    if (mediaDescriptions[i].GetMediaType() == rtpMediaType)
      return &mediaDescriptions[i];
  }

  return NULL;
}

SDPMediaDescription * SDPSessionDescription::GetMediaDescriptionByIndex(PINDEX index) const
{
  if (index > mediaDescriptions.GetSize())
    return NULL;

  return &mediaDescriptions[index-1];
}

SDPMediaDescription::Direction SDPSessionDescription::GetDirection(unsigned sessionID) const
{
  if (sessionID > 0 && sessionID <= (unsigned)mediaDescriptions.GetSize())
    return mediaDescriptions[sessionID-1].GetDirection();
  
  return defaultConnectAddress.IsEmpty() ? SDPMediaDescription::Inactive : direction;
}


bool SDPSessionDescription::IsHold() const
{
  if (defaultConnectAddress.IsEmpty()) // Old style "hold"
    return true;

  if (GetBandwidth(SDPSessionDescription::ApplicationSpecificBandwidthType()) == 0)
    return true;

  PINDEX i;
  for (i = 0; i < mediaDescriptions.GetSize(); i++) {
    SDPMediaDescription::Direction dir = mediaDescriptions[i].GetDirection();
    if ((dir == SDPMediaDescription::Undefined) || ((dir & SDPMediaDescription::RecvOnly) != 0))
      return false;
  }

  return true;
}


void SDPSessionDescription::SetDefaultConnectAddress(const OpalTransportAddress & address)
{
   defaultConnectAddress = address;
   if (ownerAddress.IsEmpty())
     ownerAddress = address;
}

#endif // OPAL_SIP

// End of file ////////////////////////////////////////////////////////////////
