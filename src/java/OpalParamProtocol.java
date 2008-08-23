/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.35
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.opalvoip;

public class OpalParamProtocol {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected OpalParamProtocol(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(OpalParamProtocol obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      exampleJNI.delete_OpalParamProtocol(swigCPtr);
    }
    swigCPtr = 0;
  }

  public void setM_prefix(String value) {
    exampleJNI.OpalParamProtocol_m_prefix_set(swigCPtr, this, value);
  }

  public String getM_prefix() {
    return exampleJNI.OpalParamProtocol_m_prefix_get(swigCPtr, this);
  }

  public void setM_userName(String value) {
    exampleJNI.OpalParamProtocol_m_userName_set(swigCPtr, this, value);
  }

  public String getM_userName() {
    return exampleJNI.OpalParamProtocol_m_userName_get(swigCPtr, this);
  }

  public void setM_displayName(String value) {
    exampleJNI.OpalParamProtocol_m_displayName_set(swigCPtr, this, value);
  }

  public String getM_displayName() {
    return exampleJNI.OpalParamProtocol_m_displayName_get(swigCPtr, this);
  }

  public void setM_vendor(String value) {
    exampleJNI.OpalParamProtocol_m_vendor_set(swigCPtr, this, value);
  }

  public String getM_vendor() {
    return exampleJNI.OpalParamProtocol_m_vendor_get(swigCPtr, this);
  }

  public void setM_name(String value) {
    exampleJNI.OpalParamProtocol_m_name_set(swigCPtr, this, value);
  }

  public String getM_name() {
    return exampleJNI.OpalParamProtocol_m_name_get(swigCPtr, this);
  }

  public void setM_version(String value) {
    exampleJNI.OpalParamProtocol_m_version_set(swigCPtr, this, value);
  }

  public String getM_version() {
    return exampleJNI.OpalParamProtocol_m_version_get(swigCPtr, this);
  }

  public void setM_t35CountryCode(long value) {
    exampleJNI.OpalParamProtocol_m_t35CountryCode_set(swigCPtr, this, value);
  }

  public long getM_t35CountryCode() {
    return exampleJNI.OpalParamProtocol_m_t35CountryCode_get(swigCPtr, this);
  }

  public void setM_t35Extension(long value) {
    exampleJNI.OpalParamProtocol_m_t35Extension_set(swigCPtr, this, value);
  }

  public long getM_t35Extension() {
    return exampleJNI.OpalParamProtocol_m_t35Extension_get(swigCPtr, this);
  }

  public void setM_manufacturerCode(long value) {
    exampleJNI.OpalParamProtocol_m_manufacturerCode_set(swigCPtr, this, value);
  }

  public long getM_manufacturerCode() {
    return exampleJNI.OpalParamProtocol_m_manufacturerCode_get(swigCPtr, this);
  }

  public void setM_interfaceAddresses(String value) {
    exampleJNI.OpalParamProtocol_m_interfaceAddresses_set(swigCPtr, this, value);
  }

  public String getM_interfaceAddresses() {
    return exampleJNI.OpalParamProtocol_m_interfaceAddresses_get(swigCPtr, this);
  }

  public OpalParamProtocol() {
    this(exampleJNI.new_OpalParamProtocol(), true);
  }

}
