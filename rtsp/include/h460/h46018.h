//
// h46018.h
//
// Code automatically generated by asnparse.
//

#ifndef __H46018_H
#define __H46018_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <opal/buildopts.h>

#if OPAL_H460

#include <ptclib/asner.h>

#include <asn/h225.h>


//
// IncomingCallIndication
//

class H46018_IncomingCallIndication : public PASN_Sequence
{
#ifndef PASN_LEANANDMEAN
    PCLASSINFO(H46018_IncomingCallIndication, PASN_Sequence);
#endif
  public:
    H46018_IncomingCallIndication(unsigned tag = UniversalSequence, TagClass tagClass = UniversalTagClass);

    H225_TransportAddress m_callSignallingAddress;
    H225_CallIdentifier m_callID;

    PINDEX GetDataLength() const;
    PBoolean Decode(PASN_Stream & strm);
    void Encode(PASN_Stream & strm) const;
#ifndef PASN_NOPRINTON
    void PrintOn(ostream & strm) const;
#endif
    Comparison Compare(const PObject & obj) const;
    PObject * Clone() const;
};


//
// LRQKeepAliveData
//

class H46018_LRQKeepAliveData : public PASN_Sequence
{
#ifndef PASN_LEANANDMEAN
    PCLASSINFO(H46018_LRQKeepAliveData, PASN_Sequence);
#endif
  public:
    H46018_LRQKeepAliveData(unsigned tag = UniversalSequence, TagClass tagClass = UniversalTagClass);

    H225_TimeToLive m_lrqKeepAliveInterval;

    PINDEX GetDataLength() const;
    PBoolean Decode(PASN_Stream & strm);
    void Encode(PASN_Stream & strm) const;
#ifndef PASN_NOPRINTON
    void PrintOn(ostream & strm) const;
#endif
    Comparison Compare(const PObject & obj) const;
    PObject * Clone() const;
};


#endif // OPAL_H460

#endif // __H46018_H


// End of h46018.h
