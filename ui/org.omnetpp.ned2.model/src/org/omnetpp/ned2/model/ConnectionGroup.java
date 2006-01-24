/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model;

import java.util.Collection;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Connection Group</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.ConnectionGroup#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.ConnectionGroup#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.ConnectionGroup#getLoop <em>Loop</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.ConnectionGroup#getCondition <em>Condition</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.ConnectionGroup#getConnection <em>Connection</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup()
 * @model extendedMetaData="name='connection-group' kind='elementOnly'"
 * @generated
 */
public class ConnectionGroup extends EObjectImpl {
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
	 * The cached value of the '{@link #getConnection() <em>Connection</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getConnection()
	 * @generated
	 * @ordered
	 */
	protected EList connection = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ConnectionGroup() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.CONNECTION_GROUP;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.CONNECTION_GROUP__WHITESPACE);
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup_Group()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
	 *        extendedMetaData="kind='group' name='group:1'"
	 * @generated
	 */
	public FeatureMap getGroup() {
		if (group == null) {
			group = new BasicFeatureMap(this,
					Ned2Package.CONNECTION_GROUP__GROUP);
		}
		return group;
	}

	/**
	 * Returns the value of the '<em><b>Loop</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Loop}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Loop</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup_Loop()
	 * @model type="org.omnetpp.ned2.model.Loop" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='loop' group='#group:1'"
	 * @generated
	 */
	public EList getLoop() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.CONNECTION_GROUP__LOOP);
	}

	/**
	 * Returns the value of the '<em><b>Condition</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Condition}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Condition</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup_Condition()
	 * @model type="org.omnetpp.ned2.model.Condition" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='condition' group='#group:1'"
	 * @generated
	 */
	public EList getCondition() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.CONNECTION_GROUP__CONDITION);
	}

	/**
	 * Returns the value of the '<em><b>Connection</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Connection}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Connection</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getConnectionGroup_Connection()
	 * @model type="org.omnetpp.ned2.model.Connection" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='connection'"
	 * @generated
	 */
	public EList getConnection() {
		if (connection == null) {
			connection = new EObjectContainmentEList(Connection.class, this,
					Ned2Package.CONNECTION_GROUP__CONNECTION);
		}
		return connection;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.CONNECTION_GROUP__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTION_GROUP__GROUP:
			return ((InternalEList) getGroup()).basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTION_GROUP__LOOP:
			return ((InternalEList) getLoop()).basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTION_GROUP__CONDITION:
			return ((InternalEList) getCondition()).basicRemove(otherEnd, msgs);
		case Ned2Package.CONNECTION_GROUP__CONNECTION:
			return ((InternalEList) getConnection())
					.basicRemove(otherEnd, msgs);
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
		case Ned2Package.CONNECTION_GROUP__WHITESPACE:
			return getWhitespace();
		case Ned2Package.CONNECTION_GROUP__GROUP:
			if (coreType)
				return getGroup();
			return ((FeatureMap.Internal) getGroup()).getWrapper();
		case Ned2Package.CONNECTION_GROUP__LOOP:
			return getLoop();
		case Ned2Package.CONNECTION_GROUP__CONDITION:
			return getCondition();
		case Ned2Package.CONNECTION_GROUP__CONNECTION:
			return getConnection();
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
		case Ned2Package.CONNECTION_GROUP__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTION_GROUP__GROUP:
			((FeatureMap.Internal) getGroup()).set(newValue);
			return;
		case Ned2Package.CONNECTION_GROUP__LOOP:
			getLoop().clear();
			getLoop().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTION_GROUP__CONDITION:
			getCondition().clear();
			getCondition().addAll((Collection) newValue);
			return;
		case Ned2Package.CONNECTION_GROUP__CONNECTION:
			getConnection().clear();
			getConnection().addAll((Collection) newValue);
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
		case Ned2Package.CONNECTION_GROUP__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.CONNECTION_GROUP__GROUP:
			getGroup().clear();
			return;
		case Ned2Package.CONNECTION_GROUP__LOOP:
			getLoop().clear();
			return;
		case Ned2Package.CONNECTION_GROUP__CONDITION:
			getCondition().clear();
			return;
		case Ned2Package.CONNECTION_GROUP__CONNECTION:
			getConnection().clear();
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
		case Ned2Package.CONNECTION_GROUP__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.CONNECTION_GROUP__GROUP:
			return group != null && !group.isEmpty();
		case Ned2Package.CONNECTION_GROUP__LOOP:
			return !getLoop().isEmpty();
		case Ned2Package.CONNECTION_GROUP__CONDITION:
			return !getCondition().isEmpty();
		case Ned2Package.CONNECTION_GROUP__CONNECTION:
			return connection != null && !connection.isEmpty();
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
		result.append(')');
		return result.toString();
	}

} // ConnectionGroup
