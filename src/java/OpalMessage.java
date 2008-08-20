/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.35
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.opalvoip;

public class OpalMessage {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected OpalMessage(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(OpalMessage obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      exampleJNI.delete_OpalMessage(swigCPtr);
    }
    swigCPtr = 0;
  }

  public void setM_type(OpalMessageType value) {
    exampleJNI.OpalMessage_m_type_set(swigCPtr, value.swigValue());
  }

  public OpalMessageType getM_type() {
    return OpalMessageType.swigToEnum(exampleJNI.OpalMessage_m_type_get(swigCPtr));
  }

  public OpalMessage_m_param getM_param() {
    long cPtr = exampleJNI.OpalMessage_m_param_get(swigCPtr);
    return (cPtr == 0) ? null : new OpalMessage_m_param(cPtr, false);
  }

  public OpalMessage() {
    this(exampleJNI.new_OpalMessage(), true);
  }

}