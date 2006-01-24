/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Whitespace</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Whitespace#getContent <em>Content</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Whitespace#getLocid <em>Locid</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getWhitespace()
 * @model extendedMetaData="name='whitespace' kind='empty'"
 * @generated
 */
public class Whitespace extends EObjectImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The default value of the '{@link #getContent() <em>Content</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getContent()
	 * @generated
	 * @ordered
	 */
	protected static final String CONTENT_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getContent() <em>Content</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getContent()
	 * @generated
	 * @ordered
	 */
	protected String content = CONTENT_EDEFAULT;

	/**
	 * The default value of the '{@link #getLocid() <em>Locid</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLocid()
	 * @generated
	 * @ordered
	 */
	protected static final String LOCID_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getLocid() <em>Locid</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLocid()
	 * @generated
	 * @ordered
	 */
	protected String locid = LOCID_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Whitespace() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.WHITESPACE;
	}

	/**
	 * Returns the value of the '<em><b>Content</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Content</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Content</em>' attribute.
	 * @see #setContent(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getWhitespace_Content()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
	 *        extendedMetaData="kind='attribute' name='content'"
	 * @generated
	 */
	public String getContent() {
		return content;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Whitespace#getContent <em>Content</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Content</em>' attribute.
	 * @see #getContent()
	 * @generated
	 */
	public void setContent(String newContent) {
		String oldContent = content;
		content = newContent;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.WHITESPACE__CONTENT, oldContent, content));
	}

	/**
	 * Returns the value of the '<em><b>Locid</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Locid</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Locid</em>' attribute.
	 * @see #setLocid(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getWhitespace_Locid()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='locid'"
	 * @generated
	 */
	public String getLocid() {
		return locid;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Whitespace#getLocid <em>Locid</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Locid</em>' attribute.
	 * @see #getLocid()
	 * @generated
	 */
	public void setLocid(String newLocid) {
		String oldLocid = locid;
		locid = newLocid;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.WHITESPACE__LOCID, oldLocid, locid));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
		case Ned2Package.WHITESPACE__CONTENT:
			return getContent();
		case Ned2Package.WHITESPACE__LOCID:
			return getLocid();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
		case Ned2Package.WHITESPACE__CONTENT:
			setContent((String) newValue);
			return;
		case Ned2Package.WHITESPACE__LOCID:
			setLocid((String) newValue);
			return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eUnset(int featureID) {
		switch (featureID) {
		case Ned2Package.WHITESPACE__CONTENT:
			setContent(CONTENT_EDEFAULT);
			return;
		case Ned2Package.WHITESPACE__LOCID:
			setLocid(LOCID_EDEFAULT);
			return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean eIsSet(int featureID) {
		switch (featureID) {
		case Ned2Package.WHITESPACE__CONTENT:
			return CONTENT_EDEFAULT == null ? content != null
					: !CONTENT_EDEFAULT.equals(content);
		case Ned2Package.WHITESPACE__LOCID:
			return LOCID_EDEFAULT == null ? locid != null : !LOCID_EDEFAULT
					.equals(locid);
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String toString() {
		if (eIsProxy())
			return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (content: ");
		result.append(content);
		result.append(", locid: ");
		result.append(locid);
		result.append(')');
		return result.toString();
	}

} // Whitespace
