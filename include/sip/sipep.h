/*
 * sipep.h
 *
 * Session Initiation Protocol endpoint.
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
 * $Log: sipep.h,v $
 * Revision 1.2002  2002/02/01 04:53:01  robertj
 * Added (very primitive!) SIP support.
 *
 */

#ifndef __OPAL_SIPEP_H
#define __OPAL_SIPEP_H

#ifdef __GNUC__
#pragma interface
#endif


#include <opal/endpoint.h>


class SIPConnection;
class SIPURL;
class SIP_PDU;
class SIPMIMEInfo;


/////////////////////////////////////////////////////////////////////////

/**Session Initiation Protocol endpoint.
 */
class SIPEndPoint : public OpalEndPoint
{
  PCLASSINFO(SIPEndPoint, OpalEndPoint);

  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint.
     */
    SIPEndPoint(
      OpalManager & manager
    );

    /**Destroy endpoint.
     */
    ~SIPEndPoint();
  //@}

  /**@name Overrides from OpalManager */
  //@{
    /**Handle new incoming connection from listener.

       The default behaviour does nothing.
      */
    virtual void NewIncomingConnection(
      OpalTransport * transport  /// Transport connection came in on
    );

    /**Set up a connection to a remote party.
       This is called from the OpalManager::SetUpConnection() function once
       it has determined that this is the endpoint for the protocol.

       The general form for this party parameter is:

            [proto:][alias@][transport$]address[:port]

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
    virtual BOOL SetUpConnection(
      OpalCall & call,        /// Owner of connection
      const PString & party,  /// Remote party to call
      void * userData = NULL  /// Arbitrary data to pass to connection
    );
  //@}

  /**@name Customisation call backs */
  //@{
    /**Create a connection for the SIP endpoint.
       The default implementation is to create a OpalSIPConnection.
      */
    virtual SIPConnection * CreateConnection(
      OpalCall & call,        /// Owner of connection
      const PString & token,  /// token used to identify connection
      SIP_PDU * invite,       /// Original INVITE pdu
      void * userData = NULL
    );
  //@}
  
  /**@name Protocol handling routines */
  //@{
    virtual void HandlePDU(
      OpalTransport & transport
    );

    /**Handle an incoming SIP PDU that has been full decoded
      */
    virtual BOOL OnReceivedPDU(
      SIP_PDU & pdu
    );

    /**Handle an incoming INVITE request
      */
    virtual BOOL OnReceivedINVITE(
      SIP_PDU & pdu
    );
  //@}
 
  
    SIPConnection * GetSIPConnectionWithLock(const PString & str);
    SIPConnection * GetSIPConnectionWithLock(const SIPMIMEInfo & mime);
    void AddNewConnection(SIPConnection * conn);

    virtual BOOL IsAcceptedAddress(const SIPURL & toAddr);

    void  SetMIMEForm(BOOL v) { mimeForm = v; }
    BOOL  GetMIMEForm() const { return mimeForm; }

    BOOL Register(
      const PString & server
    );

    unsigned GetLastSentCSeq() const { return lastSentCSeq; }

    const PString & GetRegistrationID() const { return registrationID; }
    const PString & GetRegistrationName() const { return registrationName; }
    void SetRegistrationName(
      const PString & name
    ) { registrationName = name; }

    const PString & GetRegistrationPassword() const { return registrationPassword; }
    void SetRegistrationPassword(
      const PString & name
    ) { registrationPassword = name; }

  protected:
    BOOL     mimeForm;

    PString  registrationID;
    PString  registrationName;
    PString  registrationPassword;
    unsigned lastSentCSeq;
};

#endif // __OPAL_SIPEP_H


// End of File ///////////////////////////////////////////////////////////////
