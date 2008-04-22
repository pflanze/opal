/*
 * t38proto.cxx
 *
 * T.38 protocol handler
 *
 * Open Phone Abstraction Library
 *
 * Copyright (c) 1998-2002 Equivalence Pty. Ltd.
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
 * Contributor(s): Vyacheslav Frolov.
 *
 * $Revision: 20036 $
 * $Author: csoutheren $
 * $Date: 2008-04-21 17:21:24 +1000 (Mon, 21 Apr 2008) $
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "t38proto.h"
#endif

#include <opal/buildopts.h>

#include <t38/t38proto.h>
#include <t38/sipt38.h>

#if OPAL_SIP

/////////////////////////////////////////////////////////////////////////////

SDPMediaDescription * OpalFaxMediaType::CreateSDPMediaDescription(const OpalTransportAddress & localAddress)
{
  return new SDPFaxMediaDescription(localAddress);
}

/////////////////////////////////////////////////////////////////////////////

SDPFaxMediaDescription::SDPFaxMediaDescription(const OpalTransportAddress & address)
  : SDPMediaDescription(address)
{
  t38Attributes.SetAt("T38FaxRateManagement", "transferredTCF");
  t38Attributes.SetAt("T38FaxVersion",        "0");
}

PCaselessString SDPFaxMediaDescription::GetSDPTransportType() const
{ 
  return "udptl";
; 
}

PString SDPFaxMediaDescription::GetSDPMediaType() const 
{ 
  return "image"; 
}

SDPMediaFormat * SDPFaxMediaDescription::CreateSDPMediaFormat(const PString & portString)
{
  return new SDPMediaFormat(RTP_DataFrame::DynamicBase, portString);
}


PString SDPFaxMediaDescription::GetSDPPortList() const
{
  PStringStream str;

  // output encoding names for non RTP
  SDPMediaFormatList::const_iterator format;
  for (format = formats.begin(); format != formats.end(); ++format)
    str << ' ' << format->GetEncodingName();

  return str;
}

bool SDPFaxMediaDescription::PrintOn(ostream & str, const PString & connectString) const
{
  // call ancestor
  if (!SDPMediaDescription::PrintOn(str, connectString))
    return false;

  // output options
  for (PINDEX i = 0; i < t38Attributes.GetSize(); i++) 
    str << "a=" << t38Attributes.GetKeyAt(i) << ":" << t38Attributes.GetDataAt(i) << "\r\n";

  return true;
}

void SDPFaxMediaDescription::SetAttribute(const PString & attr, const PString & value)
{
  if (attr.Left(3) *= "t38") {
    t38Attributes.SetAt(attr, value);
    return;
  }

  return SDPMediaDescription::SetAttribute(attr, value);
}

void SDPFaxMediaDescription::ProcessMediaOptions(SDPMediaFormat & /*sdpFormat*/, const OpalMediaFormat & mediaFormat)
{
  if (mediaFormat.GetDefaultSessionID() == OpalMediaFormat::DefaultDataSessionID) {
    for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); ++i) {
      const OpalMediaOption & option = mediaFormat.GetOption(i);
      if (option.GetName().Left(3) *= "t38") 
        t38Attributes.SetAt(option.GetName(), option.AsString());
    }
  }
}

/////////////////////////////////////////////////////////////////////////////

#endif // OPAL_SIP
