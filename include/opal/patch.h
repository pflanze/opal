/*
 * patch.h
 *
 * Media stream patch thread.
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: patch.h,v $
 * Revision 1.2001  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 */

#ifndef __OPAL_PATCH_H
#define __OPAL_PATCH_H

#ifdef __GNUC__
#pragma interface
#endif


class OpalMediaStream;
class OpalTranscoder;


/**Media stream "patch cord".
   This class is the thread of control that transfers data from one
   "source" OpalMediStream to one or more other "sink" OpalMediStream
   instances. It may use zero, one or two intermediary software codecs for
   each sink stream in order to match the media data formats the streams are
   capabile of doing natively.

   Note the thread is not actually started straight away. It is expected that
   the Resume() function is called on the patch when the creator code is
   ready for it to begin. For example all sink streams have been added.
  */
class OpalMediaPatch : public PThread
{
    PCLASSINFO(OpalMediaPatch, PThread);
  public:
  /**@name Construction */
  //@{
    /**Create a new patch.
       Note the thread is not started here.
     */
    OpalMediaPatch(
      OpalMediaStream & source       /// Source media stream
    );

    /**Destroy patch.
     */
    ~OpalMediaPatch();
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Standard stream print function.
       The PObject class has a << operator defined that calls this function
       polymorphically.
      */
    void PrintOn(
      ostream & strm    /// Stream to output text representation
    ) const;
  //@}

  /**@name Operations */
  //@{
    /**Thread entry point.
      */
    virtual void Main();

    /**Close the patch.
       This is an internal function that closes all of the sink streams and
       waits for the the thread to terminate. It is called when the source
       stream is called.
      */
    void Close();

    /**Add another "sink" OpalMediaStream to patch.
       The stream must not be a ReadOnly media stream for the patch to be
       able to write to it.
      */
    BOOL AddSink(
      OpalMediaStream * stream         /// Media stream to add.
    );

    /**Add existing "sink" OpalMediaStream to patch.
       If the stream is not a sink of this patch then this function does
       nothing.
      */
    void RemoveSink(
      OpalMediaStream * stream  /// Medai stream to remove
    );

    /**Get the current source stream for patch.
      */
    OpalMediaStream & GetSource() const { return source; }
  //@}

  protected:
    OpalMediaStream & source;

    class Sink : public PObject {
        PCLASSINFO(Sink, PObject);
      public:
        Sink(OpalMediaStream * s);
        ~Sink();
        OpalMediaStream * stream;
        OpalTranscoder  * primaryCodec;
        OpalTranscoder  * secondaryCodec;
    };
    PARRAY(SinkArray, Sink);

    SinkArray      sinks;
    PMutex         inUse;
};


#endif // __OPAL_PATCH_H


// End of File ///////////////////////////////////////////////////////////////
