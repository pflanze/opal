/*
 * h323h224.h
 *
 * H.323 H.224 logical channel establishment implementation for the 
 * OpenH323 Project.
 *
 * Copyright (c) 2006 Network for Educational Technology, ETH Zurich.
 * Written by Hannes Friederich.
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
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#ifndef OPAL_H224_H323H224_H
#define OPAL_H224_H323H224_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <opal/buildopts.h>

#include <h323/h323caps.h>

#include <h224/h224.h>
#include <h224/h224handler.h>

#define OPAL_H224_CAPABILITY_NAME "H.224"

/** This class describes the H.224 capability
 */
class H323_H224Capability : public H323DataCapability
{
  PCLASSINFO(H323_H224Capability, H323DataCapability);
	
public:
	
  H323_H224Capability();
  ~H323_H224Capability();
	
  Comparison Compare(const PObject & obj) const;
	
  virtual PObject * Clone() const;
	
  virtual unsigned GetSubType() const;
	
  virtual PString GetFormatName() const;
	
  virtual H323Channel * CreateChannel(H323Connection & connection,
									  H323Channel::Directions dir,
									  unsigned sesionID,
									  const H245_H2250LogicalChannelParameters * param) const;
	
  virtual PBoolean OnSendingPDU(H245_DataApplicationCapability & pdu) const;
  virtual PBoolean OnSendingPDU(H245_DataMode & pdu) const;
  virtual PBoolean OnReceivedPDU(const H245_DataApplicationCapability & pdu);
	
};

#define OPAL_REGISTER_H224_CAPABILITY() \
  H323_REGISTER_CAPABILITY(H323_H224Capability, GetOpalH224().GetName()); \

#endif // OPAL_H224_H323H224_H
