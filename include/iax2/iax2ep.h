/*
 *
 * Inter Asterisk Exchange 2
 * 
 * Open Phone Abstraction Library (OPAL)
 *
 * Describes the IAX2 extension of the OpalEndpoint class.
 *
 * Copyright (c) 2005 Indranet Technologies Ltd.
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
 * The Initial Developer of the Original Code is Indranet Technologies Ltd.
 *
 * The author of this code is Derek J Smithies
 *
 *  $Log: iax2ep.h,v $
 *  Revision 1.2  2005/08/04 08:14:17  rjongbloed
 *  Fixed Windows build under DevStudio 2003 of IAX2 code
 *
 *  Revision 1.1  2005/07/30 07:01:32  csoutheren
 *  Added implementation of IAX2 (Inter Asterisk Exchange 2) protocol
 *  Thanks to Derek Smithies of Indranet Technologies Ltd. for
 *  writing and contributing this code
 *
 *
 *
 *
 *
 */

#ifndef IAX_ENDPOINT_H
#define IAX_ENDPOINT_H

#include <ptlib.h>

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <opal/endpoint.h>
#include <iax2/iax2con.h>

class Receiver;
class Transmit;

/** A class to take frames from the transmitter, and disperse them to
    the appropriate IAX2Connection class.  This class calls a method in
    the IAX2EndPoint to do the dispersal. */
class IncomingEthernetFrames : public PThread
{
  PCLASSINFO(IncomingEthernetFrames, PThread);
public:

  /**@name Constructors/destructors*/
  //@{
  /**Construct a distributor, to send packets on to the relevant connection */
  IncomingEthernetFrames();
   
  /**Destroy the distributor */
  ~IncomingEthernetFrames() { }

  /**@name general worker methods*/
  //@{
  /*The method which gets everythig to happen */
  virtual void Main();
   
  /**Set the endpoint variable */
  void Assign(IAX2EndPoint *ep);

  /**Activate this thread to process all frames in the lists
   */
  void ProcessList();

  /**Cause this thread to die immediately */
  void Terminate();

  //@}
 protected:
  /**Global variable which holds the application specific data */
  IAX2EndPoint *endpoint;
   
  /**Flag to activate this thread*/
  PSyncPoint activate;

  /**Flag to indicate if this receiver thread should keep listening for network data */
  BOOL           keepGoing;
};


/** A class to manage global variables. There is one Endpoint per application. */
class IAX2EndPoint : public OpalEndPoint
{
  PCLASSINFO(IAX2EndPoint, OpalEndPoint);
 public:
  /**@name Construction */
  
  //@{
  /**Create the endpoint, and define local variables */
  IAX2EndPoint(
    OpalManager & manager
  );
  
  /**Destroy the endpoint, and all associated connections*/
  ~IAX2EndPoint();
  //@}
  
  /**@name connection Connection handling */
  //@{
  /**Handle new incoming connection from listener.

  The default behaviour does nothing.
  */
  virtual BOOL NewIncomingConnection(
    OpalTransport * transport  /// Transport connection came in on
  );
  
  /**Set up a connection to a remote party.
     This is called from the OpalManager::MakeConnection() function once
     it has determined that this is the endpoint for the protocol.
     
     The general form for this party parameter is:
     
       [iax2:]{username@][transport$]address[/extension][+context]
     
     where the various fields will have meanings specific to the endpoint
     type. For example, with H.323 it could be "h323:Fred@site.com" which
     indicates a user Fred at gatekeeper size.com. Whereas for the PSTN
     endpoint it could be "pstn:5551234" which is to call 5551234 on the
     first available PSTN line.
     
     The proto field is optional when passed to a specific endpoint. If it
     is present, however, it must agree with the endpoints protocol name or
     FALSE is returned.
     
     This function usually returns almost immediately with the connection
     continuing to occur in a new background thread.
     
     If FALSE is returned then the connection could not be established. For
     example if a PSTN endpoint is used and the assiciated line is engaged
     then it may return immediately. Returning a non-NULL value does not
     mean that the connection will succeed, only that an attempt is being
     made.
     
     The default behaviour is pure.
  */
  virtual BOOL MakeConnection(
			      OpalCall & call,        /// Owner of connection
			      const PString & party,  /// Remote party to call
			      void * userData = NULL  /// Arbitrary data to pass to connection
			      );
  
  /**Create a connection for the IAX endpoint.
     The default implementation is to create a IAX2Connection.
  */
  virtual IAX2Connection * CreateConnection(
					   OpalCall & call,            /// Owner of connection
					   const PString & token,      /// token used to identify connection
					   void * userData,             /// User data for connection
					   const PString & remoteParty /// Person we are calling.
					   );
  //@}
  
  /**@name worker Methods*/
  //@{
  /**Setup the Endpoint internval variables, which is called at program 
     startup.*/
  BOOL Initialise();

  /**Handle a received IAX frame. This may be a mini frame or full frame */
  virtual void IncomingEthernetFrame (Frame *frame);
  
  /**A simple test to report if the connection associated with this
     frame is still alive. This test is used when transmitting the
     frame. If the connection is gone, don't bother transmitting the
     frame. There are exceptins to this rule, such as when a hangup
     packet is sent (which is after the connections has died. */
  BOOL ConectionForFrameIsAlive(Frame *f);
  
  /**Request a new src call number, one that is different to all other src
     call numbers for this invocation of the program.*/
  int NextSrcCallNumber();
    
  /**Write the token of all connections in the connectionsActive
     structure to the trace file */
  void ReportStoredConnections();

  /**Report the port in use for IAX calls */
  WORD ListenPortNumber()  { return 4569; }
      
  /**Pointer to the transmitter class, which is always valid*/
  Transmit *transmitter;
  
  /**Pointer to the receiver class, which is always valid*/
  Receiver    *receiver;
  
  /**Report the local username*/
  PString GetLocalUserName() { return localUserName; }
  
  /**Report the number used by the computer running this program*/
  PString GetLocalNumber() { return localNumber; }
  
  /**Set the username to some value */
  void SetLocalUserName(PString newValue); 
  
  /**Set the local (on this host) number to some value */
  void SetLocalNumber(PString newValue);
  
  /**Report the password*/
  PString GetPassword() { return password; }
  
  /**Set the password to some value */
  void SetPassword(PString newValue);
    
  /**Return True if a connection (which matches this Frame ) can be
     found. This check is called prior to transmission of this
     frame. */
  BOOL ConnectionForFrameIsAlive(Frame *f);
  
  /**Get out sequence number to use on status query frames*/
  PINDEX GetOutSequenceNumberForStatusQuery();
  
  /**We have an incoming call. Do we accept ? */
  void StartRinging(PString remoteCaller);
  
  /**Call back for when a connection has closed */
  void OnConnectionClose(IAX2Connection & con);

    /**Handle new incoming connection from listener.
       
    A return value of TRUE indicates that the transport object should be
    deleted by the caller. FALSE indicates that something else (eg the
    connection) has taken over responsibility for deleting the transport.
    
    Well, that is true of Opal. In iax2, we do all the work of creating a new connection etc.
    The transport arguement is ignore. In Iax2, this method is void, as no value is returned.
    Further, in iax2, we process the incoming frame.
    */
  void NewIncomingConnection(
			     Frame *f  /// Frame carrying the new request.
			     );


  /**Call back for when a connections is established (we have received
     the first media packet) */
  void OnEstablished(OpalConnection & con);

   /**Get the data formats this endpoint is capable of operating.
       This provides a list of media data format names that may be used by an
       OpalMediaStream may be created by a connection from this endpoint.

       Note that a specific connection may not actually support all of the
       media formats returned here, but should return no more.

       The default behaviour is pure.
      */
    virtual OpalMediaFormatList GetMediaFormats() const;

  /**Return the bitmask which specifies the possible codecs we support */
  PINDEX GetSupportedCodecs(OpalMediaFormatList & list);
  
  /**Return the bitmask which specifies the preferred codec */
  PINDEX GetPreferredCodec(OpalMediaFormatList & list);

  /**Get the frame size (bytes) and frame duration (ms) for compressed
     data from this codec */
  void GetCodecLengths(PINDEX src, PINDEX &compressedBytes, PINDEX &duration);
  
  /**enum of the components from a remote party address string 
     These fields are from the address,
  
      [iax2:]{username@][transport$]address[/extension][+context]
  */
  enum RemoteAddressFields {
    protoIndex     = 0,     /*!< the protocol, or iax2: field            */
    userIndex      = 1,     /*!< the username, or alias field            */
    transportIndex = 2,     /*!< the transport, or transport field       */
    addressIndex   = 3,     /*!< the address, or 192.168.1.1 field       */
    extensionIndex = 4,     /*!< the extension, or "extension"" field    */
    contextIndex   = 5,     /*!< the context,   or "+context" field      */
    maximumIndex   = 6      /*!< the number of possible fields           */
  };

  /**Given a remote party name of the format:

     [proto:][alias@][transport$]address[/extension]

     pull the string apart and get the components. The compoents are stored
     in a PStringList, indexed by the enum RemoteAddressFields */
  static PStringList DissectRemoteParty(const PString & other);

  /**Pull frames off the incoming list, and pass on to the relevant
     connection. If no matching connection found, delete the frame.
     Repeat the process until no frames are left. */
  void ProcessReceivedEthernetFrames();


  /**Report on the frames in the current transmitter class, which are
     pending transmission*/
  void ReportTransmitterLists();

  /**Copy to the supplied OpalMediaList the media formats we support*/
  void CopyLocalMediaFormats(OpalMediaFormatList & list);
    
  //@}
  
 protected:
  /**Thread which transfers frames from the Receiver to the
     appropriate connection.  It momentarily locks the connection
     list, searches through, and then completes the trasnsfer. If need
     be, this thread will create a new conneciton (to cope with a new
     incoming call) and add the new connections to the internal
     list. */
  IncomingEthernetFrames incomingFrameHandler;

  /**List of iax packets which has been read from the ethernet, and
     is to be sent to the matching IAX2Connection */
  FrameList   packetsReadFromEthernet;
  
  /**The socket on which all data is sent/received.*/
  PUDPSocket  *sock;
  
  /**Number of active calls */
  int callnumbs;
  
  /** lock on access to call numbers variable */
  PMutex callNumbLock;
  
  /**Time when a call was started */
  PTime callStartTime;
  
  /**Name of this user, which is used as the IeCallingNumber */
  PString localUserName;
  
  /**Number, as used by the computer on the host running this program*/
  PString localNumber;
  
  /**Password for this user, which is used when processing an authentication request */
  PString password;
  
  /**Counter to use for sending on status query frames */
  PINDEX statusQueryCounter;
  
  /**Mutex for the statusQueryCounter */
  PMutex statusQueryMutex;
  
  /**Pointer to the Processor class which handles special packets (eg lagrq) that have no 
     destination call to handle them. */
  IAX2Processor * specialPacketHandler;
    
  /**For the supplied Frame, pass it to a connection in the connectionsActive structure.
     If no matching connection is found, return FALSE;
     
     If a matching connections is found, give the frame to the
     connection (for the connection to process) and return TRUE;
  */
  BOOL ProcessInMatchingConnection(Frame *f);  
  
  /**The TokenTranslationDict may need a new entry. Examine
     the list of active connections, to see if any match this frame.
     If any do, then we add a new translation entry in tokenTable;
     
     If a matching connection is found in connectionsActive, create a
     new translation entry and return TRUE. The connection, after
     processing the frame, will then delete the frame. */
  BOOL AddNewTranslationEntry(Frame *f);
  
  /**tokenTable is a hack to allow IAX2 to fit in with one of the
     demands of the opal library. 
     
     Opal demands that at connection setup, we know the unique ID which 
     this call will use. 

     Since the unique ID is remote ip adress + remote's Source Call
     number, this is unknown if we are initiating the
     call. Consequently, this table is needed, as it provides a
     translation between the initial (or psuedo) token and the
     token that is later adopted */

  PStringToString    tokenTable;
  
  /**Threading mutex on the variable tokenTable   */
  PMutex             mutexTokenTable;

  /**Thread safe counter which keeps track of the calls created by this endpoint.
     This value is used when giving outgoing calls a unique ID */
  PAtomicInteger callsEstablished;

  /**Local copy of the media types we can handle*/
  OpalMediaFormatList localMediaFormats;
};

#endif // IAX_ENDPOINT_H
/* The comment below is magic for those who use emacs to edit this file. */
/* With the comment below, the tab key does auto indent to 4 spaces.     */

/*
 * Local Variables:
 * mode:c
 * c-file-style:linux
 * c-basic-offset:2
 * End:
 */

