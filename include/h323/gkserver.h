/*
 * gkserver.h
 *
 * H225 Registration Admission and Security protocol handler
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
 * This code was based on original code from OpenGate of Egoboo Ltd. thanks
 * to Ashley Unitt for his efforts.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: gkserver.h,v $
 * Revision 1.2002  2001/08/13 05:10:39  robertj
 * Updates from OpenH323 v1.6.0 release.
 *
 * Revision 2.0  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.4  2001/08/10 11:03:49  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.3  2001/08/06 07:44:52  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.2  2001/08/06 03:18:35  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 * Improved access to H.235 secure RAS functionality.
 * Changes to H.323 secure RAS contexts to help use with gk server.
 *
 * Revision 1.1  2001/07/24 02:30:55  robertj
 * Added gatekeeper RAS protocol server classes.
 *
 */

#ifndef __H323_GKSERVER_H
#define __H323_GKSERVER_H

#ifdef __GNUC__
#pragma interface
#endif


#include <opal/guid.h>
#include <h323/h225ras.h>
#include <h323/transaddr.h>
#include <h323/h235auth.h>


class PASN_Sequence;
class PASN_Choice;

class H225_AliasAddress;
class H225_EndpointIdentifier;
class H225_GatekeeperIdentifier;
class H225_ArrayOf_TransportAddress;
class H225_GatekeeperIdentifier;
class H225_EndpointIdentifier;

class H323RegisteredEndPoint;
class H323GatekeeperServer;
class H323RasPDU;


/**This class describes an active call on a gatekeeper.
  */
class H323GatekeeperCall : public PObject
{
    PCLASSINFO(H323GatekeeperCall, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper call tracking record.
     */
    H323GatekeeperCall(
      const OpalGloballyUniqueID & callIdentifier, /// Unique call identifier
      H323RegisteredEndPoint * endpoint = NULL     /// Local endpoint
    );

    /**Destroy the call, removing itself from the endpoint.
      */
    ~H323GatekeeperCall();
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Compare two objects.
      */
    Comparison Compare(
      const PObject & obj  /// Other object
    ) const;

  /**@name Operations */
  //@{
    /**Handle an admission ARQ PDU.
       The default behaviour sets some internal variables from the ARQ data
       and then returns TRUE.
      */
    virtual BOOL OnAdmission(
      const H323GatekeeperServer & server,
      const H225_AdmissionRequest & arq,
      H225_AdmissionConfirm & acf,
      H225_AdmissionReject & arj
    );

    /**Handle a disengage DRQ PDU.
       The default behaviour simply returns TRUE.
      */
    virtual BOOL OnDisengage(
      const H323GatekeeperServer & server,
      const H225_DisengageRequest & drq,
      H225_DisengageConfirm & dcf,
      H225_DisengageReject & drj
    );
  //@}

    /**Print the name of the gatekeeper.
      */
    void PrintOn(
      ostream & strm    /// Stream to print to.
    ) const;
  //@}

  /**@name Access functions */
  //@{
    const H323RegisteredEndPoint & GetEndPoint() const { return *endpoint; }
    unsigned GetCallReference() const { return callReference; }
    const OpalGloballyUniqueID & GetCallIdentifier() const { return callIdentifier; }
    const OpalGloballyUniqueID & GetConferenceIdentifier() const { return conferenceIdentifier; }
    unsigned GetBandwidthUsed() const { return bandwidthUsed; }
    void SetBandwidthUsed(unsigned bandwidth) { bandwidthUsed = bandwidth; }
  //@}

  protected:
    H323RegisteredEndPoint * endpoint;
    unsigned                 callReference;
    OpalGloballyUniqueID     callIdentifier;
    OpalGloballyUniqueID     conferenceIdentifier;
    H323TransportAddress     srcAddress;
    H323TransportAddress     dstAddress;
    unsigned                 bandwidthUsed;
};


PSORTED_LIST(H323GatekeeperCallList, H323GatekeeperCall);


/**This class describes endpoints that are registered with a gatekeeper server.
   Note that a registered endpoint has no realationship in this software to a
   H323EndPoint class. This is purely a description of endpoints that are
   registered with the gatekeeper.
  */
class H323RegisteredEndPoint : public PObject
{
    PCLASSINFO(H323RegisteredEndPoint, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint registration record.
     */
    H323RegisteredEndPoint(
      const PString & id  /// Identifier
    );
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Compare two objects.
      */
    Comparison Compare(
      const PObject & obj  /// Other object
    ) const;

    /**Print the name of the gatekeeper.
      */
    void PrintOn(
      ostream & strm    /// Stream to print to.
    ) const;
  //@}

  /**@name Call Operations */
  //@{
    /**Add a call to the endpoints list of active calls.
       This is largely an internal routine, it is not expected the user would
       need to deal with this function.
      */
    void AddCall(
      H323GatekeeperCall * call
    ) { activeCalls.Append(call); }

    /**Remove a call from the endpoints list of active calls.
       This is largely an internal routine, it is not expected the user would
       need to deal with this function.
      */
    void RemoveCall(
      H323GatekeeperCall * call
    ) { activeCalls.Remove(call); }

    /**Get the count of active calls on this endpoint.
      */
    PINDEX GetCallCount() const { return activeCalls.GetSize(); }

    /**Get the details of teh active call on this endpoint.
      */
    H323GatekeeperCall & GetCall(
      PINDEX idx
    ) { return activeCalls[idx]; }
  //@}

  /**@name Protocol Operations */
  //@{
    /**Call back on receiving a RAS registration for this endpoint.
       The default behaviour extract information from the RRQ and sets
       internal variables to that data.

       If returns TRUE then a RCF is sent otherwise an RRJ is sent.
      */
    virtual BOOL OnRegistration(
      const H323GatekeeperServer & server,    /// Gatekeeper Data
      const H225_RegistrationRequest & rrq,
      H225_RegistrationConfirm & rcf,
      H225_RegistrationReject & rrj
    );

    /**Determine of this endpoint has exceeded its time to live.
      */
    BOOL HasExceededTimeToLive() const;
  //@}

  /**@name Access functions */
  //@{
    /**Get the endpoint identifier assigned to the endpoint.
      */
    const PString & GetIdentifier() const { return identifier; }

    /**Get the number of addresses that can be used to contact this
       endpoint via the RAS protocol.
      */
    PINDEX GetRASAddressCount() const { return rasAddresses.GetSize(); }

    /**Get an address that can be used to contact this endpoint via the RAS
       protocol.
      */
    OpalTransportAddress GetRASAddress(
      PINDEX idx
    ) const { return rasAddresses[idx]; }

    /**Get the number of addresses that can be used to contact this
       endpoint via the H.225/Q.931 protocol, ie normal calls.
      */
    PINDEX GetSignalAddressCount() const { return signalAddresses.GetSize(); }

    /**Get an address that can be used to contact this endpoint via the
       H.225/Q.931 protocol, ie normal calls.
      */
    OpalTransportAddress GetSignalAddress(
      PINDEX idx
    ) const { return signalAddresses[idx]; }

    /**Get the number of aliases this endpoint may be identified by.
      */
    PINDEX GetAliasCount() const { return aliases.GetSize(); }

    /**Get an alias that this endpoint may be identified by.
      */
    PString GetAlias(
      PINDEX idx
    ) const { return aliases[idx]; }

    /**Remove an alias that this endpoint may be identified by.
      */
    void RemoveAlias(
      const PString & alias
    ) { aliases.RemoveAt(aliases.GetValuesIndex(alias)); }

    /**Get the security context for this RAS connection.
      */
    virtual const H235Authenticators & GetAuthenticators() const { return authenticators; }
  //@}

  protected:
    PString                   identifier;
    OpalTransportAddressArray rasAddresses;
    OpalTransportAddressArray signalAddresses;
    PStringArray              aliases;
    unsigned                  timeToLive;
    H235Authenticators        authenticators;

    PTime                     lastRegistration;

    H323GatekeeperCallList    activeCalls;
};


/**This class embodies the low level H.225.0 RAS protocol on gatekeepers.
   One or more instances of this class may be used to access a single
   H323GatekeeperServer instance. Thus specific interfaces could be set up
   to receive UDP packets, all operating as the same gatekeeper.
  */
class H323GatekeeperListener : public H225_RAS
{
    PCLASSINFO(H323GatekeeperListener, H225_RAS);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper listener.
     */
    H323GatekeeperListener(
      H323EndPoint & endpoint,               /// Local endpoint
      H323GatekeeperServer & server,         /// Database for gatekeeper
      const PString & gatekeeperIdentifier,  /// Name of this gatekeeper
      OpalTransport * transport = NULL       /// Transport over which gatekeepers communicates.
    );

    /**Destroy gatekeeper listener.
     */
    ~H323GatekeeperListener();
  //@}

  /**@name Operation callbacks */
  //@{
    /**Handle a discovery GRQ PDU.
       The default behaviour does some checks and returns TRUE if suitable.
      */
    virtual BOOL OnDiscovery(
      const H225_GatekeeperRequest & grq,
      H225_GatekeeperConfirm & gcf,
      H225_GatekeeperReject & grj
    );

    /**Handle a registration RRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual BOOL OnRegistration(
      const H225_RegistrationRequest & rrq,
      H225_RegistrationConfirm & rcf,
      H225_RegistrationReject & rrj
    );

    /**Handle an unregistration URQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual BOOL OnUnregistration(
      const H225_UnregistrationRequest & rrq,
      H225_UnregistrationConfirm & rcf,
      H225_UnregistrationReject & rrj
    );

    /**Handle an admission ARQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual BOOL OnAdmission(
      const H225_AdmissionRequest & arq,
      H225_AdmissionConfirm & acf,
      H225_AdmissionReject & arj
    );

    /**Handle a disengage DRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual BOOL OnDisengage(
      const H225_DisengageRequest & drq,
      H225_DisengageConfirm & dcf,
      H225_DisengageReject & drj
    );

    /**Handle a bandwidth BRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual BOOL OnBandwidth(
      const H225_BandwidthRequest & brq,
      H225_BandwidthConfirm & bcf,
      H225_BandwidthReject & brj
    );

    /**Handle a location LRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual BOOL OnLocation(
      const H225_LocationRequest & lrq,
      H225_LocationConfirm & lcf,
      H225_LocationReject & lrj
    );
  //@}

  /**@name Low level protocol callbacks */
  //@{
    virtual BOOL HandleRasPDU(const H323RasPDU & response);
    virtual BOOL OnReceiveGatekeeperRequest(const H225_GatekeeperRequest &);
    virtual BOOL OnReceiveRegistrationRequest(const H225_RegistrationRequest &);
    virtual BOOL OnReceiveUnregistrationRequest(const H225_UnregistrationRequest &);
    virtual BOOL OnReceiveUnregistrationConfirm(const H225_UnregistrationConfirm &);
    virtual BOOL OnReceiveUnregistrationReject(const H225_UnregistrationReject &);
    virtual BOOL OnReceiveAdmissionRequest(const H225_AdmissionRequest &);
    virtual BOOL OnReceiveBandwidthRequest(const H225_BandwidthRequest &);
    virtual BOOL OnReceiveBandwidthConfirm(const H225_BandwidthConfirm &);
    virtual BOOL OnReceiveBandwidthReject(const H225_BandwidthReject &);
    virtual BOOL OnReceiveDisengageRequest(const H225_DisengageRequest &);
    virtual BOOL OnReceiveDisengageConfirm(const H225_DisengageConfirm &);
    virtual BOOL OnReceiveDisengageReject(const H225_DisengageReject &);
    virtual BOOL OnReceiveLocationRequest(const H225_LocationRequest &);
    virtual BOOL OnReceiveInfoRequestAck(const H225_InfoRequestAck &);
    virtual BOOL OnReceiveInfoRequestNak(const H225_InfoRequestNak &);
    virtual BOOL OnReceiveInfoRequestResponse(const H225_InfoRequestResponse &);
    virtual BOOL OnReceiveResourcesAvailableConfirm(const H225_ResourcesAvailableConfirm &);

    /**Get the security context for this RAS connection.
      */
    virtual H235Authenticators GetAuthenticators() const;
  //@}

  protected:
    BOOL CheckGatekeeperIdentifier(
      unsigned optionalField,
      const PASN_Sequence & pdu,
      const H225_GatekeeperIdentifier & identifier
    );
    BOOL GetRegisteredEndPoint(
      unsigned rejectTag,
      unsigned securityRejectTag,
      PASN_Choice & reject,
      unsigned securityField,
      const PASN_Sequence & pdu,
      const H225_ArrayOf_CryptoH323Token & cryptoTokens,
      const H225_EndpointIdentifier & identifier
    );

    H323GatekeeperServer   & gatekeeper;
    H323RegisteredEndPoint * currentEP;
};


/**This class implements a basic gatekeeper server functionality.
   An instance of this class contains all of the state information and
   operations for a gatekeeper. Multiple gatekeeper listeners may be using
   thsi class to link individual UDP (or other protocol) packets from various
   sources (interfaces etc) into a single instance.

   There is typically only one instance of this class, though it is not
   limited to that. An application would also quite likely descend from this
   class and override call back functions to implement more complex policy.
  */
class H323GatekeeperServer : public PObject
{
    PCLASSINFO(H323GatekeeperServer, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper.
     */
    H323GatekeeperServer(
      H323EndPoint & endpoint
    );
  //@}

  /**@name Protocol Handler Operations */
  //@{
    /**Add a gatekeeper listener to this gatekeeper server given the
       transport address for the local interface.
      */
    BOOL AddListener(
      const OpalTransportAddress & interfaceName
    );

    /**Add a gatekeeper listener to this gatekeeper server given the transport.
       Note that the transport is then owned by the listener and will be
       deleted automatically when the listener is destroyed. Note also the
       transport is deleted if this function returns FALSE and no listener was
       created.
      */
    BOOL AddListener(
      OpalTransport * transport
    );

    /**Add a gatekeeper listener to this gatekeeper server.
       Note that the gatekeeper listener is then owned by the gatekeeper
       server and will be deleted automatically when the listener is removed.
       Note also the listener is deleted if this function returns FALSE and
       the listener was not used.
      */
    BOOL AddListener(
      H323GatekeeperListener * listener
    );

    /**Create a new H323Gatlistner.
       The user woiuld not usually use this function as it is used internally
       by the server when new listeners are added by OpalTransportAddress.

       However, a user may override this function to create objects that are
       user defined descendants of H323GatekeeperListener so the user can
       maintain extra information on a interface by interface basis.
      */
    virtual H323GatekeeperListener * CreateListener(
      OpalTransport * transport  // Transport for listener
    );

    /**Remove a gatekeeper listener from this gatekeeper server.
       The gatekeeper listener is automatically deleted.
      */
    BOOL RemoveListener(
      H323GatekeeperListener * listener
    );
  //@}

  /**@name EndPoint Operations */
  //@{
    /**Call back on receiving a RAS registration for this endpoint.
       The default behaviour checks if the registered endpoint already exists
       and if not creates a new endpoint. It then calls the OnRegistration()
       on that new endpoint instance.

       If returns TRUE then a RCF is sent otherwise an RRJ is sent.
      */
    virtual BOOL OnRegistration(
      H323RegisteredEndPoint * & ep,
      const H225_RegistrationRequest & rrq,
      H225_RegistrationConfirm & rcf,
      H225_RegistrationReject & rrj
    );

    /**Handle an unregistration URQ PDU.
       The default behaviour removes the aliases defined in the URQ and if all
       aliases for the registered endpoint are removed then the endpoint itself
       is removed.
      */
    virtual BOOL OnUnregistration(
      H323RegisteredEndPoint * ep,
      const H225_UnregistrationRequest & urq,
      H225_UnregistrationConfirm & ucf,
      H225_UnregistrationReject & urj
    );

    /**Add a new registered endpoint to the server database.
       Once the endpoint has been added it is then owned by the server and
       will be deleted when it is removed.

       The user woiuld not usually use this function as it is used internally
       by the server when new registration requests (RRQ) are received.

       Note that a registered endpoint has no realationship in this software
       to a H323EndPoint class.
      */
    virtual void AddEndPoint(
      H323RegisteredEndPoint * ep
    );

    /**Remove a registered endpoint from the server database.
      */
    virtual void RemoveEndPoint(
      H323RegisteredEndPoint * ep,
      BOOL autoDelete = TRUE,
      BOOL lock = TRUE
    );

    /**Remove an alias from the server database.
       A registered endpoint is searched for and the alias removed from that
       endpoint. If there are no more aliases left for the endpoint then the
       registered endpoint itself is removed from the database.
      */
    virtual void RemoveAlias(
      const PString & alias,
      BOOL autoDelete = TRUE
    );

    /**Create a new registered endpoint object.
       The user woiuld not usually use this function as it is used internally
       by the server when new registration requests (RRQ) are received.

       However, a user may override this function to create objects that are
       user defined descendants of H323RegisteredEndPoint so the user can
       maintain extra information on a endpoint by endpoint basis.
      */
    virtual H323RegisteredEndPoint * CreateRegisteredEndPoint(
      const H225_RegistrationRequest & rrq
    );

    /**Create a new unique identifier for the registered endpoint.
       The returned identifier must be unique over the lifetime of this
       gatekeeper server.

       The default behaviour simply returns the string representation of the
       member variable nextIdentifier. There could be a problem in this
       implementation after 4,294,967,296 have been registered.
      */
    virtual PString CreateEndPointIdentifier();

    /**Find a registered alias given its endpoint identifier.
      */
    virtual H323RegisteredEndPoint * FindEndPointByIdentifier(
      const PString & identifier
    );

    /**Find a registered alias given a list of signal addresses.
      */
    virtual H323RegisteredEndPoint * FindEndPointBySignalAddresses(
      const H225_ArrayOf_TransportAddress & addresses
    );

    /**Find a registered alias given its signal address.
      */
    virtual H323RegisteredEndPoint * FindEndPointBySignalAddress(
      const H323TransportAddress & address
    );

    /**Find a registered alias given its raw alias address.
      */
    virtual H323RegisteredEndPoint * FindEndPointByAliasAddress(
      const H225_AliasAddress & alias
    );

    /**Find a registered alias given its simple alias string.
      */
    virtual H323RegisteredEndPoint * FindEndPointByAliasString(
      const PString & alias
    );

    /**Get the first registered endpoint in the gatekeeper server database.
       Note that the database is locked and no access to endpoints may be made
       until all registered endpoints are enumerated (NextEndPoint() returns
       NULL) or AbortEnumeration() is called.
      */
    H323RegisteredEndPoint * FirstEndPoint();

    /**Get the next registered endpoint in the gatekeeper server database.
       Note that the database is locked and no access to endpoints may be made
       until all registered endpoints are enumerated (NextEndPoint() returns
       NULL) or AbortEnumeration() is called.
      */
    H323RegisteredEndPoint * NextEndPoint();

    /**Abort the enumeration process, unlocking the gatekeeper server database.
      */
    void AbortEnumeration();

    /**Process all registered endpoints and remove them if they are too old.
      */
    void AgeEndPoints();
  //@}

  /**@name Call Operations */
  //@{
    /**Handle an admission ARQ PDU.
       The default behaviour verifies that the call is allowed by the policies
       the gatekeeper server requires, then attempts to look up the required
       signal address for the call. It also manages bandwidth allocations.
      */
    virtual BOOL OnAdmission(
      H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      H225_AdmissionConfirm & acf,
      H225_AdmissionReject & arj
    );

    /**Handle a disengage DRQ PDU.
       The default behaviour finds the call by its id provided in the DRQ and
       removes it from the gatekeeper server database.
      */
    virtual BOOL OnDisengage(
      H323RegisteredEndPoint & ep,
      const H225_DisengageRequest & drq,
      H225_DisengageConfirm & dcf,
      H225_DisengageReject & drj
    );

    /**Handle a bandwidth BRQ PDU.
       The default behaviour adjusts the bandwidth used by the gatekeeper and
       adjusts the remote endpoit according to those limits.
      */
    virtual BOOL OnBandwidth(
      H323RegisteredEndPoint & ep,
      const H225_BandwidthRequest & brq,
      H225_BandwidthConfirm & bcf,
      H225_BandwidthReject & brj
    );

    /**Add a new call to the server database.
       Once the call has been added it is then owned by the server and
       will be deleted when it is removed.

       The user woiuld not usually use this function as it is used internally
       by the server when new registration requests (RRQ) are received.
      */
    virtual void AddCall(
      H323GatekeeperCall * call
    );

    /**Remove a call from the server database.
      */
    virtual void RemoveCall(
      H323GatekeeperCall * call
    );

    /**Create a new call object.
       The user woiuld not usually use this function as it is used internally
       by the server when new calls (ARQ) are made.

       However, a user may override this function to create objects that are
       user defined descendants of H323GatekeeperCall so the user can
       maintain extra information on a call by call basis.
      */
    virtual H323GatekeeperCall * CreateCall(
      H323RegisteredEndPoint & endpoint,         /// Local endpoint
      const OpalGloballyUniqueID & callIdentifier
    );

    /**Find the call given the identifier.
      */
    virtual H323GatekeeperCall * FindCall(
      const OpalGloballyUniqueID & callIdentifier
    );
  //@}

  /**@name Routing operations */
  //@{
    /**Handle a location LRQ PDU.
       The default behaviour just uses TranslateAliasAddressToSignalAddress to
       determine the endpoints location.

       It is expected that a user would override this function to implement
       application specified look up algorithms.
      */
    virtual BOOL OnLocation(
      const H225_LocationRequest & lrq,
      H225_LocationConfirm & lcf,
      H225_LocationReject & lrj
    );

    /**Translate a given alias to a signal address.
       This is called by the OnAdmission() handler to fill in the ACF
       informing the calling endpoint where to actually connect to.

       It is expected that a user would override this function to implement
       application specified look up algorithms.

       The default behaviour checks the isGatekeeperRouted and if TRUE simply
       returns the gatekeepers associated endpoints (not registered endpoint,
       but real H323EndPoint) listening address.

       If isGatekeeperRouted is FALSE then it looks up the registered
       endpoints by alias and uses the saved signal address in the database.

       If the alias is not registered then the address parameter is not
       changed and the function returns TRUE if it is a valid address, FALSE
       if it was empty.
      */
    virtual BOOL TranslateAliasAddressToSignalAddress(
      const H225_AliasAddress & alias,
      H323TransportAddress & address
    );
  //@}

  /**@name Policy operations */
  //@{
    /**Check the signal address against the security policy.
       This validates that the specified endpoint is allowed to make a
       connection to or from the specified signal address.

       It is expected that a user would override this function to implement
       application specified security policy algorithms.

       The default behaviour simply returns TRUE.
      */
    virtual BOOL CheckSignalAddressPolicy(
      const H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      const OpalTransportAddress & address
    );

    /**Check the alias address against the security policy.
       This validates that the specified endpoint is allowed to make a
       connection to or from the specified alias address.

       It is expected that a user would override this function to implement
       application specified security policy algorithms.

       The default behaviour checks the canOnlyAnswerRegisteredEP or
       canOnlyCallRegisteredEP meber variables depending on if it is an
       incoming call and if that is TRUE only allows the call to proceed
       if the alias is also registered with the gatekeeper.
      */
    virtual BOOL CheckAliasAddressPolicy(
      const H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      const H225_AliasAddress & alias
    );

    /**Check the alias address against the security policy.
       This validates that the specified endpoint is allowed to make a
       connection to or from the specified simple alias string.

       It is expected that a user would override this function to implement
       application specified security policy algorithms.

       The default behaviour checks the canOnlyAnswerRegisteredEP or
       canOnlyCallRegisteredEP meber variables depending on if it is an
       incoming call and if that is TRUE only allows the call to proceed
       if the alias is also registered with the gatekeeper.
      */
    virtual BOOL CheckAliasStringPolicy(
      const H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      const PString & alias
    );

    /**Get password for user if H.235 security active.
       Returns FALSE if security active and user not found. Returns TRUE if
       security active and user is found. Also returns TRUE if security
       inactive, and the password is always the empty string.
      */
    virtual BOOL GetUsersPassword(
      const PString & alias,
      PString & password
    ) const;

    /**Allocate or change the bandwidth being used.
       This function modifies the total bandwidth used by the all endpoints
       registered with this gatekeeper. It is called when ARQ or BRQ PDU's are
       received.
      */
    virtual unsigned AllocateBandwidth(
      unsigned newBandwidth,
      unsigned oldBandwidth = 0
    );
  //@}

  /**@name Access functions */
  //@{
    /**Get the identifier name for this gatekeeper.
      */
    const PString & GetGatekeeperIdentifier() const { return gatekeeperIdentifier; }

    /**Set the identifier name for this gatekeeper.
       If adjustListeners is TRUE then all gatekeeper listeners that are
       attached to this gatekeeper server have their identifier names changed
       as well.
      */
    void SetGatekeeperIdentifier(
      const PString & id,
      BOOL adjustListeners = TRUE
    );

    /**Get the total bandwidth available in 100's of bits per second.
      */
    unsigned GetAvailableBandwidth() const { return availableBandwidth; }

    /**Set the total bandwidth available in 100's of bits per second.
      */
    void SetAvailableBandwidth(unsigned bps100) { availableBandwidth = bps100; }

    /**Get the default time to live for new registered endpoints.
      */
    unsigned GetTimeToLive() const { return defaultTimeToLive; }

    /**Set the default time to live for new registered endpoints.
      */
    void SetTimeToLive(unsigned seconds) { defaultTimeToLive = seconds; }
  //@}

  protected:
    H323EndPoint & endpoint;

    // Configuration & policy variables
    PString  gatekeeperIdentifier;

    unsigned availableBandwidth;
    unsigned defaultBandwidth;
    unsigned maximumBandwidth;
    unsigned defaultTimeToLive;
    BOOL     overwriteOnSameSignalAddress;
    BOOL     canOnlyCallRegisteredEP;
    BOOL     canOnlyAnswerRegisteredEP;
    BOOL     answerCallPreGrantedARQ;
    BOOL     makeCallPreGrantedARQ;
    BOOL     isGatekeeperRouted;
    BOOL     aliasCanBeHostName;
    BOOL     usingH235;

    PStringToString passwords;

    // Dynamic variables
    PMutex mutex;

    PLIST(ListenerList, H323GatekeeperListener);
    ListenerList listeners;

    time_t   identifierBase;
    unsigned nextIdentifier;

    PSORTED_LIST(IdList, H323RegisteredEndPoint);
    IdList byIdentifier;

    PDICTIONARY(AddressDict, PString, H323RegisteredEndPoint);
    AddressDict byAddress;

    PDICTIONARY(AliasDict, PString, H323RegisteredEndPoint);
    AliasDict byAlias;

    H323GatekeeperCallList activeCalls;

    PINDEX enumerationIndex;
};


#endif // __H323_GKSERVER_H


/////////////////////////////////////////////////////////////////////////////
