/*
 * channels.h
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2001 Equivalence Pty. Ltd.
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
 * $Log: channels.h,v $
 * Revision 1.2005  2002/01/22 04:56:55  robertj
 * Update from OpenH323, rev 1.26 to 1.28
 *
 * Revision 2.3  2002/01/14 06:35:56  robertj
 * Updated to OpenH323 v1.7.9
 *
 * Revision 2.2  2001/11/02 10:45:19  robertj
 * Updated to OpenH323 v1.7.3
 *
 * Revision 2.1  2001/10/15 04:30:09  robertj
 * Added delayed start of media patch threads.
 *
 * Revision 2.0  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.28  2002/01/17 07:04:57  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.27  2002/01/17 00:10:37  robertj
 * Fixed double copy of rtpPayloadType in RTP channel, caused much confusion.
 *
 * Revision 1.26  2002/01/14 05:18:16  robertj
 * Fixed typo on external RTP channel constructor.
 *
 * Revision 1.25  2002/01/10 05:13:50  robertj
 * Added support for external RTP stacks, thanks NuMind Software Systems.
 *
 * Revision 1.24  2001/10/23 02:18:06  dereks
 * Initial release of CU30 video codec.
 *
 * Revision 1.23  2001/07/24 02:26:53  robertj
 * Added start for handling reverse channels.
 *
 * Revision 1.22  2001/07/17 04:44:29  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.21  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.20  2001/01/25 07:27:14  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.19  2000/10/19 04:05:20  robertj
 * Added compare function for logical channel numbers, thanks Yuriy Ershov.
 *
 * Revision 1.18  2000/09/22 01:35:02  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.17  2000/08/31 08:15:32  robertj
 * Added support for dynamic RTP payload types in H.245 OpenLogicalChannel negotiations.
 *
 * Revision 1.16  2000/08/21 02:50:19  robertj
 * Fixed race condition if close call just as slow start media channels are opening.
 *
 * Revision 1.15  2000/06/15 01:46:13  robertj
 * Added channel pause (aka mute) functions.
 *
 * Revision 1.14  2000/05/18 11:53:33  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.13  2000/05/02 04:32:23  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.12  1999/12/23 23:02:34  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.11  1999/11/19 09:06:30  robertj
 * Changed to close down logical channel if get a transmit codec error.
 *
 * Revision 1.10  1999/11/06 05:37:44  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.9  1999/09/08 04:05:48  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.8  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.7  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.6  1999/06/14 05:15:55  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.5  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.4  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.3  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.2  1999/04/26 06:14:46  craigs
 * Initial implementation for RTP decoding and lots of stuff
 * As a whole, these changes are called "First Noise"
 *
 * Revision 1.1  1999/01/16 01:31:07  robertj
 * Initial revision
 *
 */

#ifndef __H323_CHANNELS_H
#define __H323_CHANNELS_H

#ifdef __GNUC__
#pragma interface
#endif


#include <rtp/rtp.h>
#include <h323/transaddr.h>


class OpalMediaStream;

class H245_OpenLogicalChannel;
class H245_OpenLogicalChannelAck;
class H245_OpenLogicalChannel_forwardLogicalChannelParameters;
class H245_OpenLogicalChannel_reverseLogicalChannelParameters;
class H245_H2250LogicalChannelParameters;
class H245_H2250LogicalChannelAckParameters;
class H245_MiscellaneousCommand_type;
class H245_MiscellaneousIndication_type;

class H323EndPoint;
class H323Connection;
class H323Capability;
class H323_RTP_Session;

class OpalListener;
class OpalTransport;


///////////////////////////////////////////////////////////////////////////////

/**Description of a Logical Channel Number.
   This is used as index into dictionary of logical channels.
 */
class H323ChannelNumber : public PObject
{
  PCLASSINFO(H323ChannelNumber, PObject);

  public:
    H323ChannelNumber() { number = 0; fromRemote = FALSE; }
    H323ChannelNumber(unsigned number, BOOL fromRemote);

    virtual PObject * Clone() const;
    virtual PINDEX HashFunction() const;
    virtual void PrintOn(ostream & strm) const;
    virtual Comparison Compare(const PObject & obj) const;

    H323ChannelNumber & operator++(int);
    operator unsigned() const { return number; }
    BOOL IsFromRemote() const { return fromRemote; }
    
  protected:
    unsigned number;
    BOOL     fromRemote;
};


/**This class describes a logical channel between the two endpoints. They may
   be created and deleted as required in the H245 protocol.

   An application may create a descendent off this class and override
   functions as required for operating the channel protocol.
 */
class H323Channel : public PObject
{
  PCLASSINFO(H323Channel, PObject);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323Channel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability   /// Capability channel is using
    );

    /**Destroy new channel.
       To avoid usage of deleted objects in background threads, this waits
       for the H323LogicalChannelThread to terminate before continuing.
     */
    ~H323Channel();
  //@}

  /**@name Overrides from PObject */
  //@{
    virtual void PrintOn(
      ostream & strm
    ) const;
  //@}

  /**@name Operations */
  //@{
    enum Directions {
      IsBidirectional,
      IsTransmitter,
      IsReceiver
    };

    /**Indicate the direction of the channel.
       Return if the channel is bidirectional, or unidirectional, and which
       direction for the latter case.
     */
    virtual Directions GetDirection() const = 0;

    /**Indicate the session number of the channel.
       Return session for channel. This is primarily for use by RTP based
       channels, for channels for which the concept of a session is not
       meaningfull, the default simply returns 0.
     */
    virtual unsigned GetSessionID() const;

    /**Set the initial bandwidth for the channel.
       This calculates the initial bandwidth required by the channel and
       returns TRUE if the connection can support this bandwidth.

       The default behaviour gets the bandwidth requirement from the codec
       object created by the channel.
     */
    virtual BOOL SetInitialBandwidth() = 0;

    /**Open the channel.
       The default behaviour just calls connection.OnStartLogicalChannel() and
       if successful sets the opened member variable.
      */
    virtual BOOL Open();

    /**This is called when the channel can start transferring data.
     */
    virtual BOOL Start() = 0;

    /**This is called to clean up any threads on connection termination.
     */
    virtual void Close();

    /**Indicate if has been opened.
     */
    BOOL IsOpen() const { return opened && !terminating; }

    /**Get the media stream associated with this logical channel.
       The default behaviour returns NULL.
      */
    virtual OpalMediaStream * GetMediaStream() const = 0;

    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_OpenLogicalChannel & openPDU  /// Open PDU to send. 
    ) const = 0;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.

       The default behaviour does nothing.
     */
    virtual void OnSendOpenAck(
      const H245_OpenLogicalChannel & open,   /// Open PDU
      H245_OpenLogicalChannelAck & ack        /// Acknowledgement PDU
    ) const;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour just returns TRUE.
     */
    virtual BOOL OnReceivedPDU(
      const H245_OpenLogicalChannel & pdu,    /// Open PDU
      unsigned & errorCode                    /// Error code on failure
    );

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour just returns TRUE.
     */
    virtual BOOL OnReceivedAckPDU(
      const H245_OpenLogicalChannelAck & pdu  /// Acknowledgement PDU
    );

    /**Limit bit flow for the logical channel.
       The default behaviour passes this on to the codec if not NULL.
     */
    virtual void OnFlowControl(
      long bitRateRestriction   /// Bit rate limitation
    );

    /**Process a miscellaneous command on the logical channel.
       The default behaviour passes this on to the codec if not NULL.
     */
    virtual void OnMiscellaneousCommand(
      const H245_MiscellaneousCommand_type & type  /// Command to process
    );

    /**Process a miscellaneous indication on the logical channel.
       The default behaviour passes this on to the codec if not NULL.
     */
    virtual void OnMiscellaneousIndication(
      const H245_MiscellaneousIndication_type & type  /// Indication to process
    );

    /**Limit bit flow for the logical channel.
       The default behaviour does nothing.
     */
    virtual void OnJitterIndication(
      DWORD jitter,           /// Estimated received jitter in microseconds
      int skippedFrameCount,  /// Frames skipped by decodec
      int additionalBuffer    /// Additional size of video decoder buffer
    );

    /**
       Send a miscellanous command to the remote transmitting video codec.
       Typically, used to indicate a problem in the received video stream.
    */
    void SendMiscCommand(unsigned command);
  //@}

  /**@name Member variable access */
  //@{
    /**Get the number of the channel.
     */
    const H323ChannelNumber & GetNumber() const { return number; }

    /**Set the number of the channel.
     */
    void SetNumber(const H323ChannelNumber & num) { number = num; }

    /**Get the number of the reverse channel (if present).
     */
    const H323ChannelNumber & GetReverseChannel() const { return reverseChannel; }

    /**Set the number of the reverse channel (if present).
     */
    void SetReverseChannel(const H323ChannelNumber & num) { reverseChannel = num; }

    /**Get the bandwidth used by the channel in 100's of bits/sec.
     */
    unsigned GetBandwidthUsed() const { return bandwidthUsed; }

    /**Get the bandwidth used by the channel in 100's of bits/sec.
     */
    BOOL SetBandwidthUsed(
      unsigned bandwidth  /// New bandwidth
    );

    /**Get the capability that created this channel.
      */
    const H323Capability & GetCapability() const { return capability; }

    /**Get the "pause" flag.
       A paused channel is one that prevents the annunciation of the channels
       data. For example for audio this would mute the data, for video it would
       still frame.

       Note that channel is not stopped, and may continue to actually receive
       data, it is just that nothing is done with it.
      */
    BOOL IsPaused() const { return paused; }

    /**Set the "pause" flag.
       A paused channel is one that prevents the annunciation of the channels
       data. For example for audio this would mute the data, for video it would
       still frame.

       Note that channel is not stopped, and may continue to actually receive
       data, it is just that nothing is done with it.
      */
    void SetPause(
      BOOL pause   /// New pause flag
    ) { paused = pause; }
  //@}

  protected:
    H323EndPoint         & endpoint;
    H323Connection       & connection;
    const H323Capability & capability;
    H323ChannelNumber      number;
    H323ChannelNumber      reverseChannel;
    BOOL                   opened;
    BOOL                   paused;
    BOOL                   terminating;

  private:
    unsigned bandwidthUsed;
};


PLIST(H323LogicalChannelList, H323Channel);



/**This class describes a unidirectional logical channel between the two
   endpoints. They may be created and deleted as required in the H245 protocol.

   An application may create a descendent off this class and override
   functions as required for operating the channel protocol.
 */
class H323UnidirectionalChannel : public H323Channel
{
  PCLASSINFO(H323UnidirectionalChannel, H323Channel);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323UnidirectionalChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability,  /// Capability channel is using
      Directions direction                /// Direction of channel
    );
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**Indicate the direction of the channel.
       Return if the channel is bidirectional, or unidirectional, and which
       direction for th latter case.
     */
    virtual Directions GetDirection() const;

    /**Set the initial bandwidth for the channel.
       This calculates the initial bandwidth required by the channel and
       returns TRUE if the connection can support this bandwidth.

       The default behaviour gets the bandwidth requirement from the codec
       object created by the channel.
     */
    virtual BOOL SetInitialBandwidth();

    /**Open the channel.
      */
    virtual BOOL Open();

    /**This is called when the channel can start transferring data.
       The default action is to start one threads, with it either calling
       Receive() or Transmit() depending on the receiver member variable.
     */
    virtual BOOL Start();

    /**This is called to clean up any threads on connection termination.
     */
    virtual void Close();

    /**Get the media stream associated with this logical channel.
       The default behaviour returns NULL.
      */
    virtual OpalMediaStream * GetMediaStream() const;
  //@}

  protected:
    BOOL              receiver;
    OpalMediaStream * mediaStream;
};


/**This class describes a bidirectional logical channel between the two
   endpoints. They may be created and deleted as required in the H245 protocol.

   An application may create a descendent off this class and override
   functions as required for operating the channel protocol.
 */
class H323BidirectionalChannel : public H323Channel
{
  PCLASSINFO(H323BidirectionalChannel, H323Channel);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323BidirectionalChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability   /// Capability channel is using
    );
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**Indicate the direction of the channel.
       Return if the channel is bidirectional, or unidirectional, and which
       direction for th latter case.
     */
    virtual Directions GetDirection() const;

    /**This is called when the channel can start transferring data.
       The default action is to start two threads, one calls Receive() and the
       other calls Transmit().
     */
    virtual BOOL Start();
  //@}
};


///////////////////////////////////////////////////////////////////////////////

/**This class is for encpsulating the IETF Real Time Protocol interface.
 */
class H323_RealTimeChannel : public H323UnidirectionalChannel
{
  PCLASSINFO(H323_RealTimeChannel, H323UnidirectionalChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323_RealTimeChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability,  /// Capability channel is using
      Directions direction                /// Direction of channel
    );
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_OpenLogicalChannel & openPDU  /// Open PDU to send. 
    ) const;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendOpenAck(
      const H245_OpenLogicalChannel & open,   /// Open PDU
      H245_OpenLogicalChannelAck & ack        /// Acknowledgement PDU
    ) const;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default makes sure the parameters are compatible and passes on
       the PDU to the rtp session.
     */
    virtual BOOL OnReceivedPDU(
      const H245_OpenLogicalChannel & pdu,    /// Open PDU
      unsigned & errorCode                    /// Error code on failure
    );

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default makes sure the parameters are compatible and passes on
       the PDU to the rtp session.
     */
    virtual BOOL OnReceivedAckPDU(
      const H245_OpenLogicalChannelAck & pdu /// Acknowledgement PDU
    );
  //@}

  /**@name Operations */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_H2250LogicalChannelParameters & param  /// Open PDU to send.
    ) const = 0;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendOpenAck(
      H245_H2250LogicalChannelAckParameters & param /// Acknowledgement PDU
    ) const = 0;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedPDU(
      const H245_H2250LogicalChannelParameters & param, /// Acknowledgement PDU
      unsigned & errorCode                              /// Error on failure
    ) = 0;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedAckPDU(
      const H245_H2250LogicalChannelAckParameters & param /// Acknowledgement PDU
    ) = 0;

    /**Set the dynamic payload type used by this channel.
      */
    virtual BOOL SetDynamicRTPPayloadType(
      int newType  /// New RTP payload type number
    );

    RTP_DataFrame::PayloadTypes GetDynamicRTPPayloadType() const { return rtpPayloadType; }
  //@}

  protected:
    RTP_DataFrame::PayloadTypes rtpPayloadType;
};


///////////////////////////////////////////////////////////////////////////////

/**This class is for encpsulating the IETF Real Time Protocol interface.
 */
class H323_RTPChannel : public H323_RealTimeChannel
{
  PCLASSINFO(H323_RTPChannel, H323_RealTimeChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323_RTPChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability,  /// Capability channel is using
      Directions direction,               /// Direction of channel
      RTP_Session & rtp                   /// RTP session for channel
    );

    /// Destroy the channel
    ~H323_RTPChannel();
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**Indicate the session number of the channel.
       Return session for channel. This returns the session ID of the
       RTP_Session member variable.
     */
    virtual unsigned GetSessionID() const;
  //@}

  /**@name Overrides from class H323_RealTimeChannel */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_H2250LogicalChannelParameters & param  /// Open PDU to send.
    ) const;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendOpenAck(
      H245_H2250LogicalChannelAckParameters & param /// Acknowledgement PDU
    ) const;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedPDU(
      const H245_H2250LogicalChannelParameters & param, /// Acknowledgement PDU
      unsigned & errorCode                              /// Error on failure
    );

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedAckPDU(
      const H245_H2250LogicalChannelAckParameters & param /// Acknowledgement PDU
    );
  //@}

  protected:
    RTP_Session      & rtpSession;
    H323_RTP_Session & rtpCallbacks;
};


///////////////////////////////////////////////////////////////////////////////

/**This class is for encpsulating the IETF Real Time Protocol interface as used
by a remote host.
 */
class H323_ExternalRTPChannel : public H323_RealTimeChannel
{
  PCLASSINFO(H323_ExternalRTPChannel, H323_RealTimeChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323_ExternalRTPChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability,  /// Capability channel is using
      Directions direction,               /// Direction of channel
      unsigned sessionID,                 /// Session ID for channel
      H323TransportAddress & data,        /// Data address
      H323TransportAddress & control      /// Control address
    );
    /**Create a new channel.
     */
    H323_ExternalRTPChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability,  /// Capability channel is using
      Directions direction,               /// Direction of channel
      unsigned sessionID,                 /// Session ID for channel
      const PIPSocket::Address & ip,      /// IP address of media server
      WORD dataPort                       /// Data port (control is dataPort+1)
    );
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**Indicate the session number of the channel.
       Return session for channel. This returns the session ID of the
       RTP_Session member variable.
     */
    virtual unsigned GetSessionID() const;

    /**Start the channel.
      */
    virtual BOOL Start();

    /**Handle channel data reception.

       This is called by the thread started by the Start() function and is
       typically a loop writing to the codec and reading from the transport
       (eg RTP_session).
      */
    virtual void Receive();

    /**Handle channel data transmission.

       This is called by the thread started by the Start() function and is
       typically a loop reading from the codec and writing to the transport
       (eg an RTP_session).
      */
    virtual void Transmit();
  //@}

  /**@name Overrides from class H323_RealTimeChannel */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_H2250LogicalChannelParameters & param  /// Open PDU to send.
    ) const;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendOpenAck(
      H245_H2250LogicalChannelAckParameters & param /// Acknowledgement PDU
    ) const;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedPDU(
      const H245_H2250LogicalChannelParameters & param, /// Acknowledgement PDU
      unsigned & errorCode                              /// Error on failure
    );

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedAckPDU(
      const H245_H2250LogicalChannelAckParameters & param /// Acknowledgement PDU
    );
  //@}

    const H323TransportAddress & GetRemoteMediaAddress()        const { return remoteMediaAddress; }
    const H323TransportAddress & GetRemoteMediaControlAddress() const { return remoteMediaControlAddress; }

    BOOL GetRemoteAddress(
      PIPSocket::Address & ip,
      WORD & dataPort
    ) const;

  protected:
    unsigned             sessionID;
    H323TransportAddress externalMediaAddress;
    H323TransportAddress externalMediaControlAddress;
    H323TransportAddress remoteMediaAddress;
    H323TransportAddress remoteMediaControlAddress;
};


///////////////////////////////////////////////////////////////////////////////

/**This class describes a data logical channel between the two endpoints.
   They may be created and deleted as required in the H245 protocol.

   An application may create a descendent off this class and override
   functions as required for operating the channel protocol.
 */
class H323DataChannel : public H323UnidirectionalChannel
{
  PCLASSINFO(H323DataChannel, H323UnidirectionalChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323DataChannel(
      H323Connection & connection,        /// Connection to endpoint for channel
      const H323Capability & capability,  /// Capability channel is using
      Directions direction                /// Direction of channel
    );

    /**Destroy the channel.
      */
    ~H323DataChannel();
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**This is called to clean up any threads on connection termination.
     */
    virtual void Close();

    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_OpenLogicalChannel & openPDU  /// Open PDU to send. 
    ) const;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendOpenAck(
      const H245_OpenLogicalChannel & open,   /// Open PDU
      H245_OpenLogicalChannelAck & ack        /// Acknowledgement PDU
    ) const;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default makes sure the parameters are compatible and passes on
       the PDU to the rtp session.
     */
    virtual BOOL OnReceivedPDU(
      const H245_OpenLogicalChannel & pdu,    /// Open PDU
      unsigned & errorCode                    /// Error code on failure
    );

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default makes sure the parameters are compatible and passes on
       the PDU to the rtp session.
     */
    virtual BOOL OnReceivedAckPDU(
      const H245_OpenLogicalChannelAck & pdu /// Acknowledgement PDU
    );
  //@}

  /**@name Operations */
  //@{
    /**Create the OpalListener class to be used.
       This is called on receipt of an OpenLogicalChannel request.

       The default behaviour creates a compatible listener using the
       connections control channel as a basis and returns TRUE if successful.
      */
    virtual BOOL CreateListener();

    /**Create the OpalTransport class to be used.
       This is called on receipt of an OpenLogicalChannelAck response. It
       should not return TRUE unless the transport member variable is set.

       The default behaviour uses the connection signalling channel to create
       the transport and returns TRUE if successful.
      */
    virtual BOOL CreateTransport();
  //@}

  protected:
    OpalListener  * listener;
    OpalTransport * transport;
    BOOL            separateReverseChannel;
};


#endif // __H323_CHANNELS_H


/////////////////////////////////////////////////////////////////////////////
