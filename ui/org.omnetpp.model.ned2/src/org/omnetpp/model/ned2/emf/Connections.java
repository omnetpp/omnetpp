/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.model.ned2.emf;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Connections</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Connections#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Connections#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Connections#getConnection <em>Connection</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Connections#getConnectionGroup <em>Connection Group</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getConnections()
 * @model extendedMetaData="name='connections' kind='elementOnly'"
 * @generated
 */
public class Connections extends EObjectImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The cached value of the '{@link #getWhitespace() <em>Whitespace</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getWhitespace()
	 * @generated
	 * @ordered
	 */
	protected EList whitespace = null;

	/**
	 * The cached value of the '{@link #getGroup() <em>Group</em>}' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGroup()
	 * @generated
	 * @ordered
	 */
	protected FeatureMap group = null;

	/**
	 * The default value of the '{@link #isCheckUnconnected() <em>Check Unconnected</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isCheckUnconnected()
	 * @generated
	 * @ordered
	 */
	protected static final boolean CHECK_UNCONNECTED_EDEFAULT = true;

	/**
	 * The cached value of the '{@link #isCheckUnconnected() <em>Check Unconnected</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isCheckUnconnected()
	 * @generated
	 * @ordered
	 */
	protected boolean checkUnconnected = CHECK_UNCONNECTED_EDEFAULT;

	/**
	 * This is true if the Check Unconnected attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean checkUnconnectedESet = false;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Connections() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.CONNECTIONS;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Whitespace</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getConnections_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.CONNECTIONS__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Group</b></em>' attribute list.
	 * The list contents are of type {@link org.eclipse.emf.ecore.util.FeatureMap.Entry}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Group</em>' attribute list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Group</em>' attribute list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getConnections_Group()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
	 *        extendedMetaData="kind='group' name='group:1'"
	 * @generated
	 */
	public FeatureMap getGroup() {
		if (group == null) {
			group = new BasicFeatureMap(this, Ned2Package.CONNECTIONS__GROUP);
		}
		return group;
	}

	/**
	 * Returns the value of the '<em><b>Connection</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Connection}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Connection</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Connection</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getConnections_Connection()
	 * @model type="org.omnetpp.model.ned2.emf.Connection" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='connection' group='#group:1'"
	 * @generated
	 */
	public EList getConnection() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.CONNECTIONS__CONNECTION);
	}

	/**
	 * Returns the value of the '<em><b>Connection Group</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.ConnectionGroup}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Connection Group</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Connection Group</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getConnections_ConnectionGroup()
	 * @model type="org.omnetpp.model.ned2.emf.ConnectionGroup" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='connection-group' group='#group:1'"
	 * @generated
	 */
	public EList getConnectionGroup() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.CONNECTIONS__CONNECTION_GROUP);
	}

	/**
	 * Returns the value of the '<em><b>Check Unconnected</b></em>' attribute.
	 * The default value is <code>"true"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Check Unconnected</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Check Unconnected</em>' attribute.
	 * @see #isSetCheckUnconnected()
	 * @see #unsetCheckUnconnected()
	 * @see #setCheckUnconnected(boolean)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getConnections_CheckUnconnected()
	 * @model default="true" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='check-unconnected'"
	 * @generated
	 */
	public boolean isCheckUnconnected() {
		return checkUnconnected;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Check Unconnected</em>' attribute.
	 * @see #isSetCheckUnconnected()
	 * @see #unsetCheckUnconnected()
	 * @see #isCheckUnconnected()
	 * @generated
	 */
	public void setCheckUnconnected(boolean newCheckUnconnected) {
		boolean oldCheckUnconnected = checkUnconnected;
		checkUnconnected = newCheckUnconnected;
		boolean oldCheckUnconnectedESet = checkUnconnectedESet;
		checkUnconnectedESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.CONNECTIONS__CHECK_UNCONNECTED,
					oldCheckUnconnected, checkUnconnected,
					!oldCheckUnconnectedESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.model.ned2.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetCheckUnconnected()
	 * @see #isCheckUnconnected()
	 * @see #setCheckUnconnected(boolean)
	 * @generated
	 */
	public void unsetCheckUnconnected() {
		boolean oldCheckUnconnected = checkUnconnected;
		boolean oldCheckUnconnectedESet = checkUnconnectedESet;
		checkUnconnected = CHECK_UNCONNECTED_EDEFAULT;
		checkUnconnectedESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.CONNECTIONS__CHECK_UNCONNECTED,
					oldCheckUnconnected, CHECK_UNCONNECTED_EDEFAULT,
					oldCheckUnconnectedESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.model.ned2.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Check Unconnected</em>' attribute is set.
	 * @see #unsetCheckUnconnected()
	 * @see #isCheckUnconnected()
	 * @see #setCheckUnconnected(boolean)
	 * @generated
	 */
	public boolean isSetCheckUnconnected() {
		return checkUnconnectedESet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.CONNECTIONS__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTIONS__GROUP:
			return ((InternalEList) getGroup()).basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTIONS__CONNECTION:
			return ((InternalEList) getConnection())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTIONS__CONNECTION_GROUP:
			return ((InternalEList) getConnectionGroup()).basicRemove(otherEnd,
					msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
		case Ned2Package.CONNECTIONS__WHITESPACE:
			return getWhitespace();
		case Ned2Package.CONNECTIONS__GROUP:
			if (coreType)
				return getGroup();
			return ((FeatureMap.Internal) getGroup()).getWrapper();
		case Ned2Package.CONNECTIONS__CONNECTION:
			return getConnection();
		case Ned2Package.CONNECTIONS__CONNECTION_GROUP:
			return getConnectionGroup();
		case Ned2Package.CONNECTIONS__CHECK_UNCONNECTED:
			return isCheckUnconnected() ? Boolean.TRUE : Boolean.FALSE;
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
		case Ned2Package.CONNECTIONS__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTIONS__GROUP:
			((FeatureMap.Internal) getGroup()).set(newValue);
			return;
		case Ned2Package.CONNECTIONS__CONNECTION:
			getConnection().clear();
			getConnection().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTIONS__CONNECTION_GROUP:
			getConnectionGroup().clear();
			getConnectionGroup().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTIONS__CHECK_UNCONNECTED:
			setCheckUnconnected(((Boolean) newValue).booleanValue());
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
		case Ned2Package.CONNECTIONS__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.CONNECTIONS__GROUP:
			getGroup().clear();
			return;
		case Ned2Package.CONNECTIONS__CONNECTION:
			getConnection().clear();
			return;
		case Ned2Package.CONNECTIONS__CONNECTION_GROUP:
			getConnectionGroup().clear();
			return;
		case Ned2Package.CONNECTIONS__CHECK_UNCONNECTED:
			unsetCheckUnconnected();
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
		case Ned2Package.CONNECTIONS__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.CONNECTIONS__GROUP:
			return group != null && !group.isEmpty();
		case Ned2Package.CONNECTIONS__CONNECTION:
			return !getConnection().isEmpty();
		case Ned2Package.CONNECTIONS__CONNECTION_GROUP:
			return !getConnectionGroup().isEmpty();
		case Ned2Package.CONNECTIONS__CHECK_UNCONNECTED:
			return isSetCheckUnconnected();
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
		result.append(" (group: ");
		result.append(group);
		result.append(", checkUnconnected: ");
		if (checkUnconnectedESet)
			result.append(checkUnconnected);
		else
			result.append("<unset>");
		result.append(')');
		return result.toString();
	}

} // Connections
