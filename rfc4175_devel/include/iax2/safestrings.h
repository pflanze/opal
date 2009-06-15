/*
 *
 *
 * Inter Asterisk Exchange 2
 * 
 * Provide string list handling in a thread safe fashion.
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
 *  $Log: safestrings.h,v $
 *  Revision 1.4  2007/04/19 06:17:21  csoutheren
 *  Fixes for precompiled headers with gcc
 *
 *  Revision 1.3  2007/01/17 22:27:52  dereksmithies
 *  Correctly sends DTMF to remote node. Tidy up string handling.
 *
 *  Revision 1.2  2005/08/24 04:56:25  dereksmithies
 *  Add code from Adrian Sietsma to send FullFrameTexts and FullFrameDtmfs to
 *  the remote end.  Many Thanks.
 *
 *  Revision 1.1  2005/07/30 07:01:32  csoutheren
 *  Added implementation of IAX2 (Inter Asterisk Exchange 2) protocol
 *  Thanks to Derek Smithies of Indranet Technologies Ltd. for
 *  writing and contributing this code
 *
 *
 *
 *
 */

#ifndef SAFESTRINGS_H
#define SAFESTRINGS_H

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#ifdef P_USE_PRAGMA
#pragma interface
#endif


/**This class is used to manage a list of strings in a thread safe fashion.
   
This class is thread safe.
*/
class SafeStrings :  public PObject
{
  PCLASSINFO(SafeStrings, PObject);
 public:
  /**@name Construction/Destruction */
  //@{
  /**Construct SafeStrings class
   */
  SafeStrings();
  
  /**Destructor*/
  ~SafeStrings();
  //@}
  
  /**@name General Methods*/
  //@{
  
  /**Add a new string to the list, in a thread safe fashion. */
  void AppendString(const PString & newString,        /*!<String to add to the list.    */
		    BOOL splitString = FALSE   /*!<If True, the string is added to the list, character by character.    */
		    );
  
  /**Add a new string to the list, in a thread safe fashion. */
  void AppendString(const char *newString,     /*!<String to add to the list.    */
		    BOOL splitString = FALSE   /*!<If True, the string is added to the list, character by character.    */
		    ) { PString s(newString); AppendString(s, splitString); }
  
  /**Remove the last string from this list, in a thread safe fashion. Return TRUE if succesfull*/
  BOOL GetNextString(PString & nextString /*!< resultant string.    */
		     );
  
  /** Return True if this list is empty */
  BOOL IsEmpty();

  /** Return True if there is data ready to be read from the list */
  BOOL StringsAvailable() { return !IsEmpty(); }
  
  /** Return the first string on list, and then delete all elements on the list */
  PString GetFirstDeleteAll();
  
  /** Return the contents of this string array  */
  void GetAllDeleteAll(PStringArray & res);
  
  //@}
 protected:
  /**Lock on this string array*/
  PMutex accessMutex;
  
  /**Internal String array */
  PStringArray data;
};

////////////////////////////////////////////////////////////////////////////////
/** A class to handle thread safe access to a PString */
class SafeString : public PObject
{
  PCLASSINFO(SafeString, PObject);
 public:
  /**Construct this class with an empty internal value */
  SafeString() { internal = PString::Empty(); }
  
  /**Construct this class with the internal value set to something */
  SafeString(PString newValue) { internal = newValue; }
  
  /**Assign a new value to the internal variable*/
  void operator = (PString newValue);
  
  /**Retrieve the value of the internal variable */
  PString Get() { PWaitAndSignal m(mutex); return internal; }
  
  /**print the internal string to the designated stream*/     
  virtual void PrintOn(ostream & str) const;
  
  /**Retrive the value of the internal variable as a string */
  operator PString();
  
  /**Add a new text to the string, in a thread safe fashion. */
  void operator += (PString toBeAdded);

  /**Retrieve and clear the value of the internal variable */
  PString GetAndDelete();

  /** Return True if this list is empty */
  BOOL IsEmpty() const;


 protected:
  /**The internal variable which is accessed in a thread safe fashion.*/
  PString internal;
  
  /**The lock, which is used to safeguard access to this variable */
  PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////

#endif // SAFESTRINGS_H
/* The comment below is magic for those who use emacs to edit this file. */
/* With the comment below, the tab key does auto indent to 4 spaces.     */

/*
 * Local Variables:
 * mode:c
 * c-file-style:linux
 * c-basic-offset:2
 * End:
 */
