/*
 * q931.h
 *
 * Q.931 protocol handler
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
 * Contributor(s): ______________________________________.
 *
 * $Log: q931.h,v $
 * Revision 1.2002  2001/08/13 05:10:39  robertj
 * Updates from OpenH323 v1.6.0 release.
 *
 * Revision 2.0  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.32  2001/08/03 14:12:07  robertj
 * Fixed value for Call State Information Element
 *
 * Revision 1.31  2001/07/24 23:40:15  craigs
 * Added ability to remove Q931 IE
 *
 * Revision 1.30  2001/06/14 06:25:13  robertj
 * Added further H.225 PDU build functions.
 * Moved some functionality from connection to PDU class.
 *
 * Revision 1.29  2001/05/30 04:38:38  robertj
 * Added BuildStatusEnquiry() Q.931 function, thanks Markus Storm
 *
 * Revision 1.28  2001/04/11 00:12:38  robertj
 * Added some enums for numbering plans and call types, thanks Markus Storm.
 *
 * Revision 1.27  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.26  2001/01/19 07:01:42  robertj
 * Added all of the Q.931 message type codes.
 *
 * Revision 1.25  2000/10/13 02:15:23  robertj
 * Added support for Progress Indicator Q.931/H.225 message.
 *
 * Revision 1.24  2000/07/09 14:53:17  robertj
 * Added facility IE to facility message.
 * Changed reference to the word "field" to be more correct IE or "Information Element"
 *
 * Revision 1.23  2000/06/21 08:07:39  robertj
 * Added cause/reason to release complete PDU, where relevent.
 *
 * Revision 1.22  2000/05/18 11:53:34  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.21  2000/05/09 12:19:23  robertj
 * Added ability to get and set "distinctive ring" Q.931 functionality.
 *
 * Revision 1.20  2000/05/08 14:07:26  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.19  2000/05/06 02:17:49  robertj
 * Changed the new CallingPartyNumber code so defaults for octet3a are application dependent.
 *
 * Revision 1.18  2000/05/05 00:44:05  robertj
 * Added presentation and screening fields to Calling Party Number field, thanks Dean Anderson.
 *
 * Revision 1.17  2000/05/02 04:32:25  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.16  2000/03/21 01:07:21  robertj
 * Fixed incorrect call reference code being used in originated call.
 *
 * Revision 1.15  1999/12/23 22:43:36  robertj
 * Added calling party number field.
 *
 * Revision 1.14  1999/09/10 03:36:48  robertj
 * Added simple Q.931 Status response to Q.931 Status Enquiry
 *
 * Revision 1.13  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.12  1999/08/13 06:34:38  robertj
 * Fixed problem in CallPartyNumber Q.931 encoding.
 * Added field name display to Q.931 protocol.
 *
 * Revision 1.11  1999/08/10 13:14:15  robertj
 * Added Q.931 Called Number field if have "phone number" style destination addres.
 *
 * Revision 1.10  1999/07/23 02:36:56  robertj
 * Finally found correct value for FACILITY message.
 *
 * Revision 1.9  1999/07/16 02:15:30  robertj
 * Fixed more tunneling problems.
 *
 * Revision 1.8  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.7  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.6  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 * Revision 1.5  1999/02/23 11:04:29  robertj
 * Added capability to make outgoing call.
 *
 * Revision 1.4  1999/01/16 11:31:46  robertj
 * Fixed name in header comment.
 *
 * Revision 1.3  1999/01/16 01:31:39  robertj
 * Major implementation.
 *
 * Revision 1.2  1999/01/02 04:00:55  robertj
 * Added higher level protocol negotiations.
 *
 * Revision 1.1  1998/12/14 09:13:41  robertj
 * Initial revision
 *
 */

#ifndef __H323_Q931_H
#define __H323_Q931_H

#ifdef __GNUC__
#pragma interface
#endif


///////////////////////////////////////////////////////////////////////////////

/**This class embodies a Q.931 Protocol Data Unit.
  */
class Q931 : public PObject
{
  PCLASSINFO(Q931, PObject)
  public:
    enum MsgTypes {
      NationalEscapeMsg  = 0x00,
      AlertingMsg        = 0x01,
      CallProceedingMsg  = 0x02,
      ConnectMsg         = 0x07,
      ConnectAckMsg      = 0x0f,
      ProgressMsg        = 0x03,
      SetupMsg           = 0x05,
      SetupAckMsg        = 0x0d,
      ResumeMsg          = 0x26,
      ResumeAckMsg       = 0x2e,
      ResumeRejectMsg    = 0x22,
      SuspendMsg         = 0x25,
      SuspendAckMsg      = 0x2d,
      SuspendRejectMsg   = 0x21,
      UserInformationMsg = 0x20,
      DisconnectMsg      = 0x45,
      ReleaseMsg         = 0x4d,
      ReleaseCompleteMsg = 0x5a,
      RestartMsg         = 0x46,
      RestartAckMsg      = 0x4e,
      SegmentMsg         = 0x60,
      CongestionCtrlMsg  = 0x79,
      InformationMsg     = 0x7b,
      NotifyMsg          = 0x6e,
      StatusMsg          = 0x7d,
      StatusEnquiryMsg   = 0x75,
      FacilityMsg        = 0x62
    };

    Q931();
    Q931 & operator=(const Q931 & other);

    void BuildFacility(int callRef, BOOL fromDest);
    void BuildInformation(int callRef, BOOL fromDest);
    void BuildProgress(
      int callRef,
      BOOL fromDest,
      unsigned description,
      unsigned codingStandard = 0,
      unsigned location = 0
    );
    void BuildNotify(int callRef, BOOL fromDest);
    void BuildCallProceeding(int callRef);
    void BuildSetupAcknowledge(int callRef);
    void BuildAlerting(int callRef);
    void BuildSetup(int callRef = -1);
    void BuildConnect(int callRef);
    void BuildStatus(int callRef, BOOL fromDest);
    void BuildStatusEnquiry(int callRef, BOOL fromDest);
    void BuildReleaseComplete(int callRef, BOOL fromDest);

    BOOL Decode(const PBYTEArray & data);
    BOOL Encode(PBYTEArray & data) const;

    void PrintOn(ostream & strm) const;
    PString GetMessageTypeName() const;

    void GenerateCallReference();
    unsigned GetCallReference() const { return callReference; }
    BOOL IsFromDestination() const { return fromDestination; }
    MsgTypes GetMessageType() const { return messageType; }

    enum InformationElementCodes {
      BearerCapabilityIE   = 0x04,
      CauseIE              = 0x08,
      FacilityIE           = 0x1c,
      ProgressIndicatorIE  = 0x1e,
      CallStateIE          = 0x14,
      DisplayIE            = 0x28,
      SignalIE             = 0x34,
      CallingPartyNumberIE = 0x6c,
      CalledPartyNumberIE  = 0x70,
      UserUserIE           = 0x7e
    };
    BOOL HasIE(InformationElementCodes ie) const;
    PBYTEArray GetIE(InformationElementCodes ie) const;
    void SetIE(InformationElementCodes ie, const PBYTEArray & userData);
    void RemoveIE(InformationElementCodes ie);

    enum CauseValues {
      NoRouteToNetwork      = 0x02,
      NoRouteToDestination  = 0x03,
      ChannelUnacceptable   = 0x06,
      NormalCallClearing    = 0x10,
      UserBusy              = 0x11,
      NoResponse            = 0x12,
      NoAnswer              = 0x13,
      SubscriberAbsent      = 0x14,
      CallRejected          = 0x15,
      NumberChanged         = 0x16,
      Redirection           = 0x17,
      DestinationOutOfOrder = 0x1b,
      InvalidNumberFormat   = 0x1c,
      StatusEnquiryResponse = 0x1e,
      Congestion            = 0x2a,
      InvalidCallReference  = 0x51,
      ErrorInCauseIE        = 0
    };
    void SetCause(
      CauseValues value,
      unsigned standard = 0,  // 0 = ITU-T standardized coding
      unsigned location = 0   // 0 = User
    );
    CauseValues GetCause(
      unsigned * standard = NULL,  // 0 = ITU-T standardized coding
      unsigned * location = NULL   // 0 = User
    ) const;

    enum SignalInfo {
      SignalDialToneOn,
      SignalRingBackToneOn,
      SignalInterceptToneOn,
      SignalNetworkCongestionToneOn,
      SignalBusyToneOn,
      SignalConfirmToneOn,
      SignalAnswerToneOn,
      SignalCallWaitingTone,
      SignalOffhookWarningTone,
      SignalPreemptionToneOn,
      SignalTonesOff = 0x3f,
      SignalAlertingPattern0 = 0x40,
      SignalAlertingPattern1,
      SignalAlertingPattern2,
      SignalAlertingPattern3,
      SignalAlertingPattern4,
      SignalAlertingPattern5,
      SignalAlertingPattern6,
      SignalAlertingPattern7,
      SignalAlretingOff = 0x4f,
      SignalErrorInIE = 0x100
    };
    void SetSignalInfo(SignalInfo value);
    SignalInfo GetSignalInfo() const;

    void SetProgressIndicator(
      unsigned description,
      unsigned codingStandard = 0,
      unsigned location = 0
    );
    BOOL GetProgressIndicator(
      unsigned & description,
      unsigned * codingStandard = NULL,
      unsigned * location = NULL
    );

    void SetDisplayName(const PString & name);
    PString GetDisplayName() const;

    enum NumberingPlanCodes {
      UnknownPlan          = 0x00,
      ISDNPlan             = 0x01,
      DataPlan             = 0x03,
      TelexPlan            = 0x04,
      NationalStandardPlan = 0x08,
      PrivatePlan          = 0x09,
      ReservedPlan         = 0x0f
    };

    enum TypeOfNumberCodes {
      UnknownType          = 0x00,
      InternationalType    = 0x01,
      NationalType         = 0x02,
      NetworkSpecificType  = 0x03,
      SubscriberType       = 0x04,
      AbbreviatedType      = 0x06,
      ReservedType         = 0x07
    };

    void SetCallingPartyNumber(
      const PString & number, // Number string
      unsigned plan = 1,      // 1 = ISDN/Telephony numbering system
      unsigned type = 0,      // 0 = Unknown number type
      int presentation = -1,  // 0 = presentation allowed, -1 = no octet3a
      int screening = -1      //  0 = user provided, not screened
    );
    BOOL GetCallingPartyNumber(
      PString & number,               // Number string
      unsigned * plan = NULL,         // ISDN/Telephony numbering system
      unsigned * type = NULL,         // Number type
      unsigned * presentation = NULL, // Presentation indicator
      unsigned * screening = NULL,    // Screening indicator
      unsigned defPresentation = 0,   // Default value if octet3a not present
      unsigned defScreening = 0       // Default value if octet3a not present
    ) const;

    void SetCalledPartyNumber(
      const PString & number, // Number string
      unsigned plan = 1,      // 1 = ISDN/Telephony numbering system
      unsigned type = 0       // 0 = Unknown number type
    );
    BOOL GetCalledPartyNumber(
      PString & number,       // Number string
      unsigned * plan = NULL, // ISDN/Telephony numbering system
      unsigned * type = NULL  // Number type
    ) const;

  protected:
    unsigned callReference;
    BOOL fromDestination;
    unsigned protocolDiscriminator;
    MsgTypes messageType;

    PDICTIONARY(InternalInformationElements, POrdinalKey, PBYTEArray);
    InternalInformationElements informationElements;
};


#endif // __H323_Q931_H


/////////////////////////////////////////////////////////////////////////////
