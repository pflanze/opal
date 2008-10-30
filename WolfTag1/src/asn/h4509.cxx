//
// h4509.cxx
//
// Code automatically generated by asnparse.
//

#ifdef P_USE_PRAGMA
#pragma implementation "h4509.h"
#endif

#include <ptlib.h>
#include "asn/h4509.h"

#define new PNEW


#if ! H323_DISABLE_H4509



#ifndef PASN_NOPRINTON
const static PASN_Names Names_H4509_H323CallCompletionOperations[]={
        {"ccbsRequest",40}
       ,{"ccnrRequest",27}
       ,{"ccCancel",28}
       ,{"ccExecPossible",29}
       ,{"ccRingout",31}
       ,{"ccSuspend",32}
       ,{"ccResume",33}
};
#endif
//
// H323CallCompletionOperations
//

H4509_H323CallCompletionOperations::H4509_H323CallCompletionOperations(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Enumeration(tag, tagClass, 40, PFalse
#ifndef PASN_NOPRINTON
    ,(const PASN_Names *)Names_H4509_H323CallCompletionOperations,7
#endif
    )
{
}


H4509_H323CallCompletionOperations & H4509_H323CallCompletionOperations::operator=(unsigned v)
{
  SetValue(v);
  return *this;
}


PObject * H4509_H323CallCompletionOperations::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_H323CallCompletionOperations::Class()), PInvalidCast);
#endif
  return new H4509_H323CallCompletionOperations(*this);
}



#ifndef PASN_NOPRINTON
const static PASN_Names Names_H4509_CcArg[]={
      {"shortArg",0}
     ,{"longArg",1}
};
#endif
//
// CcArg
//

H4509_CcArg::H4509_CcArg(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Choice(tag, tagClass, 2, PTrue
#ifndef PASN_NOPRINTON
    ,(const PASN_Names *)Names_H4509_CcArg,2
#endif
)
{
}


#if defined(__GNUC__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 9
H4509_CcArg::operator H4509_CcShortArg &() const
#else
H4509_CcArg::operator H4509_CcShortArg &()
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(PAssertNULL(choice), H4509_CcShortArg), PInvalidCast);
#endif
  return *(H4509_CcShortArg *)choice;
}


H4509_CcArg::operator const H4509_CcShortArg &() const
#endif
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(PAssertNULL(choice), H4509_CcShortArg), PInvalidCast);
#endif
  return *(H4509_CcShortArg *)choice;
}


#if defined(__GNUC__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 9
H4509_CcArg::operator H4509_CcLongArg &() const
#else
H4509_CcArg::operator H4509_CcLongArg &()
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(PAssertNULL(choice), H4509_CcLongArg), PInvalidCast);
#endif
  return *(H4509_CcLongArg *)choice;
}


H4509_CcArg::operator const H4509_CcLongArg &() const
#endif
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(PAssertNULL(choice), H4509_CcLongArg), PInvalidCast);
#endif
  return *(H4509_CcLongArg *)choice;
}


PBoolean H4509_CcArg::CreateObject()
{
  switch (tag) {
    case e_shortArg :
      choice = new H4509_CcShortArg();
      return PTrue;
    case e_longArg :
      choice = new H4509_CcLongArg();
      return PTrue;
  }

  choice = NULL;
  return PFalse;
}


PObject * H4509_CcArg::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_CcArg::Class()), PInvalidCast);
#endif
  return new H4509_CcArg(*this);
}


#ifndef PASN_NOPRINTON
const static PASN_Names Names_H4509_CallCompletionErrors[]={
        {"shortTermRejection",1010}
       ,{"longTermRejection",1011}
       ,{"remoteUserBusyAgain",1012}
       ,{"failureToMatch",1013}
};
#endif
//
// CallCompletionErrors
//

H4509_CallCompletionErrors::H4509_CallCompletionErrors(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Enumeration(tag, tagClass, 1013, PFalse
#ifndef PASN_NOPRINTON
    ,(const PASN_Names *)Names_H4509_CallCompletionErrors,4
#endif
    )
{
}


H4509_CallCompletionErrors & H4509_CallCompletionErrors::operator=(unsigned v)
{
  SetValue(v);
  return *this;
}


PObject * H4509_CallCompletionErrors::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_CallCompletionErrors::Class()), PInvalidCast);
#endif
  return new H4509_CallCompletionErrors(*this);
}


//
// ArrayOf_MixedExtension
//

H4509_ArrayOf_MixedExtension::H4509_ArrayOf_MixedExtension(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Array(tag, tagClass)
{
}


PASN_Object * H4509_ArrayOf_MixedExtension::CreateObject() const
{
  return new H4504_MixedExtension;
}


H4504_MixedExtension & H4509_ArrayOf_MixedExtension::operator[](PINDEX i) const
{
  return (H4504_MixedExtension &)array[i];
}


PObject * H4509_ArrayOf_MixedExtension::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_ArrayOf_MixedExtension::Class()), PInvalidCast);
#endif
  return new H4509_ArrayOf_MixedExtension(*this);
}


//
// CcRequestArg
//

H4509_CcRequestArg::H4509_CcRequestArg(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Sequence(tag, tagClass, 3, PTrue, 0)
{
  m_extension.SetConstraints(PASN_Object::FixedConstraint, 0, 255);
}


#ifndef PASN_NOPRINTON
void H4509_CcRequestArg::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << "{\n";
  strm << setw(indent+10) << "numberA = " << setprecision(indent) << m_numberA << '\n';
  strm << setw(indent+10) << "numberB = " << setprecision(indent) << m_numberB << '\n';
  if (HasOptionalField(e_ccIdentifier))
    strm << setw(indent+15) << "ccIdentifier = " << setprecision(indent) << m_ccIdentifier << '\n';
  strm << setw(indent+10) << "service = " << setprecision(indent) << m_service << '\n';
  strm << setw(indent+21) << "can_retain_service = " << setprecision(indent) << m_can_retain_service << '\n';
  if (HasOptionalField(e_retain_sig_connection))
    strm << setw(indent+24) << "retain_sig_connection = " << setprecision(indent) << m_retain_sig_connection << '\n';
  if (HasOptionalField(e_extension))
    strm << setw(indent+12) << "extension = " << setprecision(indent) << m_extension << '\n';
  strm << setw(indent-1) << setprecision(indent-2) << "}";
}
#endif


PObject::Comparison H4509_CcRequestArg::Compare(const PObject & obj) const
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(&obj, H4509_CcRequestArg), PInvalidCast);
#endif
  const H4509_CcRequestArg & other = (const H4509_CcRequestArg &)obj;

  Comparison result;

  if ((result = m_numberA.Compare(other.m_numberA)) != EqualTo)
    return result;
  if ((result = m_numberB.Compare(other.m_numberB)) != EqualTo)
    return result;
  if ((result = m_ccIdentifier.Compare(other.m_ccIdentifier)) != EqualTo)
    return result;
  if ((result = m_service.Compare(other.m_service)) != EqualTo)
    return result;
  if ((result = m_can_retain_service.Compare(other.m_can_retain_service)) != EqualTo)
    return result;
  if ((result = m_retain_sig_connection.Compare(other.m_retain_sig_connection)) != EqualTo)
    return result;
  if ((result = m_extension.Compare(other.m_extension)) != EqualTo)
    return result;

  return PASN_Sequence::Compare(other);
}


PINDEX H4509_CcRequestArg::GetDataLength() const
{
  PINDEX length = 0;
  length += m_numberA.GetObjectLength();
  length += m_numberB.GetObjectLength();
  if (HasOptionalField(e_ccIdentifier))
    length += m_ccIdentifier.GetObjectLength();
  length += m_service.GetObjectLength();
  length += m_can_retain_service.GetObjectLength();
  if (HasOptionalField(e_retain_sig_connection))
    length += m_retain_sig_connection.GetObjectLength();
  if (HasOptionalField(e_extension))
    length += m_extension.GetObjectLength();
  return length;
}


PBoolean H4509_CcRequestArg::Decode(PASN_Stream & strm)
{
  if (!PreambleDecode(strm))
    return PFalse;

  if (!m_numberA.Decode(strm))
    return PFalse;
  if (!m_numberB.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_ccIdentifier) && !m_ccIdentifier.Decode(strm))
    return PFalse;
  if (!m_service.Decode(strm))
    return PFalse;
  if (!m_can_retain_service.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_retain_sig_connection) && !m_retain_sig_connection.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_extension) && !m_extension.Decode(strm))
    return PFalse;

  return UnknownExtensionsDecode(strm);
}


void H4509_CcRequestArg::Encode(PASN_Stream & strm) const
{
  PreambleEncode(strm);

  m_numberA.Encode(strm);
  m_numberB.Encode(strm);
  if (HasOptionalField(e_ccIdentifier))
    m_ccIdentifier.Encode(strm);
  m_service.Encode(strm);
  m_can_retain_service.Encode(strm);
  if (HasOptionalField(e_retain_sig_connection))
    m_retain_sig_connection.Encode(strm);
  if (HasOptionalField(e_extension))
    m_extension.Encode(strm);

  UnknownExtensionsEncode(strm);
}


PObject * H4509_CcRequestArg::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_CcRequestArg::Class()), PInvalidCast);
#endif
  return new H4509_CcRequestArg(*this);
}


//
// CcRequestRes
//

H4509_CcRequestRes::H4509_CcRequestRes(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Sequence(tag, tagClass, 1, PTrue, 0)
{
  m_extension.SetConstraints(PASN_Object::FixedConstraint, 0, 255);
}


#ifndef PASN_NOPRINTON
void H4509_CcRequestRes::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << "{\n";
  strm << setw(indent+17) << "retain_service = " << setprecision(indent) << m_retain_service << '\n';
  if (HasOptionalField(e_extension))
    strm << setw(indent+12) << "extension = " << setprecision(indent) << m_extension << '\n';
  strm << setw(indent-1) << setprecision(indent-2) << "}";
}
#endif


PObject::Comparison H4509_CcRequestRes::Compare(const PObject & obj) const
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(&obj, H4509_CcRequestRes), PInvalidCast);
#endif
  const H4509_CcRequestRes & other = (const H4509_CcRequestRes &)obj;

  Comparison result;

  if ((result = m_retain_service.Compare(other.m_retain_service)) != EqualTo)
    return result;
  if ((result = m_extension.Compare(other.m_extension)) != EqualTo)
    return result;

  return PASN_Sequence::Compare(other);
}


PINDEX H4509_CcRequestRes::GetDataLength() const
{
  PINDEX length = 0;
  length += m_retain_service.GetObjectLength();
  if (HasOptionalField(e_extension))
    length += m_extension.GetObjectLength();
  return length;
}


PBoolean H4509_CcRequestRes::Decode(PASN_Stream & strm)
{
  if (!PreambleDecode(strm))
    return PFalse;

  if (!m_retain_service.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_extension) && !m_extension.Decode(strm))
    return PFalse;

  return UnknownExtensionsDecode(strm);
}


void H4509_CcRequestRes::Encode(PASN_Stream & strm) const
{
  PreambleEncode(strm);

  m_retain_service.Encode(strm);
  if (HasOptionalField(e_extension))
    m_extension.Encode(strm);

  UnknownExtensionsEncode(strm);
}


PObject * H4509_CcRequestRes::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_CcRequestRes::Class()), PInvalidCast);
#endif
  return new H4509_CcRequestRes(*this);
}


//
// CcShortArg
//

H4509_CcShortArg::H4509_CcShortArg(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Sequence(tag, tagClass, 2, PTrue, 0)
{
  m_extension.SetConstraints(PASN_Object::FixedConstraint, 0, 255);
}


#ifndef PASN_NOPRINTON
void H4509_CcShortArg::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << "{\n";
  if (HasOptionalField(e_ccIdentifier))
    strm << setw(indent+15) << "ccIdentifier = " << setprecision(indent) << m_ccIdentifier << '\n';
  if (HasOptionalField(e_extension))
    strm << setw(indent+12) << "extension = " << setprecision(indent) << m_extension << '\n';
  strm << setw(indent-1) << setprecision(indent-2) << "}";
}
#endif


PObject::Comparison H4509_CcShortArg::Compare(const PObject & obj) const
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(&obj, H4509_CcShortArg), PInvalidCast);
#endif
  const H4509_CcShortArg & other = (const H4509_CcShortArg &)obj;

  Comparison result;

  if ((result = m_ccIdentifier.Compare(other.m_ccIdentifier)) != EqualTo)
    return result;
  if ((result = m_extension.Compare(other.m_extension)) != EqualTo)
    return result;

  return PASN_Sequence::Compare(other);
}


PINDEX H4509_CcShortArg::GetDataLength() const
{
  PINDEX length = 0;
  if (HasOptionalField(e_ccIdentifier))
    length += m_ccIdentifier.GetObjectLength();
  if (HasOptionalField(e_extension))
    length += m_extension.GetObjectLength();
  return length;
}


PBoolean H4509_CcShortArg::Decode(PASN_Stream & strm)
{
  if (!PreambleDecode(strm))
    return PFalse;

  if (HasOptionalField(e_ccIdentifier) && !m_ccIdentifier.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_extension) && !m_extension.Decode(strm))
    return PFalse;

  return UnknownExtensionsDecode(strm);
}


void H4509_CcShortArg::Encode(PASN_Stream & strm) const
{
  PreambleEncode(strm);

  if (HasOptionalField(e_ccIdentifier))
    m_ccIdentifier.Encode(strm);
  if (HasOptionalField(e_extension))
    m_extension.Encode(strm);

  UnknownExtensionsEncode(strm);
}


PObject * H4509_CcShortArg::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_CcShortArg::Class()), PInvalidCast);
#endif
  return new H4509_CcShortArg(*this);
}


//
// CcLongArg
//

H4509_CcLongArg::H4509_CcLongArg(unsigned tag, PASN_Object::TagClass tagClass)
  : PASN_Sequence(tag, tagClass, 5, PTrue, 0)
{
  m_extension.SetConstraints(PASN_Object::FixedConstraint, 0, 255);
}


#ifndef PASN_NOPRINTON
void H4509_CcLongArg::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << "{\n";
  if (HasOptionalField(e_numberA))
    strm << setw(indent+10) << "numberA = " << setprecision(indent) << m_numberA << '\n';
  if (HasOptionalField(e_numberB))
    strm << setw(indent+10) << "numberB = " << setprecision(indent) << m_numberB << '\n';
  if (HasOptionalField(e_ccIdentifier))
    strm << setw(indent+15) << "ccIdentifier = " << setprecision(indent) << m_ccIdentifier << '\n';
  if (HasOptionalField(e_service))
    strm << setw(indent+10) << "service = " << setprecision(indent) << m_service << '\n';
  if (HasOptionalField(e_extension))
    strm << setw(indent+12) << "extension = " << setprecision(indent) << m_extension << '\n';
  strm << setw(indent-1) << setprecision(indent-2) << "}";
}
#endif


PObject::Comparison H4509_CcLongArg::Compare(const PObject & obj) const
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(&obj, H4509_CcLongArg), PInvalidCast);
#endif
  const H4509_CcLongArg & other = (const H4509_CcLongArg &)obj;

  Comparison result;

  if ((result = m_numberA.Compare(other.m_numberA)) != EqualTo)
    return result;
  if ((result = m_numberB.Compare(other.m_numberB)) != EqualTo)
    return result;
  if ((result = m_ccIdentifier.Compare(other.m_ccIdentifier)) != EqualTo)
    return result;
  if ((result = m_service.Compare(other.m_service)) != EqualTo)
    return result;
  if ((result = m_extension.Compare(other.m_extension)) != EqualTo)
    return result;

  return PASN_Sequence::Compare(other);
}


PINDEX H4509_CcLongArg::GetDataLength() const
{
  PINDEX length = 0;
  if (HasOptionalField(e_numberA))
    length += m_numberA.GetObjectLength();
  if (HasOptionalField(e_numberB))
    length += m_numberB.GetObjectLength();
  if (HasOptionalField(e_ccIdentifier))
    length += m_ccIdentifier.GetObjectLength();
  if (HasOptionalField(e_service))
    length += m_service.GetObjectLength();
  if (HasOptionalField(e_extension))
    length += m_extension.GetObjectLength();
  return length;
}


PBoolean H4509_CcLongArg::Decode(PASN_Stream & strm)
{
  if (!PreambleDecode(strm))
    return PFalse;

  if (HasOptionalField(e_numberA) && !m_numberA.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_numberB) && !m_numberB.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_ccIdentifier) && !m_ccIdentifier.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_service) && !m_service.Decode(strm))
    return PFalse;
  if (HasOptionalField(e_extension) && !m_extension.Decode(strm))
    return PFalse;

  return UnknownExtensionsDecode(strm);
}


void H4509_CcLongArg::Encode(PASN_Stream & strm) const
{
  PreambleEncode(strm);

  if (HasOptionalField(e_numberA))
    m_numberA.Encode(strm);
  if (HasOptionalField(e_numberB))
    m_numberB.Encode(strm);
  if (HasOptionalField(e_ccIdentifier))
    m_ccIdentifier.Encode(strm);
  if (HasOptionalField(e_service))
    m_service.Encode(strm);
  if (HasOptionalField(e_extension))
    m_extension.Encode(strm);

  UnknownExtensionsEncode(strm);
}


PObject * H4509_CcLongArg::Clone() const
{
#ifndef PASN_LEANANDMEAN
  PAssert(IsClass(H4509_CcLongArg::Class()), PInvalidCast);
#endif
  return new H4509_CcLongArg(*this);
}


#endif // if ! H323_DISABLE_H4509


// End of h4509.cxx
