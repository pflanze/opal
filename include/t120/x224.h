/*
 * x224.h
 *
 * X.224 protocol handler
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
 * $Log: x224.h,v $
 * Revision 1.2001  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.7  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.6  2000/05/18 11:53:35  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.5  2000/05/02 04:32:25  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.4  1999/08/31 13:30:20  robertj
 * Added gatekeeper support.
 *
 * Revision 1.3  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 * Revision 1.2  1999/01/16 11:31:47  robertj
 * Fixed name in header comment.
 *
 * Revision 1.1  1998/12/14 09:13:51  robertj
 * Initial revision
 *
 */

#ifndef __T120_X224_H
#define __T120_X224_H

#ifdef __GNUC__
#pragma interface
#endif


#include <ptlib/sockets.h>


///////////////////////////////////////////////////////////////////////////////

/**This class embodies X.224 Class Zero Protocol Data Unit.
  */
class X224 : public PObject
{
  PCLASSINFO(X224, PObject)

  public:
    enum Codes {
      ConnectRequest = 0xe0,
      ConnectConfirm = 0xd0,
      DataPDU = 0xf0
    };

    X224();

    void BuildConnectRequest();
    void BuildConnectConfirm();
    void BuildData(const PBYTEArray & data);

    void PrintOn(ostream & strm) const;
    BOOL Decode(const PBYTEArray & rawData);
    BOOL Encode(PBYTEArray & rawData) const;

    int GetCode() const { return header[0]; }
    const PBYTEArray & GetData() const { return data; }

  private:
    PBYTEArray header;
    PBYTEArray data;
};


#endif // __T120_X224_H


/////////////////////////////////////////////////////////////////////////////
