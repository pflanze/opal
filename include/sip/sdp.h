/*
 * sdp.h
 *
 * Session Description Protocol
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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

#ifndef __OPAL_SDP_H
#define __OPAL_SDP_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <opal/transports.h>
#include <opal/mediafmt.h>
#include <rtp/rtp.h>


/////////////////////////////////////////////////////////

class SDPMediaFormat : public PObject
{
  PCLASSINFO(SDPMediaFormat, PObject);
  public:
    // the following values are mandated by RFC 2833
    enum NTEEvent {
      Digit0 = 0,
      Digit1 = 1,
      Digit2 = 2,
      Digit3 = 3,
      Digit4 = 4,
      Digit5 = 5,
      Digit6 = 6,
      Digit7 = 7,
      Digit8 = 8,
      Digit9 = 9,
      Star   = 10,
      Hash   = 11,
      A      = 12,
      B      = 13,
      C      = 14,
      D      = 15,
      Flash  = 16
    };
    
    SDPMediaFormat(
      RTP_DataFrame::PayloadTypes payloadType,
      const char * name = NULL
    );

    SDPMediaFormat(
      const OpalMediaFormat & mediaFormat,
      RTP_DataFrame::PayloadTypes pt,
      const char * nteString = NULL
    );

    void PrintOn(ostream & str) const;

    RTP_DataFrame::PayloadTypes GetPayloadType() const { return payloadType; }

    PString GetEncodingName() const         { return encodingName; }
    void SetEncodingName(const PString & v) { encodingName = v; }

    void SetFMTP(const PString & _fmtp); 
    PString GetFMTP() const;

    unsigned GetClockRate(void)                        { return clockRate ; }
    void SetClockRate(unsigned  v)                     { clockRate = v; }

    void SetParameters(const PString & v) { parameters = v; }

    void SetPacketTime(const PString & optionName, unsigned ptime);

    const OpalMediaFormat & GetMediaFormat() const;

  protected:
    void AddNTEString(const PString & str);
    void AddNTEToken(const PString & ostr);
    PString GetNTEString() const;

#if OPAL_T38FAX
    void AddNSEString(const PString & str);
    void AddNSEToken(const PString & ostr);
    PString GetNSEString() const;
#endif

    void AddNXEString(POrdinalSet & nxeSet, const PString & str);
    void AddNXEToken(POrdinalSet & nxeSet, const PString & ostr);
    PString GetNXEString(POrdinalSet & nxeSet) const;

    mutable OpalMediaFormat mediaFormat;
    RTP_DataFrame::PayloadTypes payloadType;

    unsigned clockRate;
    PString encodingName;
    PString parameters;
    PString fmtp;

    mutable POrdinalSet nteSet;     // used for NTE formats only
#if OPAL_T38FAX
    mutable POrdinalSet nseSet;     // used for NSE formats only
#endif
};

PLIST(SDPMediaFormatList, SDPMediaFormat);


/////////////////////////////////////////////////////////

class SDPMediaDescription : public PObject
{
  PCLASSINFO(SDPMediaDescription, PObject);
  public:
    enum Direction {
      RecvOnly,
      SendOnly,
      SendRecv,
      Inactive,
      Undefined
    };
    
    SDPMediaDescription(
      const OpalTransportAddress & address,
      const OpalMediaType & mediaType,
      const PCaselessString & _transport
    );

    virtual void PrintOn(ostream & strm) const;
    virtual void PrintOn(const OpalTransportAddress & commonAddr, ostream & str) const;

    virtual PBoolean Decode(const PString & str);

    virtual OpalMediaType GetMediaType() const { return mediaType; }

    virtual const SDPMediaFormatList & GetSDPMediaFormats() const { return formats; }

    virtual OpalMediaFormatList GetMediaFormats(const OpalMediaType & mediaType) const;
    virtual void CreateRTPMap(const OpalMediaSessionId & sessionID, RTP_DataFrame::PayloadMapType & map) const;

    virtual void AddSDPMediaFormat(SDPMediaFormat * sdpMediaFormat);
    virtual void AddSDPMediaFormat(const OpalMediaFormat & mediaFormat, RTP_DataFrame::PayloadTypes pt, const char * nteString = NULL);

    virtual void AddMediaFormat(const OpalMediaFormat & mediaFormat, const RTP_DataFrame::PayloadMapType & map);
    virtual void AddMediaFormats(const OpalMediaFormatList & mediaFormats, const OpalMediaSessionId & sessionId, const RTP_DataFrame::PayloadMapType & map);

    virtual void SetAttribute(const PString & attr, const PString & value);

    virtual void SetDirection(const Direction & d) { direction = d; }
    virtual Direction GetDirection() const { return direction; }

    virtual const OpalTransportAddress & GetTransportAddress() const { return transportAddress; }
    virtual PBoolean SetTransportAddress(const OpalTransportAddress &t);

    virtual PString GetTransport() const         { return transport; }
    virtual void SetTransport(const PString & v) { transport = v; }

    virtual WORD GetPort() const { return port; }

    virtual SDPMediaFormat * CreateSDPMediaFormatFromName(const PString & token) = 0;
    virtual SDPMediaFormat * CreateSDPMediaFormatFromFormat(const OpalMediaFormat & mediaFormat, RTP_DataFrame::PayloadTypes pt, const char * nteString) = 0;

  protected:
    virtual void PrintOn(ostream & strm, const PString & str) const;
    virtual bool PrintFormat(ostream & str) const;

    virtual SDPMediaFormat * FindFormat(PString & str) const;
    virtual void SetPacketTime(const PString & optionName, const PString & value);

    OpalMediaType mediaType;
    WORD portCount;
    PCaselessString transport;
    OpalTransportAddress transportAddress;
    WORD port;

    Direction direction;

    SDPMediaFormatList formats;

    OpalMediaTypeDefinition * mediaTypeDefn;
};

PLIST(SDPMediaDescriptionList, SDPMediaDescription);

/////////////////////////////////////////////////////////

class OpalRTPAVPSDPMediaDescription : public SDPMediaDescription
{
  public:
    OpalRTPAVPSDPMediaDescription(const OpalMediaType & _mediaType, const OpalTransportAddress & _address);
    SDPMediaFormat * CreateSDPMediaFormatFromName(const PString & token);
    SDPMediaFormat * CreateSDPMediaFormatFromFormat(const OpalMediaFormat & mediaFormat, RTP_DataFrame::PayloadTypes pt, const char * nteString = NULL);

    void SetAttribute(const PString & attr, const PString & value);
    bool PrintFormat(ostream & str) const;
};

////////////////////////////////////////////////////////////////////////////

#if OPAL_AUDIO

class OpalAudioSDPMediaDescription : public OpalRTPAVPSDPMediaDescription
{
  public:
    OpalAudioSDPMediaDescription(const OpalTransportAddress & _address);
    void SetAttribute(const PString & attr, const PString & value);
};

#endif

////////////////////////////////////////////////////////////////////////////

#if OPAL_VIDEO

class OpalVideoSDPMediaDescription : public OpalRTPAVPSDPMediaDescription
{
  public:
    OpalVideoSDPMediaDescription(const OpalTransportAddress & _address);
    void SetAttribute(const PString & attr, const PString & value);
};

#endif

/////////////////////////////////////////////////////////

class SDPSessionDescription : public PObject
{
  PCLASSINFO(SDPSessionDescription, PObject);
  public:
    SDPSessionDescription(
      const OpalTransportAddress & address = OpalTransportAddress()
    );

    typedef unsigned int SessionKey;

    void PrintOn(ostream & strm) const;
    PString Encode() const;
    PBoolean Decode(const PString & str);

    void SetSessionName(const PString & v) { sessionName = v; }
    PString GetSessionName() const         { return sessionName; }

    void SetUserName(const PString & v)    { ownerUsername = v; }
    PString GetUserName() const            { return ownerUsername; }

    const SDPMediaDescriptionList & GetMediaDescriptions() const { return mediaDescriptions; }

    SDPMediaDescription * GetMediaDescription(const SessionKey & rtpMediaType) const;
    SDPMediaDescription * GetMediaDescription(const std::string & rtpMediaType) const;
    void AddMediaDescription(SDPMediaDescription * md) { mediaDescriptions.Append(md); }
    
    void SetDirection(const SDPMediaDescription::Direction & d) { direction = d; }
    SDPMediaDescription::Direction GetDirection(const OpalMediaSessionId & sessionId) const;

    const OpalTransportAddress & GetDefaultConnectAddress() const { return defaultConnectAddress; }
    void SetDefaultConnectAddress(
      const OpalTransportAddress & address
    ) { defaultConnectAddress = address; }
	
    const PString & GetBandwidthModifier() const { return bandwidthModifier; }
    void SetBandwidthModifier(const PString & modifier) { bandwidthModifier = modifier; }

    PINDEX GetBandwidthValue() const { return bandwidthValue; }
    void SetBandwidthValue(PINDEX value) { bandwidthValue = value; }

    static const PString & ConferenceTotalBandwidthModifier();
    static const PString & ApplicationSpecificBandwidthModifier();

  protected:
    void ParseOwner(const PString & str);

    SDPMediaDescriptionList mediaDescriptions;
    SDPMediaDescription::Direction direction;

    PINDEX protocolVersion;
    PString sessionName;

    PString ownerUsername;
    unsigned ownerSessionId;
    unsigned ownerVersion;
    OpalTransportAddress ownerAddress;
    OpalTransportAddress defaultConnectAddress;
    WORD defaultConnectPort;
	
    PString bandwidthModifier;
    PINDEX bandwidthValue;
};

/////////////////////////////////////////////////////////


#endif // __OPAL_SDP_H


// End of File ///////////////////////////////////////////////////////////////
