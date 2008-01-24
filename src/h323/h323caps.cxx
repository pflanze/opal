/*
 * h323caps.cxx
 *
 * H.323 protocol handler
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

#include <opal/buildopts.h>
#if OPAL_H323

#ifdef __GNUC__
#pragma implementation "h323caps.h"
#endif

#include <h323/h323caps.h>

#include <h323/h323ep.h>
#include <h323/h323con.h>
#include <h323/h323pdu.h>
#include <h323/transaddr.h>


#define DEFINE_G711_CAPABILITY(cls, code, capName) \
class cls : public H323_G711Capability { \
  public: \
    cls() : H323_G711Capability(code) { } \
}; \
H323_REGISTER_CAPABILITY(cls, capName) \

#ifndef NO_H323_AUDIO_CODECS

DEFINE_G711_CAPABILITY(H323_G711ALaw64Capability, H323_G711Capability::ALaw, OPAL_G711_ALAW_64K)
DEFINE_G711_CAPABILITY(H323_G711uLaw64Capability, H323_G711Capability::muLaw, OPAL_G711_ULAW_64K)

#endif


#if PTRACING
ostream & operator<<(ostream & o , H323Capability::MainTypes t)
{
  const char * const names[] = {
    "Audio", "Video", "Data", "UserInput"
  };
  return o << names[t];
}

ostream & operator<<(ostream & o , H323Capability::CapabilityDirection d)
{
  const char * const names[] = {
    "Unknown", "Receive", "Transmit", "ReceiveAndTransmit", "NoDirection"
  };
  return o << names[d];
}
#endif


/////////////////////////////////////////////////////////////////////////////

H323Capability::H323Capability()
{
  assignedCapabilityNumber = 0; // Unassigned
  capabilityDirection = e_Unknown;
  rtpPayloadType = RTP_DataFrame::IllegalPayloadType;
}


PObject::Comparison H323Capability::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, H323Capability), PInvalidCast);
  const H323Capability & other = (const H323Capability &)obj;

  int mt = GetMainType();
  int omt = other.GetMainType();
  if (mt < omt)
    return LessThan;
  if (mt > omt)
    return GreaterThan;

  int st = GetSubType();
  int ost = other.GetSubType();
  if (st < ost)
    return LessThan;
  if (st > ost)
    return GreaterThan;

  return EqualTo;
}


void H323Capability::PrintOn(ostream & strm) const
{
  strm << GetFormatName();
  if (assignedCapabilityNumber != 0)
    strm << " <" << assignedCapabilityNumber << '>';
}


H323Capability * H323Capability::Create(const PString & name)
{
  H323Capability * cap = H323CapabilityFactory::CreateInstance(name);
  if (cap == NULL)
    return NULL;

  return (H323Capability *)cap->Clone();
}


void H323Capability::SetTxFramesInPacket(unsigned /*frames*/)
{
}


unsigned H323Capability::GetTxFramesInPacket() const
{
  return 1;
}


unsigned H323Capability::GetRxFramesInPacket() const
{
  return 1;
}


PBoolean H323Capability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return subTypePDU.GetTag() == GetSubType();
}


PBoolean H323Capability::OnReceivedPDU(const H245_Capability & cap)
{
  switch (cap.GetTag()) {
    case H245_Capability::e_receiveVideoCapability:
    case H245_Capability::e_receiveAudioCapability:
    case H245_Capability::e_receiveDataApplicationCapability:
    case H245_Capability::e_h233EncryptionReceiveCapability:
    case H245_Capability::e_receiveUserInputCapability:
      capabilityDirection = e_Receive;
      break;

    case H245_Capability::e_transmitVideoCapability:
    case H245_Capability::e_transmitAudioCapability:
    case H245_Capability::e_transmitDataApplicationCapability:
    case H245_Capability::e_h233EncryptionTransmitCapability:
    case H245_Capability::e_transmitUserInputCapability:
      capabilityDirection = e_Transmit;
      break;

    case H245_Capability::e_receiveAndTransmitVideoCapability:
    case H245_Capability::e_receiveAndTransmitAudioCapability:
    case H245_Capability::e_receiveAndTransmitDataApplicationCapability:
    case H245_Capability::e_receiveAndTransmitUserInputCapability:
      capabilityDirection = e_ReceiveAndTransmit;
      break;

    case H245_Capability::e_conferenceCapability:
    case H245_Capability::e_h235SecurityCapability:
    case H245_Capability::e_maxPendingReplacementFor:
      capabilityDirection = e_NoDirection;
  }

  return PTrue;
}


PBoolean H323Capability::IsUsable(const H323Connection &) const
{
  return PTrue;
}


OpalMediaFormat H323Capability::GetMediaFormat() const
{
  return mediaFormat.IsValid() ? mediaFormat : OpalMediaFormat(GetFormatName());
}


OpalMediaFormat & H323Capability::GetWritableMediaFormat()
{
  if (!mediaFormat.IsValid())
    mediaFormat = GetFormatName();
  return mediaFormat;
}


/////////////////////////////////////////////////////////////////////////////

H323RealTimeCapability::H323RealTimeCapability()
{
    rtpqos = NULL;
}

H323RealTimeCapability::H323RealTimeCapability(const H323RealTimeCapability & rtc)
  : H323Capability(rtc)
{
  if (rtc.rtpqos == NULL) 
    rtpqos = NULL;
  else {
    rtpqos  = new RTP_QOS();
    *rtpqos = *rtc.rtpqos;
  }
}

H323RealTimeCapability::~H323RealTimeCapability()
{
  if (rtpqos != NULL)
    delete rtpqos;
}

void H323RealTimeCapability::AttachQoS(RTP_QOS * _rtpqos)
{
  if (rtpqos != NULL)
    delete rtpqos;
    
  rtpqos = _rtpqos;
}

H323Channel * H323RealTimeCapability::CreateChannel(H323Connection & connection,
                                                    H323Channel::Directions dir,
                                                    unsigned sessionID,
                                 const H245_H2250LogicalChannelParameters * param) const
{
  return connection.CreateRealTimeLogicalChannel(*this, dir, sessionID, param, rtpqos);
}


/////////////////////////////////////////////////////////////////////////////

H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(CompareFuncType _compareFunc,
                                                             const BYTE * dataPtr,
                                                             PINDEX dataSize)
  :
    t35CountryCode(OpalProductInfo::Default().t35CountryCode),
    t35Extension(OpalProductInfo::Default().t35Extension),
    manufacturerCode(OpalProductInfo::Default().manufacturerCode),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(0),
    comparisonLength(0),
    compareFunc(_compareFunc)
{
}

H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(const BYTE * dataPtr,
                                                             PINDEX dataSize,
                                                             PINDEX _offset,
                                                             PINDEX _len)
  : t35CountryCode(OpalProductInfo::Default().t35CountryCode),
    t35Extension(OpalProductInfo::Default().t35Extension),
    manufacturerCode(OpalProductInfo::Default().manufacturerCode),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(_offset),
    comparisonLength(_len),
    compareFunc(NULL)
{
}


H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(const PString & _oid,
                                                             const BYTE * dataPtr,
                                                             PINDEX dataSize,
                                                             PINDEX _offset,
                                                             PINDEX _len)
  : oid(_oid),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(_offset),
    comparisonLength(_len),
    compareFunc(NULL)
{
}


H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(BYTE country,
                                                             BYTE extension,
                                                             WORD maufacturer,
                                                             const BYTE * dataPtr,
                                                             PINDEX dataSize,
                                                             PINDEX _offset,
                                                             PINDEX _len)
  : t35CountryCode(country),
    t35Extension(extension),
    manufacturerCode(maufacturer),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(_offset),
    comparisonLength(_len),
    compareFunc(NULL)
{
}


H323NonStandardCapabilityInfo::~H323NonStandardCapabilityInfo()
{
}


PBoolean H323NonStandardCapabilityInfo::OnSendingPDU(PBYTEArray & data) const
{
  data = nonStandardData;
  return data.GetSize() > 0;
}


PBoolean H323NonStandardCapabilityInfo::OnReceivedPDU(const PBYTEArray & data)
{
  if (CompareData(data) != PObject::EqualTo)
    return PFalse;

  nonStandardData = data;
  return PTrue;
}


PBoolean H323NonStandardCapabilityInfo::OnSendingNonStandardPDU(PASN_Choice & pdu,
                                                            unsigned nonStandardTag) const
{
  PBYTEArray data;
  if (!OnSendingPDU(data))
    return PFalse;

  pdu.SetTag(nonStandardTag);
  H245_NonStandardParameter & param = (H245_NonStandardParameter &)pdu.GetObject();

  if (!oid) {
    param.m_nonStandardIdentifier.SetTag(H245_NonStandardIdentifier::e_object);
    PASN_ObjectId & nonStandardIdentifier = param.m_nonStandardIdentifier;
    nonStandardIdentifier = oid;
  }
  else {
    param.m_nonStandardIdentifier.SetTag(H245_NonStandardIdentifier::e_h221NonStandard);
    H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;
    h221.m_t35CountryCode = (unsigned)t35CountryCode;
    h221.m_t35Extension = (unsigned)t35Extension;
    h221.m_manufacturerCode = (unsigned)manufacturerCode;
  }

  param.m_data = data;
  return data.GetSize() > 0;
}


PBoolean H323NonStandardCapabilityInfo::OnReceivedNonStandardPDU(const PASN_Choice & pdu,
                                                             unsigned nonStandardTag)
{
  if (pdu.GetTag() != nonStandardTag)
    return PFalse;

  const H245_NonStandardParameter & param = (const H245_NonStandardParameter &)pdu.GetObject();

  if (CompareParam(param) != PObject::EqualTo)
    return PFalse;

  return OnReceivedPDU(param.m_data);
}


PBoolean H323NonStandardCapabilityInfo::IsMatch(const H245_NonStandardParameter & param) const
{
  return CompareParam(param) == PObject::EqualTo && CompareData(param.m_data) == PObject::EqualTo;
}


PObject::Comparison H323NonStandardCapabilityInfo::CompareParam(const H245_NonStandardParameter & param) const
{
  if (!oid) {
    if (param.m_nonStandardIdentifier.GetTag() != H245_NonStandardIdentifier::e_object)
      return PObject::LessThan;

    const PASN_ObjectId & nonStandardIdentifier = param.m_nonStandardIdentifier;
    return oid.Compare(nonStandardIdentifier.AsString());
  }

  if (param.m_nonStandardIdentifier.GetTag() != H245_NonStandardIdentifier::e_h221NonStandard)
    return PObject::LessThan;

  const H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;

  if (h221.m_t35CountryCode < (unsigned)t35CountryCode)
    return PObject::LessThan;
  if (h221.m_t35CountryCode > (unsigned)t35CountryCode)
    return PObject::GreaterThan;

  if (h221.m_t35Extension < (unsigned)t35Extension)
    return PObject::LessThan;
  if (h221.m_t35Extension > (unsigned)t35Extension)
    return PObject::GreaterThan;

  if (h221.m_manufacturerCode < (unsigned)manufacturerCode)
    return PObject::LessThan;
  if (h221.m_manufacturerCode > (unsigned)manufacturerCode)
    return PObject::GreaterThan;


  return PObject::EqualTo;
}


PObject::Comparison H323NonStandardCapabilityInfo::CompareInfo(const H323NonStandardCapabilityInfo & other) const
{
  return CompareData(other.nonStandardData);
}


PObject::Comparison H323NonStandardCapabilityInfo::CompareData(const PBYTEArray & data) const
{
  if (comparisonOffset >= nonStandardData.GetSize())
    return PObject::LessThan;
  if (comparisonOffset >= data.GetSize())
    return PObject::GreaterThan;

  PINDEX len = comparisonLength;
  if (comparisonOffset+len > nonStandardData.GetSize())
    len = nonStandardData.GetSize() - comparisonOffset;

  if (comparisonOffset+len > data.GetSize())
    return PObject::GreaterThan;

  int cmp = memcmp((const BYTE *)nonStandardData + comparisonOffset,
                   (const BYTE *)data + comparisonOffset,
                   len);
  if (cmp < 0)
    return PObject::LessThan;
  if (cmp > 0)
    return PObject::GreaterThan;
  return PObject::EqualTo;
}


/////////////////////////////////////////////////////////////////////////////

H323GenericCapabilityInfo::H323GenericCapabilityInfo(const PString & standardId, PINDEX maxBitRate)
  : maxBitRate(maxBitRate)
{
  identifier = new H245_CapabilityIdentifier(H245_CapabilityIdentifier::e_standard);
  PASN_ObjectId & object_id = *identifier;
  object_id = standardId;
}


H323GenericCapabilityInfo::H323GenericCapabilityInfo(const H323GenericCapabilityInfo & obj)
  : maxBitRate(obj.maxBitRate)
{
  identifier = new H245_CapabilityIdentifier(*obj.identifier);
}


H323GenericCapabilityInfo::~H323GenericCapabilityInfo()
{
  delete identifier;
}


PBoolean H323GenericCapabilityInfo::OnSendingGenericPDU(H245_GenericCapability & pdu,
                                                    const OpalMediaFormat & mediaFormat,
                                                    H323Capability::CommandType type) const
{
  pdu.m_capabilityIdentifier = *identifier;

  unsigned bitRate = maxBitRate != 0 ? maxBitRate : ((mediaFormat.GetBandwidth()+99)/100);
  if (bitRate != 0) {
    pdu.IncludeOptionalField(H245_GenericCapability::e_maxBitRate);
    pdu.m_maxBitRate = bitRate;
  }

  for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
    const OpalMediaOption & option = mediaFormat.GetOption(i);
    OpalMediaOption::H245GenericInfo genericInfo = option.GetH245Generic();
    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::None)
      continue;
    switch (type) {
      case H323Capability::e_TCS :
        if (genericInfo.excludeTCS)
          continue;
        break;
      case H323Capability::e_OLC :
        if (genericInfo.excludeOLC)
          continue;
        break;
      case H323Capability::e_ReqMode :
        if (genericInfo.excludeReqMode)
          continue;
        break;
    }

    H245_GenericParameter * param = new H245_GenericParameter;

    param->m_parameterIdentifier.SetTag(H245_ParameterIdentifier::e_standard);
    (PASN_Integer &)param->m_parameterIdentifier = genericInfo.ordinal;

    if (PIsDescendant(&option, OpalMediaOptionBoolean)) {
      if (!((const OpalMediaOptionBoolean &)option).GetValue()) {
        delete param;
        continue; // Do not include a logical at all if it is false
      }
      param->m_parameterValue.SetTag(H245_ParameterValue::e_logical);
    }
    else if (PIsDescendant(&option, OpalMediaOptionUnsigned)) {
      unsigned tag;
      switch (genericInfo.integerType) {
        default :
        case OpalMediaOption::H245GenericInfo::UnsignedInt :
          tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsignedMin : H245_ParameterValue::e_unsignedMax;
          break;

        case OpalMediaOption::H245GenericInfo::Unsigned32 :
          tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsigned32Min : H245_ParameterValue::e_unsigned32Max;
          break;

        case OpalMediaOption::H245GenericInfo::BooleanArray :
          tag = H245_ParameterValue::e_booleanArray;
          break;
      }

      param->m_parameterValue.SetTag(tag);
      (PASN_Integer &)param->m_parameterValue = ((const OpalMediaOptionUnsigned &)option).GetValue();
    }
    else {
      param->m_parameterValue.SetTag(H245_ParameterValue::e_octetString);
      PASN_OctetString & octetString = param->m_parameterValue;
      if (PIsDescendant(&option, OpalMediaOptionOctets))
        octetString = ((const OpalMediaOptionOctets &)option).GetValue();
      else
        octetString = option.AsString();
    }

    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::Collapsing) {
      pdu.IncludeOptionalField(H245_GenericCapability::e_collapsing);
      pdu.m_collapsing.Append(param);
    }
    else {
      pdu.IncludeOptionalField(H245_GenericCapability::e_nonCollapsing);
      pdu.m_nonCollapsing.Append(param);
    }
  }

  return PTrue;
}

PBoolean H323GenericCapabilityInfo::OnReceivedGenericPDU(OpalMediaFormat & mediaFormat,
                                                     const H245_GenericCapability & pdu,
                                                     H323Capability::CommandType type)
{
  if (pdu.m_capabilityIdentifier != *identifier)
    return PFalse;

  if (pdu.HasOptionalField(H245_GenericCapability::e_maxBitRate)) {
    maxBitRate = pdu.m_maxBitRate;
    mediaFormat.SetOptionInteger(OpalMediaFormat::MaxBitRateOption(), maxBitRate*100);
  }

  for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
    const OpalMediaOption & option = mediaFormat.GetOption(i);
    OpalMediaOption::H245GenericInfo genericInfo = option.GetH245Generic();
    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::None)
      continue;
    switch (type) {
      case H323Capability::e_TCS :
        if (genericInfo.excludeTCS)
          continue;
        break;
      case H323Capability::e_OLC :
        if (genericInfo.excludeOLC)
          continue;
        break;
      case H323Capability::e_ReqMode :
        if (genericInfo.excludeReqMode)
          continue;
        break;
    }

    const H245_ArrayOf_GenericParameter * params;
    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::Collapsing) {
      if (!pdu.HasOptionalField(H245_GenericCapability::e_collapsing))
        continue;
      params = &pdu.m_collapsing;
    }
    else {
      if (!pdu.HasOptionalField(H245_GenericCapability::e_nonCollapsing))
        continue;
      params = &pdu.m_nonCollapsing;
    }

    if (PIsDescendant(&option, OpalMediaOptionBoolean))
      ((OpalMediaOptionBoolean &)option).SetValue(false);

    for (PINDEX j = 0; j < params->GetSize(); j++) {
      const H245_GenericParameter & param = (*params)[j];
      if (param.m_parameterIdentifier.GetTag() == H245_ParameterIdentifier::e_standard &&
                         (const PASN_Integer &)param.m_parameterIdentifier == genericInfo.ordinal) {
        if (PIsDescendant(&option, OpalMediaOptionBoolean)) {
          if (param.m_parameterValue.GetTag() == H245_ParameterValue::e_logical) {
            ((OpalMediaOptionBoolean &)option).SetValue(true);
            break;
          }
        }
        else if (PIsDescendant(&option, OpalMediaOptionUnsigned)) {
          unsigned tag;
          switch (genericInfo.integerType) {
            default :
            case OpalMediaOption::H245GenericInfo::UnsignedInt :
              tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsignedMin : H245_ParameterValue::e_unsignedMax;
              break;
 
            case OpalMediaOption::H245GenericInfo::Unsigned32 :
              tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsigned32Min : H245_ParameterValue::e_unsigned32Max;
              break;
 
            case OpalMediaOption::H245GenericInfo::BooleanArray :
              tag = H245_ParameterValue::e_booleanArray;
              break;
          }
 
          if (param.m_parameterValue.GetTag() == tag) {
            ((OpalMediaOptionUnsigned &)option).SetValue((const PASN_Integer &)param.m_parameterValue);
            break;
          }
        }
        else {
          if (param.m_parameterValue.GetTag() == H245_ParameterValue::e_octetString) {
            const PASN_OctetString & octetString = param.m_parameterValue;
            if (PIsDescendant(&option, OpalMediaOptionOctets))
              ((OpalMediaOptionOctets &)option).SetValue(octetString);
            else
              ((OpalMediaOption &)option).FromString(octetString.AsString());
            break;
          }
        }

        PTRACE(2, "Invalid generic parameter type (" << param.m_parameterValue.GetTagName()
               << ") for option \"" << option.GetName() << "\" (" << option.GetClass() << ')');
      }
    }
  }

  return PTrue;
}

PBoolean H323GenericCapabilityInfo::IsMatch(const H245_GenericCapability & param) const
{
  return param.m_capabilityIdentifier == *identifier;
}

PObject::Comparison H323GenericCapabilityInfo::CompareInfo(const H323GenericCapabilityInfo & obj) const
{
  return identifier->Compare(*obj.identifier);
}


/////////////////////////////////////////////////////////////////////////////

#if OPAL_AUDIO

H323AudioCapability::H323AudioCapability()
{
}


H323Capability::MainTypes H323AudioCapability::GetMainType() const
{
  return e_Audio;
}


OpalMediaType H323AudioCapability::GetMediaType() const
{
  return OpalMediaType::Audio();
}


void H323AudioCapability::SetTxFramesInPacket(unsigned frames)
{
  GetWritableMediaFormat().SetOptionInteger(OpalAudioFormat::TxFramesPerPacketOption(), frames);
}


unsigned H323AudioCapability::GetTxFramesInPacket() const
{
  return GetMediaFormat().GetOptionInteger(OpalAudioFormat::TxFramesPerPacketOption(), 1);
}


unsigned H323AudioCapability::GetRxFramesInPacket() const
{
  return GetMediaFormat().GetOptionInteger(OpalAudioFormat::RxFramesPerPacketOption(), 1);
}


PBoolean H323AudioCapability::OnSendingPDU(H245_Capability & cap) const
{
  cap.SetTag(H245_Capability::e_receiveAudioCapability);
  return OnSendingPDU((H245_AudioCapability &)cap, GetRxFramesInPacket(), e_TCS);
}


PBoolean H323AudioCapability::OnSendingPDU(H245_DataType & dataType) const
{
  dataType.SetTag(H245_DataType::e_audioData);
  return OnSendingPDU((H245_AudioCapability &)dataType, GetTxFramesInPacket(), e_OLC);
}


PBoolean H323AudioCapability::OnSendingPDU(H245_ModeElement & mode) const
{
  mode.m_type.SetTag(H245_ModeElementType::e_audioMode);
  return OnSendingPDU((H245_AudioMode &)mode.m_type);
}


PBoolean H323AudioCapability::OnSendingPDU(H245_AudioCapability & pdu,
                                       unsigned packetSize) const
{
  pdu.SetTag(GetSubType());

  // Set the maximum number of frames
  PASN_Integer & value = pdu;
  value = packetSize;
  return PTrue;
}


PBoolean H323AudioCapability::OnSendingPDU(H245_AudioCapability & pdu,
                                       unsigned packetSize,
                                       CommandType) const
{
  return OnSendingPDU(pdu, packetSize);
}


PBoolean H323AudioCapability::OnSendingPDU(H245_AudioMode & pdu) const
{
  static const H245_AudioMode::Choices AudioTable[] = {
    H245_AudioMode::e_nonStandard,
    H245_AudioMode::e_g711Alaw64k,
    H245_AudioMode::e_g711Alaw56k,
    H245_AudioMode::e_g711Ulaw64k,
    H245_AudioMode::e_g711Ulaw56k,
    H245_AudioMode::e_g722_64k,
    H245_AudioMode::e_g722_56k,
    H245_AudioMode::e_g722_48k,
    H245_AudioMode::e_g7231,
    H245_AudioMode::e_g728,
    H245_AudioMode::e_g729,
    H245_AudioMode::e_g729AnnexA,
    H245_AudioMode::e_is11172AudioMode,
    H245_AudioMode::e_is13818AudioMode,
    H245_AudioMode::e_g729wAnnexB,
    H245_AudioMode::e_g729AnnexAwAnnexB,
    H245_AudioMode::e_g7231AnnexCMode,
    H245_AudioMode::e_gsmFullRate,
    H245_AudioMode::e_gsmHalfRate,
    H245_AudioMode::e_gsmEnhancedFullRate,
    H245_AudioMode::e_genericAudioMode,
    H245_AudioMode::e_g729Extensions
  };

  unsigned subType = GetSubType();
  if (subType >= PARRAYSIZE(AudioTable))
    return PFalse;

  pdu.SetTag(AudioTable[subType]);
  return PTrue;
}


PBoolean H323AudioCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag() != H245_Capability::e_receiveAudioCapability &&
      cap.GetTag() != H245_Capability::e_receiveAndTransmitAudioCapability)
    return PFalse;

  unsigned txFramesInPacket = GetTxFramesInPacket();
  unsigned packetSize = txFramesInPacket;
  if (!OnReceivedPDU((const H245_AudioCapability &)cap, packetSize, e_TCS))
    return PFalse;

  // Clamp our transmit size to maximum allowed
  if (txFramesInPacket > packetSize) {
    PTRACE(4, "H323\tCapability tx frames reduced from "
           << txFramesInPacket << " to " << packetSize);
    SetTxFramesInPacket(packetSize);
  }
  else {
    PTRACE(4, "H323\tCapability tx frames left at "
           << txFramesInPacket << " as remote allows " << packetSize);
  }

  return PTrue;
}


PBoolean H323AudioCapability::OnReceivedPDU(const H245_DataType & dataType, PBoolean receiver)
{
  if (dataType.GetTag() != H245_DataType::e_audioData)
    return PFalse;

  unsigned xFramesInPacket = receiver ? GetRxFramesInPacket() : GetTxFramesInPacket();
  unsigned packetSize = xFramesInPacket;
  if (!OnReceivedPDU((const H245_AudioCapability &)dataType, packetSize, e_OLC))
    return PFalse;

  // Clamp our transmit size to maximum allowed
  if (xFramesInPacket > packetSize) {
    PTRACE(4, "H323\tCapability " << (receiver ? 'r' : 't') << "x frames reduced from "
           << xFramesInPacket << " to " << packetSize);
    if (!receiver)
      SetTxFramesInPacket(packetSize);
  }
  else {
    PTRACE(4, "H323\tCapability " << (receiver ? 'r' : 't') << "x frames left at "
           << xFramesInPacket << " as remote allows " << packetSize);
  }

  return PTrue;
}


PBoolean H323AudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                        unsigned & packetSize)
{
  if (pdu.GetTag() != GetSubType())
    return PFalse;

  const PASN_Integer & value = pdu;

  // Get the maximum number of frames
  packetSize = value;
  return PTrue;
}


PBoolean H323AudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                        unsigned & packetSize,
                                        CommandType)
{
  return OnReceivedPDU(pdu, packetSize);
}


/////////////////////////////////////////////////////////////////////////////

H323GenericAudioCapability::H323GenericAudioCapability(const PString &standardId, PINDEX maxBitRate)
  : H323AudioCapability(),
    H323GenericCapabilityInfo(standardId, maxBitRate)
{
}

PObject::Comparison H323GenericAudioCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323GenericAudioCapability))
    return LessThan;

  return CompareInfo((const H323GenericAudioCapability &)obj);
}


unsigned H323GenericAudioCapability::GetSubType() const
{
  return H245_AudioCapability::e_genericAudioCapability;
}


PBoolean H323GenericAudioCapability::OnSendingPDU(H245_AudioCapability & pdu, unsigned, CommandType type) const
{
  pdu.SetTag(H245_AudioCapability::e_genericAudioCapability);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), type);
}

PBoolean H323GenericAudioCapability::OnSendingPDU(H245_AudioMode & pdu) const
{
  pdu.SetTag(H245_VideoMode::e_genericVideoMode);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), e_ReqMode);
}

PBoolean H323GenericAudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu, unsigned &, CommandType type)
{
  if( pdu.GetTag() != H245_AudioCapability::e_genericAudioCapability)
    return PFalse;
  return OnReceivedGenericPDU(GetWritableMediaFormat(), pdu, type);
}

PBoolean H323GenericAudioCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323GenericCapabilityInfo::IsMatch((const H245_GenericCapability &)subTypePDU.GetObject());
}


/////////////////////////////////////////////////////////////////////////////

H323NonStandardAudioCapability::H323NonStandardAudioCapability(
      H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
      const BYTE * fixedData,
      PINDEX dataSize)
  : H323AudioCapability(),
    H323NonStandardCapabilityInfo(compareFunc, fixedData, dataSize)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(const BYTE * fixedData,
                                                                     PINDEX dataSize,
                                                                     PINDEX offset,
                                                                     PINDEX length)
  : H323AudioCapability(),
    H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(const PString & oid,
                                                                 const BYTE * fixedData,
                                                                       PINDEX dataSize,
                                                                       PINDEX offset,
                                                                      PINDEX length)
  : H323AudioCapability(),
    H323NonStandardCapabilityInfo(oid, fixedData, dataSize, offset, length)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(BYTE country,
                                                               BYTE extension,
                                                               WORD maufacturer,
                                                       const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323AudioCapability(),
    H323NonStandardCapabilityInfo(country, extension, maufacturer, fixedData, dataSize, offset, length)
{
}


PObject::Comparison H323NonStandardAudioCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323NonStandardAudioCapability))
    return PObject::LessThan;

  return CompareInfo((const H323NonStandardAudioCapability &)obj);
}


unsigned H323NonStandardAudioCapability::GetSubType() const
{
  return H245_AudioCapability::e_nonStandard;
}


PBoolean H323NonStandardAudioCapability::OnSendingPDU(H245_AudioCapability & pdu,
                                                  unsigned) const
{
  return OnSendingNonStandardPDU(pdu, H245_AudioCapability::e_nonStandard);
}


PBoolean H323NonStandardAudioCapability::OnSendingPDU(H245_AudioMode & pdu) const
{
  return OnSendingNonStandardPDU(pdu, H245_AudioMode::e_nonStandard);
}


PBoolean H323NonStandardAudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                                   unsigned &)
{
  return OnReceivedNonStandardPDU(pdu, H245_AudioCapability::e_nonStandard);
}


PBoolean H323NonStandardAudioCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323NonStandardCapabilityInfo::IsMatch((const H245_NonStandardParameter &)subTypePDU.GetObject());
}

#endif // OPAL_AUDIO

/////////////////////////////////////////////////////////////////////////////

#if OPAL_VIDEO

H323Capability::MainTypes H323VideoCapability::GetMainType() const
{
  return e_Video;
}


PBoolean H323VideoCapability::OnSendingPDU(H245_Capability & cap) const
{
  cap.SetTag(H245_Capability::e_receiveVideoCapability);
  return OnSendingPDU((H245_VideoCapability &)cap, e_TCS);
}


PBoolean H323VideoCapability::OnSendingPDU(H245_DataType & dataType) const
{
  dataType.SetTag(H245_DataType::e_videoData);
  return OnSendingPDU((H245_VideoCapability &)dataType, e_OLC);
}


PBoolean H323VideoCapability::OnSendingPDU(H245_VideoCapability & /*pdu*/) const
{
  return PFalse;
}


PBoolean H323VideoCapability::OnSendingPDU(H245_VideoCapability & pdu, CommandType) const
{
  return OnSendingPDU(pdu);
}


PBoolean H323VideoCapability::OnSendingPDU(H245_ModeElement & mode) const
{
  mode.m_type.SetTag(H245_ModeElementType::e_videoMode);
  return OnSendingPDU((H245_VideoMode &)mode.m_type);
}


PBoolean H323VideoCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag() != H245_Capability::e_receiveVideoCapability &&
      cap.GetTag() != H245_Capability::e_receiveAndTransmitVideoCapability)
    return PFalse;

  return OnReceivedPDU((const H245_VideoCapability &)cap, e_TCS);
}


PBoolean H323VideoCapability::OnReceivedPDU(const H245_DataType & dataType, PBoolean)
{
  if (dataType.GetTag() != H245_DataType::e_videoData)
    return PFalse;

  return OnReceivedPDU((const H245_VideoCapability &)dataType, e_OLC);
}


PBoolean H323VideoCapability::OnReceivedPDU(const H245_VideoCapability &)
{
  return PFalse;
}


PBoolean H323VideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu, CommandType)
{
  return OnReceivedPDU(pdu);
}


OpalMediaType H323VideoCapability::GetMediaType() const
{
  return OpalMediaType::Video();
}


/////////////////////////////////////////////////////////////////////////////

H323NonStandardVideoCapability::H323NonStandardVideoCapability(
      H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
      const BYTE * fixedData,
      PINDEX dataSize)
  : H323VideoCapability(),
    H323NonStandardCapabilityInfo(compareFunc, fixedData, dataSize)
{
}

H323NonStandardVideoCapability::H323NonStandardVideoCapability(const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}


H323NonStandardVideoCapability::H323NonStandardVideoCapability(const PString & oid,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(oid, fixedData, dataSize, offset, length)
{
}


H323NonStandardVideoCapability::H323NonStandardVideoCapability(BYTE country,
                                                               BYTE extension,
                                                               WORD maufacturer,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(country, extension, maufacturer, fixedData, dataSize, offset, length)
{
}


PObject::Comparison H323NonStandardVideoCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323NonStandardVideoCapability))
    return PObject::LessThan;

  return CompareInfo((const H323NonStandardVideoCapability &)obj);
}


unsigned H323NonStandardVideoCapability::GetSubType() const
{
  return H245_VideoCapability::e_nonStandard;
}


PBoolean H323NonStandardVideoCapability::OnSendingPDU(H245_VideoCapability & pdu) const
{
  return OnSendingNonStandardPDU(pdu, H245_VideoCapability::e_nonStandard);
}


PBoolean H323NonStandardVideoCapability::OnSendingPDU(H245_VideoMode & pdu) const
{
  return OnSendingNonStandardPDU(pdu, H245_VideoMode::e_nonStandard);
}


PBoolean H323NonStandardVideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu)
{
  return OnReceivedNonStandardPDU(pdu, H245_VideoCapability::e_nonStandard);
}


PBoolean H323NonStandardVideoCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323NonStandardCapabilityInfo::IsMatch((const H245_NonStandardParameter &)subTypePDU.GetObject());
}

/////////////////////////////////////////////////////////////////////////////

H323GenericVideoCapability::H323GenericVideoCapability(const PString &standardId, PINDEX maxBitRate)
  : H323VideoCapability(),
    H323GenericCapabilityInfo(standardId, maxBitRate)
{
}

PObject::Comparison H323GenericVideoCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323GenericVideoCapability))
    return LessThan;

  return CompareInfo((const H323GenericVideoCapability &)obj);
}


unsigned H323GenericVideoCapability::GetSubType() const
{
  return H245_VideoCapability::e_genericVideoCapability;
}


PBoolean H323GenericVideoCapability::OnSendingPDU(H245_VideoCapability & pdu, CommandType type) const
{
  pdu.SetTag(H245_VideoCapability::e_genericVideoCapability);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), type);
}

PBoolean H323GenericVideoCapability::OnSendingPDU(H245_VideoMode & pdu) const
{
  pdu.SetTag(H245_VideoMode::e_genericVideoMode);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), e_ReqMode);
}

PBoolean H323GenericVideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu, CommandType type)
{
  if (pdu.GetTag() != H245_VideoCapability::e_genericVideoCapability)
    return PFalse;
  return OnReceivedGenericPDU(GetWritableMediaFormat(), pdu, type);
}

PBoolean H323GenericVideoCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323GenericCapabilityInfo::IsMatch((const H245_GenericCapability &)subTypePDU.GetObject());
}


#endif // OPAL_VIDEO

/////////////////////////////////////////////////////////////////////////////

H323DataCapability::H323DataCapability(unsigned rate)
  : maxBitRate(rate)
{
}


H323Capability::MainTypes H323DataCapability::GetMainType() const
{
  return e_Data;
}


PBoolean H323DataCapability::OnSendingPDU(H245_Capability & cap) const
{
  cap.SetTag(H245_Capability::e_receiveAndTransmitDataApplicationCapability);
  H245_DataApplicationCapability & app = cap;
  app.m_maxBitRate = maxBitRate;
  return OnSendingPDU(app, e_TCS);
}


PBoolean H323DataCapability::OnSendingPDU(H245_DataType & dataType) const
{
  dataType.SetTag(H245_DataType::e_data);
  H245_DataApplicationCapability & app = dataType;
  app.m_maxBitRate = maxBitRate;
  return OnSendingPDU(app, e_OLC);
}


PBoolean H323DataCapability::OnSendingPDU(H245_DataApplicationCapability &) const
{
  return PFalse;
}


PBoolean H323DataCapability::OnSendingPDU(H245_DataApplicationCapability & pdu, CommandType) const
{
  return OnSendingPDU(pdu);
}


PBoolean H323DataCapability::OnSendingPDU(H245_ModeElement & mode) const
{
  mode.m_type.SetTag(H245_ModeElementType::e_dataMode);
  H245_DataMode & type = mode.m_type;
  type.m_bitRate = maxBitRate;
  return OnSendingPDU(type);
}


PBoolean H323DataCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag() != H245_Capability::e_receiveDataApplicationCapability &&
      cap.GetTag() != H245_Capability::e_receiveAndTransmitDataApplicationCapability)
    return PFalse;

  const H245_DataApplicationCapability & app = cap;
  maxBitRate = app.m_maxBitRate;
  return OnReceivedPDU(app, e_TCS);
}


PBoolean H323DataCapability::OnReceivedPDU(const H245_DataType & dataType, PBoolean)
{
  if (dataType.GetTag() != H245_DataType::e_data)
    return PFalse;

  const H245_DataApplicationCapability & app = dataType;
  maxBitRate = app.m_maxBitRate;
  return OnReceivedPDU(app, e_OLC);
}


PBoolean H323DataCapability::OnReceivedPDU(const H245_DataApplicationCapability &)
{
  return PFalse;
}


PBoolean H323DataCapability::OnReceivedPDU(const H245_DataApplicationCapability & pdu, CommandType)
{
  return OnReceivedPDU(pdu);
}


/////////////////////////////////////////////////////////////////////////////

H323NonStandardDataCapability::H323NonStandardDataCapability(unsigned maxBitRate,
                                                         const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323DataCapability(maxBitRate),
    H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}


H323NonStandardDataCapability::H323NonStandardDataCapability(unsigned maxBitRate,
                                                             const PString & oid,
                                                             const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323DataCapability(maxBitRate),
    H323NonStandardCapabilityInfo(oid, fixedData, dataSize, offset, length)
{
}


H323NonStandardDataCapability::H323NonStandardDataCapability(unsigned maxBitRate,
                                                             BYTE country,
                                                             BYTE extension,
                                                             WORD maufacturer,
                                                             const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323DataCapability(maxBitRate),
    H323NonStandardCapabilityInfo(country, extension, maufacturer, fixedData, dataSize, offset, length)
{
}


PObject::Comparison H323NonStandardDataCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323NonStandardDataCapability))
    return PObject::LessThan;

  return CompareInfo((const H323NonStandardDataCapability &)obj);
}


unsigned H323NonStandardDataCapability::GetSubType() const
{
  return H245_DataApplicationCapability_application::e_nonStandard;
}


PBoolean H323NonStandardDataCapability::OnSendingPDU(H245_DataApplicationCapability & pdu) const
{
  return OnSendingNonStandardPDU(pdu.m_application, H245_DataApplicationCapability_application::e_nonStandard);
}


PBoolean H323NonStandardDataCapability::OnSendingPDU(H245_DataMode & pdu) const
{
  return OnSendingNonStandardPDU(pdu.m_application, H245_DataMode_application::e_nonStandard);
}


PBoolean H323NonStandardDataCapability::OnReceivedPDU(const H245_DataApplicationCapability & pdu)
{
  return OnReceivedNonStandardPDU(pdu.m_application, H245_DataApplicationCapability_application::e_nonStandard);
}


PBoolean H323NonStandardDataCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323NonStandardCapabilityInfo::IsMatch((const H245_NonStandardParameter &)subTypePDU.GetObject());
}


/////////////////////////////////////////////////////////////////////////////

H323_G711Capability::H323_G711Capability(Mode m, Speed s)
  : H323AudioCapability()
{
  mode = m;
  speed = s;
  SetTxFramesInPacket(240);   // 240ms max, 30ms desired
}


PObject * H323_G711Capability::Clone() const
{
  return new H323_G711Capability(*this);
}


unsigned H323_G711Capability::GetSubType() const
{
  static const unsigned G711SubType[2][2] = {
    { H245_AudioCapability::e_g711Alaw64k, H245_AudioCapability::e_g711Alaw56k },
    { H245_AudioCapability::e_g711Ulaw64k, H245_AudioCapability::e_g711Ulaw56k }
  };
  return G711SubType[mode][speed];
}


PString H323_G711Capability::GetFormatName() const
{
  static const char * const G711Name[2][2] = {
    { OPAL_G711_ALAW_64K, "G.711-ALaw-56k" },
    { OPAL_G711_ULAW_64K, "G.711-uLaw-56k" }
  };
  return G711Name[mode][speed];
}


/////////////////////////////////////////////////////////////////////////////

#if 0

H323_G728Capability::H323_G728Capability()
  : H323AudioCapability(240, 30) // 240ms max, 30ms desired
{
}


PObject * H323_G728Capability::Clone() const
{
  return new H323_G728Capability(*this);
}


unsigned H323_G728Capability::GetSubType() const
{
  return H245_AudioCapability::e_g728;
}


PString H323_G728Capability::GetFormatName() const
{
  return OpalG728;
}


/////////////////////////////////////////////////////////////////////////////

static const char * const G729Name[4] = {
  OPAL_G729,
  OPAL_G729A,
  OPAL_G729B,
  OPAL_G729AB
};

H323_G729Capability::H323_G729Capability(Mode m)
{
  mode = m;
}


PObject * H323_G729Capability::Clone() const
{
  return new H323_G729Capability(*this);
}


unsigned H323_G729Capability::GetSubType() const
{
  static const unsigned G729SubType[4] = {
    H245_AudioCapability::e_g729,
    H245_AudioCapability::e_g729AnnexA,
    H245_AudioCapability::e_g729wAnnexB,
    H245_AudioCapability::e_g729AnnexAwAnnexB
  };
  return G729SubType[mode];
}


PString H323_G729Capability::GetFormatName() const
{
  return G729Name[mode];
}


/////////////////////////////////////////////////////////////////////////////

H323_G7231Capability::H323_G7231Capability(PBoolean sid)
{
  allowSIDFrames = sid;
}


PObject * H323_G7231Capability::Clone() const
{
  return new H323_G7231Capability(*this);
}


unsigned H323_G7231Capability::GetSubType() const
{
  return H245_AudioCapability::e_g7231;
}


PString H323_G7231Capability::GetFormatName() const
{
  return OpalG7231_6k3;
}


PBoolean H323_G7231Capability::OnSendingPDU(H245_AudioCapability & pdu,
                                        unsigned packetSize) const
{
  pdu.SetTag(H245_AudioCapability::e_g7231);
  H245_AudioCapability_g7231 & g7231 = pdu;
  g7231.m_maxAl_sduAudioFrames = packetSize;
  g7231.m_silenceSuppression = allowSIDFrames;
  return PTrue;
}


PBoolean H323_G7231Capability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                         unsigned & packetSize)
{
  if (pdu.GetTag() != H245_AudioCapability::e_g7231)
    return PFalse;

  const H245_AudioCapability_g7231 & g7231 = pdu;
  packetSize = g7231.m_maxAl_sduAudioFrames;
  allowSIDFrames = g7231.m_silenceSuppression;
  return PTrue;
}


///////////////////////////////////////////////////////////////////////////////

H323_GSM0610Capability::H323_GSM0610Capability()
{
}


PObject * H323_GSM0610Capability::Clone() const
{
  return new H323_GSM0610Capability(*this);
}


unsigned H323_GSM0610Capability::GetSubType() const
{
  return H245_AudioCapability::e_gsmFullRate;
}


PString H323_GSM0610Capability::GetFormatName() const
{
  return OpalGSM0610;
}


void H323_GSM0610Capability::SetTxFramesInPacket(unsigned frames)
{
  H323AudioCapability::SetTxFramesInPacket(frames > 7 ? 7 : frames);
}


PBoolean H323_GSM0610Capability::OnSendingPDU(H245_AudioCapability & cap,
                                          unsigned packetSize) const
{
  cap.SetTag(H245_AudioCapability::e_gsmFullRate);

  H245_GSMAudioCapability & gsm = cap;
  gsm.m_audioUnitSize = packetSize*33;
  return PTrue;
}


PBoolean H323_GSM0610Capability::OnReceivedPDU(const H245_AudioCapability & cap,
                                           unsigned & packetSize)
{
  if (cap.GetTag() != H245_AudioCapability::e_gsmFullRate)
    return PFalse;

  const H245_GSMAudioCapability & gsm = cap;
  packetSize = gsm.m_audioUnitSize/33;
  if (packetSize == 0)
    packetSize = 1;
  return PTrue;
}

#endif   // 0

/////////////////////////////////////////////////////////////////////////////

static const char * const UIISubTypeNames[H323_UserInputCapability::NumSubTypes] = {
  "UserInput/basicString",
  "UserInput/iA5String",
  "UserInput/generalString",
  "UserInput/dtmf",
  "UserInput/hookflash",
  OPAL_RFC2833
};

const char * H323_UserInputCapability::GetSubTypeName(SubTypes s)
{
  return s < PARRAYSIZE(UIISubTypeNames) ? UIISubTypeNames[s] : "<Unknown>";
}

#define DECLARE_USER_INPUT_CLASS(type) \
class H323_UserInputCapability_##type : public H323_UserInputCapability \
{ \
  public: \
    H323_UserInputCapability_##type() \
    : H323_UserInputCapability(H323_UserInputCapability::type) { } \
};\

#define DEFINE_USER_INPUT(type) \
  DECLARE_USER_INPUT_CLASS(type) \
  const OpalMediaFormat UserInput_##type( \
    UIISubTypeNames[H323_UserInputCapability::type], \
    OpalMediaType::UserInput(), RTP_DataFrame::IllegalPayloadType, NULL, PFalse, 1, 0, 0, 0 \
  ); \
  H323_REGISTER_CAPABILITY(H323_UserInputCapability_##type, UIISubTypeNames[H323_UserInputCapability::type]) \

DEFINE_USER_INPUT(BasicString);
DEFINE_USER_INPUT(IA5String);
DEFINE_USER_INPUT(GeneralString);
DEFINE_USER_INPUT(SignalToneH245);
DEFINE_USER_INPUT(HookFlashH245);

OPAL_DEFINE_MEDIA_TYPE(rfc2833, rfc2833); 

DECLARE_USER_INPUT_CLASS(SignalToneRFC2833)
H323_REGISTER_CAPABILITY(H323_UserInputCapability_SignalToneRFC2833, UIISubTypeNames[H323_UserInputCapability::SignalToneRFC2833])

H323_UserInputCapability::H323_UserInputCapability(SubTypes _subType)
{
  subType = _subType;
  rtpPayloadType = OpalRFC2833.GetPayloadType();
}

OpalMediaType H323_UserInputCapability::GetMediaType() const
{
  return OpalMediaType::UserInput();
}


PObject * H323_UserInputCapability::Clone() const
{
  return new H323_UserInputCapability(*this);
}


H323Capability::MainTypes H323_UserInputCapability::GetMainType() const
{
  return e_UserInput;
}


#define SignalToneRFC2833_SubType 10000

static unsigned UserInputCapabilitySubTypeCodes[] = {
  H245_UserInputCapability::e_basicString,
  H245_UserInputCapability::e_iA5String,
  H245_UserInputCapability::e_generalString,
  H245_UserInputCapability::e_dtmf,
  H245_UserInputCapability::e_hookflash,
  SignalToneRFC2833_SubType
};

unsigned  H323_UserInputCapability::GetSubType()  const
{
  return UserInputCapabilitySubTypeCodes[subType];
}


PString H323_UserInputCapability::GetFormatName() const
{
  return UIISubTypeNames[subType];
}


H323Channel * H323_UserInputCapability::CreateChannel(H323Connection &,
                                                      H323Channel::Directions,
                                                      unsigned,
                                                      const H245_H2250LogicalChannelParameters *) const
{
  PTRACE(1, "Codec\tCannot create UserInputCapability channel");
  return NULL;
}


PBoolean H323_UserInputCapability::OnSendingPDU(H245_Capability & pdu) const
{
  if (subType == SignalToneRFC2833) {
    pdu.SetTag(H245_Capability::e_receiveRTPAudioTelephonyEventCapability);
    H245_AudioTelephonyEventCapability & atec = pdu;
    atec.m_dynamicRTPPayloadType = rtpPayloadType;
    atec.m_audioTelephoneEvent = "0-16"; // Support DTMF 0-9,*,#,A-D & hookflash
  }
  else {
    pdu.SetTag(H245_Capability::e_receiveUserInputCapability);
    H245_UserInputCapability & ui = pdu;
    ui.SetTag(UserInputCapabilitySubTypeCodes[subType]);
  }
  return PTrue;
}


PBoolean H323_UserInputCapability::OnSendingPDU(H245_DataType &) const
{
  PTRACE(1, "Codec\tCannot have UserInputCapability in DataType");
  return PFalse;
}


PBoolean H323_UserInputCapability::OnSendingPDU(H245_ModeElement &) const
{
  PTRACE(1, "Codec\tCannot have UserInputCapability in ModeElement");
  return PFalse;
}


PBoolean H323_UserInputCapability::OnReceivedPDU(const H245_Capability & pdu)
{
  H323Capability::OnReceivedPDU(pdu);

  if (pdu.GetTag() == H245_Capability::e_receiveRTPAudioTelephonyEventCapability) {
    subType = SignalToneRFC2833;
    const H245_AudioTelephonyEventCapability & atec = pdu;
    rtpPayloadType = (RTP_DataFrame::PayloadTypes)(int)atec.m_dynamicRTPPayloadType;
    // Really should verify atec.m_audioTelephoneEvent here
    return PTrue;
  }

  if (pdu.GetTag() != H245_Capability::e_receiveUserInputCapability &&
      pdu.GetTag() != H245_Capability::e_receiveAndTransmitUserInputCapability)
    return PFalse;

  const H245_UserInputCapability & ui = pdu;
  return ui.GetTag() == UserInputCapabilitySubTypeCodes[subType];
}


PBoolean H323_UserInputCapability::OnReceivedPDU(const H245_DataType &, PBoolean)
{
  PTRACE(1, "Codec\tCannot have UserInputCapability in DataType");
  return PFalse;
}


PBoolean H323_UserInputCapability::IsUsable(const H323Connection & connection) const
{
  if (connection.GetControlVersion() >= 7)
    return PTrue;

  if (connection.GetRemoteApplication().Find("AltiServ-ITG") != P_MAX_INDEX)
    return PFalse;

  return subType != SignalToneRFC2833;
}


void H323_UserInputCapability::AddAllCapabilities(H323Capabilities & capabilities,
                                                  PINDEX descriptorNum,
                                                  PINDEX simultaneous)
{
  PINDEX num = capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(HookFlashH245));
  if (descriptorNum == P_MAX_INDEX) {
    descriptorNum = num;
    simultaneous = P_MAX_INDEX;
  }
  else if (simultaneous == P_MAX_INDEX)
    simultaneous = num+1;

  num = capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(BasicString));
  if (simultaneous == P_MAX_INDEX)
    simultaneous = num;

  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(SignalToneH245));
  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(SignalToneRFC2833));
}


/////////////////////////////////////////////////////////////////////////////

PBoolean H323SimultaneousCapabilities::SetSize(PINDEX newSize)
{
  PINDEX oldSize = GetSize();

  if (!H323CapabilitiesListArray::SetSize(newSize))
    return PFalse;

  while (oldSize < newSize) {
    H323CapabilitiesList * list = new H323CapabilitiesList;
    // The lowest level list should not delete codecs on destruction
    list->DisallowDeleteObjects();
    SetAt(oldSize++, list);
  }

  return PTrue;
}


PBoolean H323CapabilitiesSet::SetSize(PINDEX newSize)
{
  PINDEX oldSize = GetSize();

  if (!H323CapabilitiesSetArray::SetSize(newSize))
    return PFalse;

  while (oldSize < newSize)
    SetAt(oldSize++, new H323SimultaneousCapabilities);

  return PTrue;
}


H323Capabilities::H323Capabilities()
{
}


H323Capabilities::H323Capabilities(const H323Connection & connection,
                                   const H245_TerminalCapabilitySet & pdu)
{
  H323Capabilities allCapabilities;
  const H323Capabilities & localCapabilities = connection.GetLocalCapabilities();
  for (PINDEX c = 0; c < localCapabilities.GetSize(); c++)
    allCapabilities.Add(allCapabilities.Copy(localCapabilities[c]));
  allCapabilities.AddAllCapabilities(connection.GetEndPoint(), 0, 0, "*");
  H323_UserInputCapability::AddAllCapabilities(allCapabilities, P_MAX_INDEX, P_MAX_INDEX);

  // Decode out of the PDU, the list of known codecs.
  if (pdu.HasOptionalField(H245_TerminalCapabilitySet::e_capabilityTable)) {
    for (PINDEX i = 0; i < pdu.m_capabilityTable.GetSize(); i++) {
      if (pdu.m_capabilityTable[i].HasOptionalField(H245_CapabilityTableEntry::e_capability)) {
        H323Capability * capability = allCapabilities.FindCapability(pdu.m_capabilityTable[i].m_capability);
        if (capability != NULL) {
          H323Capability * copy = (H323Capability *)capability->Clone();
          copy->SetCapabilityNumber(pdu.m_capabilityTable[i].m_capabilityTableEntryNumber);
          if (copy->OnReceivedPDU(pdu.m_capabilityTable[i].m_capability))
            table.Append(copy);
          else
            delete copy;
        }
      }
    }
  }

  PINDEX outerSize = pdu.m_capabilityDescriptors.GetSize();
  set.SetSize(outerSize);
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    H245_CapabilityDescriptor & desc = pdu.m_capabilityDescriptors[outer];
    if (desc.HasOptionalField(H245_CapabilityDescriptor::e_simultaneousCapabilities)) {
      PINDEX middleSize = desc.m_simultaneousCapabilities.GetSize();
      set[outer].SetSize(middleSize);
      for (PINDEX middle = 0; middle < middleSize; middle++) {
        H245_AlternativeCapabilitySet & alt = desc.m_simultaneousCapabilities[middle];
        for (PINDEX inner = 0; inner < alt.GetSize(); inner++) {
          for (PINDEX cap = 0; cap < table.GetSize(); cap++) {
            if (table[cap].GetCapabilityNumber() == alt[inner]) {
              set[outer][middle].Append(&table[cap]);
              break;
            }
          }
        }
      }
    }
  }
}


H323Capabilities::H323Capabilities(const H323Capabilities & original)
{
  operator=(original);
}


H323Capabilities & H323Capabilities::operator=(const H323Capabilities & original)
{
  RemoveAll();

  for (PINDEX i = 0; i < original.GetSize(); i++)
    Copy(original[i]);

  PINDEX outerSize = original.set.GetSize();
  set.SetSize(outerSize);
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = original.set[outer].GetSize();
    set[outer].SetSize(middleSize);
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = original.set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++)
        set[outer][middle].Append(FindCapability(original.set[outer][middle][inner].GetCapabilityNumber()));
    }
  }

  return *this;
}


void H323Capabilities::PrintOn(ostream & strm) const
{
  int indent = strm.precision()-1;
  strm << setw(indent) << " " << "Table:\n";
  for (PINDEX i = 0; i < table.GetSize(); i++)
    strm << setw(indent+2) << " " << table[i] << '\n';

  strm << setw(indent) << " " << "Set:\n";
  for (PINDEX outer = 0; outer < set.GetSize(); outer++) {
    strm << setw(indent+2) << " " << outer << ":\n";
    for (PINDEX middle = 0; middle < set[outer].GetSize(); middle++) {
      strm << setw(indent+4) << " " << middle << ":\n";
      for (PINDEX inner = 0; inner < set[outer][middle].GetSize(); inner++)
        strm << setw(indent+6) << " " << set[outer][middle][inner] << '\n';
    }
  }
}


PINDEX H323Capabilities::SetCapability(PINDEX descriptorNum,
                                       PINDEX simultaneousNum,
                                       H323Capability * capability)
{
  // Make sure capability has been added to table.
  Add(capability);

  PBoolean newDescriptor = descriptorNum == P_MAX_INDEX;
  if (newDescriptor)
    descriptorNum = set.GetSize();

  // Make sure the outer array is big enough
  set.SetMinSize(descriptorNum+1);

  if (simultaneousNum == P_MAX_INDEX)
    simultaneousNum = set[descriptorNum].GetSize();

  // Make sure the middle array is big enough
  set[descriptorNum].SetMinSize(simultaneousNum+1);

  // Now we can put the new entry in.
  set[descriptorNum][simultaneousNum].Append(capability);
  return newDescriptor ? descriptorNum : simultaneousNum;
}


PINDEX H323Capabilities::SetCapability(PINDEX descriptorNum,
                                       H323Capability * capability)
{
  // Make sure capability has been added to table.
  Add(capability);

  // Search through existing set for simultaneous entries of same media type
  if (descriptorNum < set.GetSize()) {
    for (PINDEX middle = 0; middle < set[descriptorNum].GetSize(); middle++) {
      for (PINDEX inner = 0; inner < set[descriptorNum][middle].GetSize(); inner++) {
        H323Capability * cap = FindCapability(set[descriptorNum][middle][inner]);
        if (cap != NULL) {
          if (cap->GetMediaType() == capability->GetMediaType())
            return SetCapability(descriptorNum, middle, capability);
          break;
        }
      }
    }
  }

  return SetCapability(descriptorNum, P_MAX_INDEX, capability);
}


static PBoolean MatchWildcard(const PCaselessString & str, const PStringArray & wildcard)
{
  PINDEX last = 0;
  for (PINDEX i = 0; i < wildcard.GetSize(); i++) {
    if (wildcard[i].IsEmpty())
      last = str.GetLength();
    else {
      PINDEX next = str.Find(wildcard[i], last);
      if (next == P_MAX_INDEX)
        return PFalse;
      last = next + wildcard[i].GetLength();
    }
  }

  return last == str.GetLength();
}


PINDEX H323Capabilities::AddMediaFormat(PINDEX descriptorNum,
                                        PINDEX simultaneous,
                                        const OpalMediaFormat & mediaFormat)
{
  PINDEX reply = descriptorNum == P_MAX_INDEX ? P_MAX_INDEX : simultaneous;

  if (FindCapability(mediaFormat, H323Capability::e_Unknown, true) == NULL) {
    H323Capability * capability = H323Capability::Create(mediaFormat);
    if (capability != NULL) {
      capability->GetWritableMediaFormat() = mediaFormat;
      reply = SetCapability(descriptorNum, simultaneous, capability);
    }
  }

  return reply;
}


PINDEX H323Capabilities::AddAllCapabilities(PINDEX descriptorNum,
                                            PINDEX simultaneous,
                                            const PString & name,
                                            PBoolean exact)
{
  PINDEX reply = descriptorNum == P_MAX_INDEX ? P_MAX_INDEX : simultaneous;

  PStringArray wildcard = name.Tokenise('*', PFalse);

  H323CapabilityFactory::KeyList_T stdCaps = H323CapabilityFactory::GetKeyList();
  H323CapabilityFactory::KeyList_T::const_iterator r;

  for (r = stdCaps.begin(); r != stdCaps.end(); ++r) {
    PCaselessString capName = *r;
    if ((exact ? (capName == name) : MatchWildcard(capName, wildcard)) &&
        FindCapability(capName, H323Capability::e_Unknown, exact) == NULL) {
      H323Capability * capability = H323Capability::Create(capName);
      PINDEX num = SetCapability(descriptorNum, simultaneous, capability);
      if (descriptorNum == P_MAX_INDEX) {
        reply = num;
        descriptorNum = num;
        simultaneous = P_MAX_INDEX;
      }
      else if (simultaneous == P_MAX_INDEX) {
        if (reply == P_MAX_INDEX)
          reply = num;
        simultaneous = num;
      }
    }
  }

  return reply;
}


static unsigned MergeCapabilityNumber(const H323CapabilitiesList & table,
                                      unsigned newCapabilityNumber)
{
  // Assign a unique number to the codec, check if the user wants a specific
  // value and start with that.
  if (newCapabilityNumber == 0)
    newCapabilityNumber = 1;

  PINDEX i = 0;
  while (i < table.GetSize()) {
    if (table[i].GetCapabilityNumber() != newCapabilityNumber)
      i++;
    else {
      // If it already in use, increment it
      newCapabilityNumber++;
      i = 0;
    }
  }

  return newCapabilityNumber;
}


void H323Capabilities::Add(H323Capability * capability)
{
  // See if already added, confuses things if you add the same instance twice
  if (table.GetObjectsIndex(capability) != P_MAX_INDEX)
    return;

  capability->SetCapabilityNumber(MergeCapabilityNumber(table, 1));
  table.Append(capability);

  PTRACE(3, "H323\tAdded capability: " << *capability);
}


H323Capability * H323Capabilities::Copy(const H323Capability & capability)
{
  H323Capability * newCapability = (H323Capability *)capability.Clone();
  newCapability->SetCapabilityNumber(MergeCapabilityNumber(table, capability.GetCapabilityNumber()));
  table.Append(newCapability);

  PTRACE(3, "H323\tAdded capability: " << *newCapability);
  return newCapability;
}


void H323Capabilities::Remove(H323Capability * capability)
{
  if (capability == NULL)
    return;

  PTRACE(3, "H323\tRemoving capability: " << *capability);

  unsigned capabilityNumber = capability->GetCapabilityNumber();

  for (PINDEX outer = 0; outer < set.GetSize(); outer++) {
    for (PINDEX middle = 0; middle < set[outer].GetSize(); middle++) {
      for (PINDEX inner = 0; inner < set[outer][middle].GetSize(); inner++) {
        if (set[outer][middle][inner].GetCapabilityNumber() == capabilityNumber) {
          set[outer][middle].RemoveAt(inner);
          break;
        }
      }
      if (set[outer][middle].GetSize() == 0)
        set[outer].RemoveAt(middle);
    }
    if (set[outer].GetSize() == 0)
      set.RemoveAt(outer);
  }

  table.Remove(capability);
}


void H323Capabilities::Remove(const PString & codecName)
{
  H323Capability * cap = FindCapability(codecName);
  while (cap != NULL) {
    Remove(cap);
    cap = FindCapability(codecName);
  }
}


void H323Capabilities::Remove(const PStringArray & codecNames)
{
  for (PINDEX i = 0; i < codecNames.GetSize(); i++)
    Remove(codecNames[i]);
}


void H323Capabilities::RemoveAll()
{
  table.RemoveAll();
  set.RemoveAll();
}


H323Capability * H323Capabilities::FindCapability(unsigned capabilityNumber) const
{
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i].GetCapabilityNumber() == capabilityNumber) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  PTRACE(4, "H323\tCould not find capability: " << capabilityNumber);
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const PString & formatName,
                                                  H323Capability::CapabilityDirection direction,
                                                  PBoolean exact) const
{
  PStringArray wildcard = formatName.Tokenise('*', PFalse);

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    PCaselessString str = table[i].GetFormatName();
    if ((exact ? (str == formatName) : MatchWildcard(str, wildcard)) &&
          (direction == H323Capability::e_Unknown ||
           table[i].GetCapabilityDirection() == direction)) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  PTRACE(4, "H323\tCould not find capability: \"" << formatName << '"');
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(
                              H323Capability::CapabilityDirection direction) const
{
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i].GetCapabilityDirection() == direction) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  PTRACE(4, "H323\tCould not find capability: \"" << direction << '"');
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H323Capability & capability) const
{

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i] == capability) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  PTRACE(4, "H323\tCould not find capability: " << capability);
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H245_Capability & cap) const
{
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    switch (cap.GetTag()) {
      case H245_Capability::e_receiveAudioCapability :
      case H245_Capability::e_transmitAudioCapability :
      case H245_Capability::e_receiveAndTransmitAudioCapability :
        if (capability.GetMainType() == H323Capability::e_Audio) {
          const H245_AudioCapability & audio = cap;
          if (capability.IsMatch(audio))
            return &capability;
        }
        break;

      case H245_Capability::e_receiveVideoCapability :
      case H245_Capability::e_transmitVideoCapability :
      case H245_Capability::e_receiveAndTransmitVideoCapability :
        if (capability.GetMainType() == H323Capability::e_Video) {
          const H245_VideoCapability & video = cap;
          if (capability.IsMatch(video))
            return &capability;
        }
        break;

      case H245_Capability::e_receiveDataApplicationCapability :
      case H245_Capability::e_transmitDataApplicationCapability :
      case H245_Capability::e_receiveAndTransmitDataApplicationCapability :
        if (capability.GetMainType() == H323Capability::e_Data) {
          const H245_DataApplicationCapability & data = cap;
          if (capability.IsMatch(data.m_application))
            return &capability;
        }
        break;

      case H245_Capability::e_receiveUserInputCapability :
      case H245_Capability::e_transmitUserInputCapability :
      case H245_Capability::e_receiveAndTransmitUserInputCapability :
        if (capability.GetMainType() == H323Capability::e_UserInput) {
          const H245_UserInputCapability & ui = cap;
          if (capability.IsMatch(ui))
            return &capability;
        }
        break;

      case H245_Capability::e_receiveRTPAudioTelephonyEventCapability :
        return FindCapability(H323Capability::e_UserInput, SignalToneRFC2833_SubType);
    }
  }

#if PTRACING
  if (PTrace::CanTrace(4)) {
    PString tagName;
    switch (cap.GetTag()) {
      case H245_Capability::e_receiveAudioCapability :
      case H245_Capability::e_transmitAudioCapability :
      case H245_Capability::e_receiveAndTransmitAudioCapability :
        tagName = ((const H245_AudioCapability &)cap).GetTagName();
        break;

      case H245_Capability::e_receiveVideoCapability :
      case H245_Capability::e_transmitVideoCapability :
      case H245_Capability::e_receiveAndTransmitVideoCapability :
        tagName = ((const H245_VideoCapability &)cap).GetTagName();
        break;

      case H245_Capability::e_receiveDataApplicationCapability :
      case H245_Capability::e_transmitDataApplicationCapability :
      case H245_Capability::e_receiveAndTransmitDataApplicationCapability :
        tagName = ((const H245_DataApplicationCapability &)cap).m_application.GetTagName();
        break;

      case H245_Capability::e_receiveUserInputCapability :
      case H245_Capability::e_transmitUserInputCapability :
      case H245_Capability::e_receiveAndTransmitUserInputCapability :
        tagName = ((const H245_UserInputCapability &)cap).GetTagName();
        break;

      default :
        tagName = "unknown";
        break;
    }
    PTRACE(4, "H323\tCould not find capability: " << cap.GetTagName() << ", type " << tagName);
  }
#endif
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H245_DataType & dataType) const
{
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    bool checkExact;
    switch (dataType.GetTag()) {
      case H245_DataType::e_audioData :
        checkExact = capability.IsMatch((const H245_AudioCapability &)dataType);
        break;

      case H245_DataType::e_videoData :
        checkExact = capability.IsMatch((const H245_VideoCapability &)dataType);
        break;

      case H245_DataType::e_data :
        checkExact = capability.IsMatch(((const H245_DataApplicationCapability &)dataType).m_application);
        break;

      default :
        checkExact = false;
    }

    if (checkExact) {
      H323Capability * compare = (H323Capability *)capability.Clone();
      if (compare->OnReceivedPDU(dataType, PFalse) && *compare == capability) {
        delete compare;
        PTRACE(3, "H323\tFound capability: " << capability);
        return &capability;
      }
      delete compare;
    }
  }

#if PTRACING
  if (PTrace::CanTrace(4)) {
    PString tagName;
    switch (dataType.GetTag()) {
      case H245_DataType::e_audioData :
        tagName = ((const H245_AudioCapability &)dataType).GetTagName();
        break;

      case H245_DataType::e_videoData :
        tagName = ((const H245_VideoCapability &)dataType).GetTagName();
        break;

      case H245_DataType::e_data :
        tagName = ((const H245_DataApplicationCapability &)dataType).m_application.GetTagName();
        break;

      default :
        tagName = "unknown";
        break;
    }
    PTRACE(4, "H323\tCould not find capability: " << dataType.GetTagName() << ", type " << tagName);
  }
#endif
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H245_ModeElement & modeElement) const
{
  PTRACE(4, "H323\tFindCapability: " << modeElement.m_type.GetTagName());

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    switch (modeElement.m_type.GetTag()) {
      case H245_ModeElementType::e_audioMode :
        if (capability.GetMainType() == H323Capability::e_Audio) {
          const H245_AudioMode & audio = modeElement.m_type;
          if (capability.IsMatch(audio))
            return &capability;
        }
        break;

      case H245_ModeElementType::e_videoMode :
        if (capability.GetMainType() == H323Capability::e_Video) {
          const H245_VideoMode & video = modeElement.m_type;
          if (capability.IsMatch(video))
            return &capability;
        }
        break;

      case H245_ModeElementType::e_dataMode :
        if (capability.GetMainType() == H323Capability::e_Data) {
          const H245_DataMode & data = modeElement.m_type;
          if (capability.IsMatch(data.m_application))
            return &capability;
        }
        break;

      default :
        break;
    }
  }

#if PTRACING
  if (PTrace::CanTrace(4)) {
    PString tagName;
    switch (modeElement.m_type.GetTag()) {
      case H245_ModeElementType::e_audioMode :
        tagName = ((const H245_AudioMode &)modeElement.m_type).GetTagName();
        break;

      case H245_ModeElementType::e_videoMode :
        tagName = ((const H245_VideoMode &)modeElement.m_type).GetTagName();
        break;

      case H245_ModeElementType::e_dataMode :
        tagName = ((const H245_DataMode &)modeElement.m_type).m_application.GetTagName();
        break;

      default :
        tagName = "unknown";
        break;
    }
    PTRACE(4, "H323\tCould not find capability: " << modeElement.m_type.GetTagName() << ", type " << tagName);
  }
#endif
  return NULL;
}


H323Capability * H323Capabilities::FindCapability(H323Capability::MainTypes mainType,
                                                  unsigned subType) const
{
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    if (capability.GetMainType() == mainType &&
                        (subType == UINT_MAX || capability.GetSubType() == subType)) {
      PTRACE(3, "H323\tFound capability: " << capability);
      return &capability;
    }
  }

  PTRACE(4, "H323\tCould not find capability: " << mainType << " subtype=" << subType);
  return NULL;
}


void H323Capabilities::BuildPDU(const H323Connection & connection,
                                H245_TerminalCapabilitySet & pdu) const
{
  PINDEX tableSize = table.GetSize();
  PINDEX setSize = set.GetSize();
  PAssert((tableSize > 0) == (setSize > 0), PLogicError);
  if (tableSize == 0 || setSize == 0)
    return;

  // Set the table of capabilities
  pdu.IncludeOptionalField(H245_TerminalCapabilitySet::e_capabilityTable);

  H245_H2250Capability & h225_0 = pdu.m_multiplexCapability;
  PINDEX rtpPacketizationCount = 0;

  // encode the capabilities
  PINDEX count = 0;
  PINDEX i;
  for (i = 0; i < tableSize; i++) {
    H323Capability & capability = table[i];
    if (capability.IsUsable(connection)) {
      pdu.m_capabilityTable.SetSize(count+1);
      H245_CapabilityTableEntry & entry = pdu.m_capabilityTable[count++];
      entry.m_capabilityTableEntryNumber = capability.GetCapabilityNumber();
      entry.IncludeOptionalField(H245_CapabilityTableEntry::e_capability);
      capability.OnSendingPDU(entry.m_capability);

      h225_0.m_mediaPacketizationCapability.m_rtpPayloadType.SetSize(rtpPacketizationCount+1);
      if (H323SetRTPPacketization(h225_0.m_mediaPacketizationCapability.m_rtpPayloadType[rtpPacketizationCount],
                                                    capability.GetMediaFormat(), RTP_DataFrame::IllegalPayloadType)) {
        // Check if already in list
        PINDEX test;
        for (test = 0; test < rtpPacketizationCount; test++) {
          if (h225_0.m_mediaPacketizationCapability.m_rtpPayloadType[test] == h225_0.m_mediaPacketizationCapability.m_rtpPayloadType[rtpPacketizationCount])
            break;
        }
        if (test == rtpPacketizationCount)
          rtpPacketizationCount++;
      }
    }
  }

  // Have some mediaPacketizations to include.
  if (rtpPacketizationCount > 0) {
    h225_0.m_mediaPacketizationCapability.m_rtpPayloadType.SetSize(rtpPacketizationCount);
    h225_0.m_mediaPacketizationCapability.IncludeOptionalField(H245_MediaPacketizationCapability::e_rtpPayloadType);
  }

  // Set the sets of compatible capabilities
  pdu.IncludeOptionalField(H245_TerminalCapabilitySet::e_capabilityDescriptors);

  pdu.m_capabilityDescriptors.SetSize(setSize);
  for (PINDEX outer = 0; outer < setSize; outer++) {
    H245_CapabilityDescriptor & desc = pdu.m_capabilityDescriptors[outer];
    desc.m_capabilityDescriptorNumber = (unsigned)(outer + 1);
    desc.IncludeOptionalField(H245_CapabilityDescriptor::e_simultaneousCapabilities);
    PINDEX middleSize = set[outer].GetSize();
    desc.m_simultaneousCapabilities.SetSize(middleSize);
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      H245_AlternativeCapabilitySet & alt = desc.m_simultaneousCapabilities[middle];
      PINDEX innerSize = set[outer][middle].GetSize();
      alt.SetSize(innerSize);
      count = 0;
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        H323Capability & capability = set[outer][middle][inner];
        if (capability.IsUsable(connection)) {
          alt.SetSize(count+1);
          alt[count++] = capability.GetCapabilityNumber();
        }
      }
    }
  }
}


PBoolean H323Capabilities::Merge(const H323Capabilities & newCaps)
{
  PTRACE_IF(4, !table.IsEmpty(), "H245\tCapability merge of:\n" << newCaps << "\nInto:\n" << *this);

  // Add any new capabilities not already in set.
  PINDEX i;
  for (i = 0; i < newCaps.GetSize(); i++) {
    // Only add if not already in
    if (!FindCapability(newCaps[i]))
      Copy(newCaps[i]);
  }

  // This should merge instead of just adding to it.
  PINDEX outerSize = newCaps.set.GetSize();
  PINDEX outerBase = set.GetSize();
  set.SetSize(outerBase+outerSize);
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = newCaps.set[outer].GetSize();
    set[outerBase+outer].SetSize(middleSize);
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = newCaps.set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        H323Capability * cap = FindCapability(newCaps.set[outer][middle][inner].GetCapabilityNumber());
        if (cap != NULL)
          set[outerBase+outer][middle].Append(cap);
      }
    }
  }

  PTRACE_IF(4, !table.IsEmpty(), "H245\tCapability merge result:\n" << *this);
  PTRACE(3, "H245\tReceived capability set, is " << (table.IsEmpty() ? "rejected" : "accepted"));
  return !table.IsEmpty();
}


void H323Capabilities::Reorder(const PStringArray & preferenceOrder)
{
  if (preferenceOrder.IsEmpty())
    return;

  table.DisallowDeleteObjects();

  PINDEX preference = 0;
  PINDEX base = 0;

  for (preference = 0; preference < preferenceOrder.GetSize(); preference++) {
    PStringArray wildcard = preferenceOrder[preference].Tokenise('*', PFalse);
    for (PINDEX idx = base; idx < table.GetSize(); idx++) {
      PCaselessString str = table[idx].GetFormatName();
      if (MatchWildcard(str, wildcard)) {
        if (idx != base)
          table.InsertAt(base, table.RemoveAt(idx));
        base++;
      }
    }
  }

  for (PINDEX outer = 0; outer < set.GetSize(); outer++) {
    for (PINDEX middle = 0; middle < set[outer].GetSize(); middle++) {
      H323CapabilitiesList & list = set[outer][middle];
      for (PINDEX idx = 0; idx < table.GetSize(); idx++) {
        for (PINDEX inner = 0; inner < list.GetSize(); inner++) {
          if (&table[idx] == &list[inner]) {
            list.Append(list.RemoveAt(inner));
            break;
          }
        }
      }
    }
  }

  table.AllowDeleteObjects();
}


PBoolean H323Capabilities::IsAllowed(const H323Capability & capability)
{
  return IsAllowed(capability.GetCapabilityNumber());
}


PBoolean H323Capabilities::IsAllowed(const unsigned a_capno)
{
  // Check that capno is actually in the set
  PINDEX outerSize = set.GetSize();
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = set[outer].GetSize();
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        if (a_capno == set[outer][middle][inner].GetCapabilityNumber()) {
          return PTrue;
        }
      }
    }
  }
  return PFalse;
}


PBoolean H323Capabilities::IsAllowed(const H323Capability & capability1,
                                 const H323Capability & capability2)
{
  return IsAllowed(capability1.GetCapabilityNumber(),
                   capability2.GetCapabilityNumber());
}


PBoolean H323Capabilities::IsAllowed(const unsigned a_capno1, const unsigned a_capno2)
{
  if (a_capno1 == a_capno2) {
    PTRACE(2, "H323\tH323Capabilities::IsAllowed() capabilities are the same.");
    return PTrue;
  }

  PINDEX outerSize = set.GetSize();
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = set[outer].GetSize();
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        if (a_capno1 == set[outer][middle][inner].GetCapabilityNumber()) {
          /* Now go searching for the other half... */
          for (PINDEX middle2 = 0; middle2 < middleSize; ++middle2) {
            if (middle != middle2) {
              PINDEX innerSize2 = set[outer][middle2].GetSize();
              for (PINDEX inner2 = 0; inner2 < innerSize2; ++inner2) {
                if (a_capno2 == set[outer][middle2][inner2].GetCapabilityNumber()) {
                  return PTrue;
                }
              }
            }
          }
        }
      }
    }
  }
  return PFalse;
}


OpalMediaFormatList H323Capabilities::GetMediaFormats() const
{
  OpalMediaFormatList formats;

  for (PINDEX i = 0; i < table.GetSize(); i++)
    formats += table[i].GetMediaFormat();

  return formats;
}


/////////////////////////////////////////////////////////////////////////////

#ifndef PASN_NOPRINTON


struct msNonStandardCodecDef {
  const char * name;
  BYTE sig[2];
};


static msNonStandardCodecDef msNonStandardCodec[] = {
  { "L&H CELP 4.8k", { 0x01, 0x11 } },
  { "ADPCM",         { 0x02, 0x00 } },
  { "L&H CELP 8k",   { 0x02, 0x11 } },
  { "L&H CELP 12k",  { 0x03, 0x11 } },
  { "L&H CELP 16k",  { 0x04, 0x11 } },
  { "IMA-ADPCM",     { 0x11, 0x00 } },
  { "GSM",           { 0x31, 0x00 } },
  { NULL,            { 0,    0    } }
};

void H245_AudioCapability::PrintOn(ostream & strm) const
{
  strm << GetTagName();

  // tag 0 is nonstandard
  if (tag == 0) {

    H245_NonStandardParameter & param = (H245_NonStandardParameter &)GetObject();
    const PBYTEArray & data = param.m_data;

    switch (param.m_nonStandardIdentifier.GetTag()) {
      case H245_NonStandardIdentifier::e_h221NonStandard:
        {
          H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;

          // Microsoft is 181/0/21324
          if ((h221.m_t35CountryCode   == 181) &&
              (h221.m_t35Extension     == 0) &&
              (h221.m_manufacturerCode == 21324)
            ) {
            PString name = "Unknown";
            PINDEX i;
            if (data.GetSize() >= 21) {
              for (i = 0; msNonStandardCodec[i].name != NULL; i++) {
                if ((data[20] == msNonStandardCodec[i].sig[0]) && 
                    (data[21] == msNonStandardCodec[i].sig[1])) {
                  name = msNonStandardCodec[i].name;
                  break;
                }
              }
            }
            strm << (PString(" [Microsoft") & name) << "]";
          }

          // Equivalence is 9/0/61
          else if ((h221.m_t35CountryCode   == 9) &&
                   (h221.m_t35Extension     == 0) &&
                   (h221.m_manufacturerCode == 61)
                  ) {
            PString name;
            if (data.GetSize() > 0)
              name = PString((const char *)(const BYTE *)data, data.GetSize());
            strm << " [Equivalence " << name << "]";
          }

          // Xiph is 181/0/38
          else if ((h221.m_t35CountryCode   == 181) &&
                   (h221.m_t35Extension     == 0) &&
                   (h221.m_manufacturerCode == 38)
                  ) {
            PString name;
            if (data.GetSize() > 0)
              name = PString((const char *)(const BYTE *)data, data.GetSize());
            strm << " [Xiph " << name << "]";
          }

          // Cisco is 181/0/18
          else if ((h221.m_t35CountryCode   == 181) &&
                   (h221.m_t35Extension     == 0) &&
                   (h221.m_manufacturerCode == 18)
                  ) {
            PString name;
            if (data.GetSize() > 0)
              name = PString((const char *)(const BYTE *)data, data.GetSize());
            strm << " [Cisco " << name << "]";
          }

        }
        break;
      default:
        break;
    }
  }

  if (choice == NULL)
    strm << " (NULL)";
  else {
    strm << ' ' << *choice;
  }

  //PASN_Choice::PrintOn(strm);
}

#endif // PASN_NOPRINTON

#endif // OPAL_H323
