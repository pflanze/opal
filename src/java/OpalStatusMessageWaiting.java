/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.36
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.opalvoip;

public class OpalStatusMessageWaiting {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected OpalStatusMessageWaiting(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(OpalStatusMessageWaiting obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      exampleJNI.delete_OpalStatusMessageWaiting(swigCPtr);
    }
    swigCPtr = 0;
  }

  public void setM_party(String value) {
    exampleJNI.OpalStatusMessageWaiting_m_party_set(swigCPtr, this, value);
  }

  public String getM_party() {
    return exampleJNI.OpalStatusMessageWaiting_m_party_get(swigCPtr, this);
  }

  public void setM_type(String value) {
    exampleJNI.OpalStatusMessageWaiting_m_type_set(swigCPtr, this, value);
  }

  public String getM_type() {
    return exampleJNI.OpalStatusMessageWaiting_m_type_get(swigCPtr, this);
  }

  public void setM_extraInfo(String value) {
    exampleJNI.OpalStatusMessageWaiting_m_extraInfo_set(swigCPtr, this, value);
  }

  public String getM_extraInfo() {
    return exampleJNI.OpalStatusMessageWaiting_m_extraInfo_get(swigCPtr, this);
  }

  public OpalStatusMessageWaiting() {
    this(exampleJNI.new_OpalStatusMessageWaiting(), true);
  }

}
