/*
 *
 * Inter Asterisk Exchange 2
 * 
 * Class to implement a thread safe list of sound packets.
 * 
 * Open Phone Abstraction Library (OPAL)
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
 *
 *
 * $Log: sound.cxx,v $
 * Revision 1.2  2005/08/26 03:07:38  dereksmithies
 * Change naming convention, so all class names contain the string "IAX2"
 *
 * Revision 1.1  2005/07/30 07:01:33  csoutheren
 * Added implementation of IAX2 (Inter Asterisk Exchange 2) protocol
 * Thanks to Derek Smithies of Indranet Technologies Ltd. for
 * writing and contributing this code
 *
 *
 *
 */

#include <ptlib.h>

#ifdef P_USE_PRAGMA
#pragma implementation "sound.h"
#endif


#include <iax2/iax2con.h>
#include <iax2/iax2ep.h>
#include <iax2/frame.h>
#include <iax2/sound.h>

#define new PNEW  
////////////////////////////////////////////////////////////////////////////////
IAX2SoundList::~IAX2SoundList()
{ 
  AllowDeleteObjects();
}



PBYTEArray * IAX2SoundList::GetLastEntry() 
{
  PWaitAndSignal m(mutex);
  
  PINDEX size = PAbstractList::GetSize();
  
  if (size == 0)
    return NULL;
  
  return (PBYTEArray *) RemoveAt(size - 1);
}

void IAX2SoundList::AddNewEntry(PBYTEArray *newElem)
{
  PWaitAndSignal m(mutex);
  
  InsertAt(0, newElem);
}

void IAX2SoundList::GetAllDeleteAll(IAX2SoundList &dest)
{
  PWaitAndSignal m(mutex);
  while(PAbstractList::GetSize() > 0) {
    dest.AddNewEntry((PBYTEArray *)RemoveAt(0));
  }
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/* The comment below is magic for those who use emacs to edit this file. */
/* With the comment below, the tab key does auto indent to 4 spaces.     */

/*
 * Local Variables:
 * mode:c
 * c-file-style:linux
 * c-basic-offset:2
 * End:
 */
