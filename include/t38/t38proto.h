/*
 * t38proto.h
 *
 * T.38 protocol handler
 *
 * Open Phone Abstraction Library
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#ifndef __OPAL_T38PROTO_H
#define __OPAL_T38PROTO_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <ptlib/pipechan.h>

#include <opal/mediafmt.h>
#include <opal/mediastrm.h>
#include <opal/endpoint.h>

class OpalTransport;
class T38_IFPPacket;
class PASN_OctetString;

namespace PWLibStupidLinkerHacks {
  extern int t38Loader;
};

///////////////////////////////////////////////////////////////////////////////
//
//  declare a media type for T.38
//

class OpalImageMediaType : public OpalMediaTypeDefinition 
{
  public:
    BYTE GetPreferredSessionId() const;
    RTP_UDP * CreateNonSecureSession(OpalConnection & conn, PHandleAggregator * aggregator, const OpalMediaSessionId & id, PBoolean remoteIsNAT);
    OpalMediaStream * CreateMediaStream(OpalConnection & conn, const OpalMediaFormat & mediaFormat,const OpalMediaSessionId & sessionID, PBoolean isSource);

#if OPAL_H323
    virtual H323Channel * CreateH323Channel(H323Connection & conn, 
                                      const H323Capability & capability, 
                                                    unsigned direction, 
                                               RTP_Session & session,
                                  const OpalMediaSessionId & sessionId,
                  const H245_H2250LogicalChannelParameters * param);
#endif

#if OPAL_SIP
    SDPMediaDescription * CreateSDPMediaDescription(const OpalMediaType &, OpalTransportAddress & localAddress);
#endif
};

///////////////////////////////////////////////////////////////////////////////

/**This class handles the processing of the T.38 protocol.
  */
class OpalT38Protocol : public PObject
{
    PCLASSINFO(OpalT38Protocol, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new protocol handler.
     */
    OpalT38Protocol();

    /**Destroy the protocol handler.
     */
    ~OpalT38Protocol();
  //@}

  /**@name Operations */
  //@{
    /**This is called to clean up any threads on connection termination.
     */
    virtual void Close();

    /**Handle the origination of a T.38 connection.
       An application would normally override this. The default just sends
       "heartbeat" T.30 no signal indicator.
      */
    virtual PBoolean Originate();

    /**Write packet to the T.38 connection.
      */
    virtual PBoolean WritePacket(
      const T38_IFPPacket & pdu
    );

    /**Write T.30 indicator packet to the T.38 connection.
      */
    virtual PBoolean WriteIndicator(
      unsigned indicator
    );

    /**Write data packet to the T.38 connection.
      */
    virtual PBoolean WriteMultipleData(
      unsigned mode,
      PINDEX count,
      unsigned * type,
      const PBYTEArray * data
    );

    /**Write data packet to the T.38 connection.
      */
    virtual PBoolean WriteData(
      unsigned mode,
      unsigned type,
      const PBYTEArray & data
    );

    /**Handle the origination of a T.38 connection.
      */
    virtual PBoolean Answer();

    /**Handle incoming T.38 packet.

       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean HandlePacket(
      const T38_IFPPacket & pdu
    );

    /**Handle lost T.38 packets.

       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean HandlePacketLost(
      unsigned nLost
    );

    /**Handle incoming T.38 indicator packet.
       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean OnIndicator(
      unsigned indicator
    );

    /**Handle incoming T.38 CNG indicator.
       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean OnCNG();

    /**Handle incoming T.38 CED indicator.
       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean OnCED();

    /**Handle incoming T.38 V.21 preamble indicator.
       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean OnPreamble();

    /**Handle incoming T.38 data mode training indicator.
       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean OnTraining(
      unsigned indicator
    );

    /**Handle incoming T.38 data packet.

       If returns PFalse, then the reading loop should be terminated.
      */
    virtual PBoolean OnData(
      unsigned mode,
      unsigned type,
      const PBYTEArray & data
    );
  //@}

    OpalTransport * GetTransport() const { return transport; }
    void SetTransport(
      OpalTransport * transport,
      PBoolean autoDelete = PTrue
    );

  protected:
    PBoolean HandleRawIFP(
      const PASN_OctetString & pdu
    );

    OpalTransport * transport;
    PBoolean            autoDeleteTransport;

    PBoolean     corrigendumASN;
    unsigned indicatorRedundancy;
    unsigned lowSpeedRedundancy;
    unsigned highSpeedRedundancy;

    int               lastSentSequenceNumber;
    PList<PBYTEArray> redundantIFPs;
};


///////////////////////////////////////////////////////////////////////////////

#if OPAL_AUDIO

/**
  *  This format is identical to the OpalPCM16 except that it uses a different
  *  sessionID in order to be compatible with T.38
  */

class OpalFaxAudioFormat : public OpalMediaFormat
{
  friend class OpalPluginCodecManager;
    PCLASSINFO(OpalFaxAudioFormat, OpalMediaFormat);
  public:
    OpalFaxAudioFormat(
      const char * fullName,    ///<  Full name of media format
      RTP_DataFrame::PayloadTypes rtpPayloadType, ///<  RTP payload type code
      const char * encodingName,///<  RTP encoding name
      PINDEX   frameSize,       ///<  Size of frame in bytes (if applicable)
      unsigned frameTime,       ///<  Time for frame in RTP units (if applicable)
      unsigned rxFrames,        ///<  Maximum number of frames per packet we can receive
      unsigned txFrames,        ///<  Desired number of frames per packet we transmit
      unsigned maxFrames = 256, ///<  Maximum possible frames per packet
      unsigned clockRate = 8000, ///<  Clock Rate 
      time_t timeStamp = 0       ///<  timestamp (for versioning)
    );
};

#endif

///////////////////////////////////////////////////////////////////////////////

/**
  *  This defines a pseudo RTP session used for T.38 channels
  */
/**This class is for the IETF Real Time Protocol interface on UDP/IP.
 */
class T38PseudoRTP : public RTP_UDP
{
  PCLASSINFO(T38PseudoRTP, RTP_UDP);

  public:
  /**@name Construction */
  //@{
    /**Create a new RTP channel.
     */
    T38PseudoRTP(
      PHandleAggregator * aggregator,  ///< RTP aggregator
      const OpalMediaSessionId & id,   ///<  Session ID for RTP channel
      PBoolean remoteIsNAT             ///<  PTrue is remote is behind NAT
    );

    /// Destroy the RTP
    ~T38PseudoRTP();

    PBoolean ReadData(RTP_DataFrame & frame, PBoolean loop);
    PBoolean WriteData(RTP_DataFrame & frame);
    RTP_Session::SendReceiveStatus OnSendData(RTP_DataFrame & frame);
    RTP_Session::SendReceiveStatus OnSendControl(RTP_ControlFrame & /*frame*/, PINDEX & /*len*/);

    RTP_Session::SendReceiveStatus ReadDataPDU(RTP_DataFrame & frame);
    RTP_Session::SendReceiveStatus OnReceiveData(RTP_DataFrame & frame);

    PBoolean SetRemoteSocketInfo(PIPSocket::Address address, WORD port, PBoolean isDataPort);

  protected:
    int WaitForPDU(PUDPSocket & dataSocket, PUDPSocket & controlSocket, const PTimeInterval & timeout);
    PBoolean OnTimeout(RTP_DataFrame & frame);
    PBoolean corrigendumASN;
    int consecutiveBadPackets;

    PBYTEArray lastIFP;

#if 0
    PList<PBYTEArray> redundantIFPs;
#endif

  //@}
};

///////////////////////////////////////////////////////////////////////////////

class OpalFaxCallInfo {
  public:
    OpalFaxCallInfo();
    PUDPSocket socket;
    PPipeChannel spanDSP;
    unsigned refCount;
    PIPSocket::Address spanDSPAddr;
    WORD spanDSPPort;
};

///////////////////////////////////////////////////////////////////////////////

/**This class describes a media stream that transfers data to/from a fax session
  */
class OpalFaxMediaStream : public OpalMediaStream
{
  PCLASSINFO(OpalFaxMediaStream, OpalMediaStream);
  public:
  /**@name Construction */
  //@{
    /**Construct a new media stream for T.38 sessions.
      */
    OpalFaxMediaStream(
      OpalConnection & conn,
      const OpalMediaFormat & mediaFormat, ///<  Media format for stream
      const OpalMediaSessionId & sessionId, 
      PBoolean isSource ,                      ///<  Is a source stream
      const PString & token,               ///<  token used to match incoming/outgoing streams
      const PString & filename,
      PBoolean receive
    );
  //@}

  /**@name Overrides of OpalMediaStream class */
  //@{
    /**Open the media stream using the media format.

       The default behaviour simply sets the isOpen variable to PTrue.
      */
    virtual PBoolean Open();

    /**Close the media stream.

       The default does nothing.
      */
    virtual PBoolean Close();

    /**Start the media stream.

       The default behaviour calls Resume() on the associated OpalMediaPatch
       thread if it was suspended.
      */
    virtual PBoolean Start();

    /**Read an RTP frame of data from the source media stream.
       The new behaviour simply calls RTP_Session::ReadData().
      */
    virtual PBoolean ReadPacket(
      RTP_DataFrame & packet
    );

    /**Write an RTP frame of data to the sink media stream.
       The new behaviour simply calls RTP_Session::WriteData().
      */
    virtual PBoolean WritePacket(
      RTP_DataFrame & packet
    );

    /**Indicate if the media stream is synchronous.
       Returns PFalse for RTP streams.
      */
    virtual PBoolean IsSynchronous() const;

    virtual PString GetSpanDSPCommandLine(OpalFaxCallInfo &);

  //@}

  protected:
    PMutex infoMutex;
    PString sessionToken;
    OpalFaxCallInfo * faxCallInfo;
    PFilePath filename;
    PBoolean receive;
    BYTE writeBuffer[320];
    PINDEX writeBufferLen;
};

///////////////////////////////////////////////////////////////////////////////

/**This class describes a media stream that transfers data to/from a T.38 session
  */
class OpalT38MediaStream : public OpalFaxMediaStream
{
  PCLASSINFO(OpalT38MediaStream, OpalFaxMediaStream);
  public:
    OpalT38MediaStream(
      OpalConnection & conn,
      const OpalMediaFormat & mediaFormat, ///<  Media format for stream
      const OpalMediaSessionId & sessionID, 
      PBoolean isSource ,                      ///<  Is a source stream
      const PString & token,               ///<  token used to match incoming/outgoing streams
      const PString & filename,            ///<  filename
      PBoolean receive
    );

    PString GetSpanDSPCommandLine(OpalFaxCallInfo &);

    PBoolean ReadPacket(RTP_DataFrame & packet);
    PBoolean WritePacket(RTP_DataFrame & packet);
};

///////////////////////////////////////////////////////////////////////////////

class OpalFaxConnection;

/** Fax Endpoint.
 */
class OpalFaxEndPoint : public OpalEndPoint
{
  PCLASSINFO(OpalFaxEndPoint, OpalEndPoint);
  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint.
     */
    OpalFaxEndPoint(
      OpalManager & manager,      ///<  Manager of all endpoints.
      const char * prefix = "fax" ///<  Prefix for URL style address strings
    );

    /**Destroy endpoint.
     */
    ~OpalFaxEndPoint();
  //@}

    virtual PBoolean MakeConnection(
      OpalCall & call,          ///<  Owner of connection
      const PString & party,    ///<  Remote party to call
      void * userData = NULL,          ///<  Arbitrary data to pass to connection
      unsigned int options = 0,     ///<  options to pass to conneciton
      OpalConnection::StringOptions * stringOptions = NULL
    );

    /**Create a connection for the fax endpoint.
      */
    virtual OpalFaxConnection * CreateConnection(
      OpalCall & call,          ///< Owner of connection
      const PString & filename, ///< filename to send/receive
      PBoolean receive,
      void * userData = NULL,   ///< Arbitrary data to pass to connection
      OpalConnection::StringOptions * stringOptions = NULL
    );

    /**Get the data formats this endpoint is capable of operating.
       This provides a list of media data format names that may be used by an
       OpalMediaStream may be created by a connection from this endpoint.

       Note that a specific connection may not actually support all of the
       media formats returned here, but should return no more.

       The default behaviour is pure.
      */
    virtual OpalMediaFormatList GetMediaFormats() const;

    virtual PString MakeToken();

  /**@name User Interface operations */
    /**Accept the incoming connection.
      */
    virtual void AcceptIncomingConnection(
      const PString & connectionToken ///<  Token of connection to accept call
    );

    /**Call back when patching a media stream.
       This function is called when a connection has created a new media
       patch between two streams.
      */
    virtual void OnPatchMediaStream(
      const OpalFaxConnection & connection, ///<  Connection having new patch
      PBoolean isSource,                         ///<  Source patch
      OpalMediaPatch & patch                 ///<  New patch
    );
  //@}
};

///////////////////////////////////////////////////////////////////////////////

/** Fax Connection
 */
class OpalFaxConnection : public OpalConnection
{
  PCLASSINFO(OpalFaxConnection, OpalConnection);
  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint.
     */
    OpalFaxConnection(
      OpalCall & call,                 ///<   Owner calll for connection
      OpalFaxEndPoint & endpoint,      ///<   Owner endpoint for connection
      const PString & filename,        ///<   filename to send/receive
      PBoolean receive,                    ///<   PTrue if receiving a fax
      const PString & _token,           ///<  token for connection
      OpalConnection::StringOptions * stringOptions = NULL
    );

    /**Destroy endpoint.
     */
    ~OpalFaxConnection();
  //@}

  /**@name Overrides from OpalConnection */
  //@{
    /**Start an outgoing connection.
       This function will initiate the connection to the remote entity, for
       example in H.323 it sends a SETUP, in SIP it sends an INVITE etc.

       The default behaviour does.
      */
    virtual PBoolean SetUpConnection();

    /**Indicate to remote endpoint an alert is in progress.
       If this is an incoming connection and the AnswerCallResponse is in a
       AnswerCallDeferred or AnswerCallPending state, then this function is
       used to indicate to that endpoint that an alert is in progress. This is
       usually due to another connection which is in the call (the B party)
       has received an OnAlerting() indicating that its remote endpoint is
       "ringing".

       The default behaviour does nothing.
      */
    virtual PBoolean SetAlerting(
      const PString & calleeName,   ///<  Name of endpoint being alerted.
      PBoolean withMedia                ///<  Open media with alerting
    );

    /**Indicate to remote endpoint we are connected.

       The default behaviour does nothing.
      */
    virtual PBoolean SetConnected();

    /**Get the data formats this connection is capable of operating.
       This provides a list of media data format names that an
       OpalMediaStream may be created in within this connection.

       The default behaviour returns the formats the PSoundChannel can do,
       typically only PCM-16.
      */
    virtual OpalMediaFormatList GetMediaFormats() const;

    OpalMediaStream * CreateMediaStream(const OpalMediaFormat & mediaFormat, const OpalMediaSessionId & sessionID, PBoolean isSource);

    /**Call back when patching a media stream.
       This function is called when a connection has created a new media
       patch between two streams.
       Add the echo canceler patch and call the endpoint function of
       the same name.
       Add a PCM silence detector filter.
      */
    virtual void OnPatchMediaStream(
      PBoolean isSource,
      OpalMediaPatch & patch    ///<  New patch
    );

    /**Open source transmitter media stream for session.
      */
    virtual PBoolean OpenSourceMediaStream(
      const OpalMediaFormatList & mediaFormats, ///<  Optional media format to open
      const OpalMediaSessionId & sessionID           ///<  Session to start stream on
    );

    /**Open source transmitter media stream for session.
      */
    virtual OpalMediaStream * OpenSinkMediaStream(
      OpalMediaStream & source    ///<  Source media sink format to open to
    );

  /**@name New operations */
  //@{
    /**Accept the incoming connection.
      */
    virtual void AcceptIncoming();

  //@}

    void AdjustMediaFormats(OpalMediaFormatList & mediaFormats) const;

    PFilePath GetFilename() const { return filename; }
    bool IsFaxReceive() const     { return receive; }

  protected:
    OpalFaxEndPoint & endpoint;
    PString filename;
    PBoolean receive;
    PBoolean forceFaxAudio;
};

/////////////////////////////////////////////////////////////////////////////////

class OpalT38Connection;

/** T.38 Endpoint
 */
class OpalT38EndPoint : public OpalFaxEndPoint
{
  PCLASSINFO(OpalT38EndPoint, OpalFaxEndPoint);
  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint.
     */
    OpalT38EndPoint(
      OpalManager & manager,      ///<  Manager of all endpoints.
      const char * prefix = "t38" ///<  Prefix for URL style address strings
    );
    OpalMediaFormatList GetMediaFormats() const;
    PString MakeToken();
    virtual OpalFaxConnection * CreateConnection(OpalCall & call, const PString & filename, PBoolean receive, void * /*userData*/, OpalConnection::StringOptions * stringOptions);
};

///////////////////////////////////////////////////////////////////////////////

/** T.38 Connection
 */
class OpalT38Connection : public OpalFaxConnection
{
  PCLASSINFO(OpalT38Connection, OpalFaxConnection);
  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint.
     */
    OpalT38Connection(
      OpalCall & call,                 ///<  Owner calll for connection
      OpalT38EndPoint & endpoint,      ///<  Owner endpoint for connection
      const PString & filename,        ///<  filename to send/receive
      PBoolean receive,
      const PString & _token,           ///<  token for connection
      OpalConnection::StringOptions * stringOptions = NULL
    );
    void AdjustMediaFormats(OpalMediaFormatList & mediaFormats) const;
    OpalMediaFormatList GetMediaFormats() const;
};

///////////////////////////////////////////////////////////////////////////////

#define OPAL_T38            "T.38"
#define OPAL_PCM16_FAX      "PCM-16-Fax"

extern const OpalFaxAudioFormat & GetOpalPCM16Fax();
extern const OpalMediaFormat    & GetOpalT38();

#define OpalPCM16Fax          GetOpalPCM16Fax()
#define OpalT38               GetOpalT38()


#endif // __OPAL_T38PROTO_H


