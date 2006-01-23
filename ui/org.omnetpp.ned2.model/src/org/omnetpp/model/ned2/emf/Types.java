/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.model.ned2.emf;

import java.util.Collection;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Types</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getChannel <em>Channel</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getChannelInterface <em>Channel Interface</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getSimpleModule <em>Simple Module</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getCompoundModule <em>Compound Module</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Types#getModuleInterface <em>Module Interface</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes()
 * @model extendedMetaData="name='types' kind='elementOnly'"
 * @generated
 */
public class Types extends EObjectImpl {
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
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Types() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.TYPES;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.TYPES__WHITESPACE);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_Group()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
	 *        extendedMetaData="kind='group' name='group:1'"
	 * @generated
	 */
	public FeatureMap getGroup() {
		if (group == null) {
			group = new BasicFeatureMap(this, Ned2Package.TYPES__GROUP);
		}
		return group;
	}

	/**
	 * Returns the value of the '<em><b>Channel</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Channel}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Channel</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Channel</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_Channel()
	 * @model type="org.omnetpp.model.ned2.emf.Channel" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='channel' group='#group:1'"
	 * @generated
	 */
	public EList getChannel() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.TYPES__CHANNEL);
	}

	/**
	 * Returns the value of the '<em><b>Channel Interface</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.ChannelInterface}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Channel Interface</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Channel Interface</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_ChannelInterface()
	 * @model type="org.omnetpp.model.ned2.emf.ChannelInterface" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='channel-interface' group='#group:1'"
	 * @generated
	 */
	public EList getChannelInterface() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.TYPES__CHANNEL_INTERFACE);
	}

	/**
	 * Returns the value of the '<em><b>Simple Module</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.SimpleModule}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Simple Module</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Simple Module</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_SimpleModule()
	 * @model type="org.omnetpp.model.ned2.emf.SimpleModule" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='simple-module' group='#group:1'"
	 * @generated
	 */
	public EList getSimpleModule() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.TYPES__SIMPLE_MODULE);
	}

	/**
	 * Returns the value of the '<em><b>Compound Module</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.CompoundModule}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Compound Module</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Compound Module</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_CompoundModule()
	 * @model type="org.omnetpp.model.ned2.emf.CompoundModule" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='compound-module' group='#group:1'"
	 * @generated
	 */
	public EList getCompoundModule() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.TYPES__COMPOUND_MODULE);
	}

	/**
	 * Returns the value of the '<em><b>Module Interface</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.ModuleInterface}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Module Interface</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Module Interface</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getTypes_ModuleInterface()
	 * @model type="org.omnetpp.model.ned2.emf.ModuleInterface" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='module-interface' group='#group:1'"
	 * @generated
	 */
	public EList getModuleInterface() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.TYPES__MODULE_INTERFACE);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.TYPES__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.TYPES__GROUP:
			return ((InternalEList) getGroup()).basicRemove(otherEnd, msgs);
		case Ned2Package.TYPES__CHANNEL:
			return ((InternalEList) getChannel()).basicRemove(otherEnd, msgs);
		case Ned2Package.TYPES__CHANNEL_INTERFACE:
			return ((InternalEList) getChannelInterface()).basicRemove(
					otherEnd, msgs);
		case Ned2Package.TYPES__SIMPLE_MODULE:
			return ((InternalEList) getSimpleModule()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.TYPES__COMPOUND_MODULE:
			return ((InternalEList) getCompoundModule()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.TYPES__MODULE_INTERFACE:
			return ((InternalEList) getModuleInterface()).basicRemove(otherEnd,
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
		case Ned2Package.TYPES__WHITESPACE:
			return getWhitespace();
		case Ned2Package.TYPES__GROUP:
			if (coreType)
				return getGroup();
			return ((FeatureMap.Internal) getGroup()).getWrapper();
		case Ned2Package.TYPES__CHANNEL:
			return getChannel();
		case Ned2Package.TYPES__CHANNEL_INTERFACE:
			return getChannelInterface();
		case Ned2Package.TYPES__SIMPLE_MODULE:
			return getSimpleModule();
		case Ned2Package.TYPES__COMPOUND_MODULE:
			return getCompoundModule();
		case Ned2Package.TYPES__MODULE_INTERFACE:
			return getModuleInterface();
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
		case Ned2Package.TYPES__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.TYPES__GROUP:
			((FeatureMap.Internal) getGroup()).set(newValue);
			return;
		case Ned2Package.TYPES__CHANNEL:
			getChannel().clear();
			getChannel().addAll((Collection) newValue);
			return;
		case Ned2Package.TYPES__CHANNEL_INTERFACE:
			getChannelInterface().clear();
			getChannelInterface().addAll((Collection) newValue);
			return;
		case Ned2Package.TYPES__SIMPLE_MODULE:
			getSimpleModule().clear();
			getSimpleModule().addAll((Collection) newValue);
			return;
		case Ned2Package.TYPES__COMPOUND_MODULE:
			getCompoundModule().clear();
			getCompoundModule().addAll((Collection) newValue);
			return;
		case Ned2Package.TYPES__MODULE_INTERFACE:
			getModuleInterface().clear();
			getModuleInterface().addAll((Collection) newValue);
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
		case Ned2Package.TYPES__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.TYPES__GROUP:
			getGroup().clear();
			return;
		case Ned2Package.TYPES__CHANNEL:
			getChannel().clear();
			return;
		case Ned2Package.TYPES__CHANNEL_INTERFACE:
			getChannelInterface().clear();
			return;
		case Ned2Package.TYPES__SIMPLE_MODULE:
			getSimpleModule().clear();
			return;
		case Ned2Package.TYPES__COMPOUND_MODULE:
			getCompoundModule().clear();
			return;
		case Ned2Package.TYPES__MODULE_INTERFACE:
			getModuleInterface().clear();
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
		case Ned2Package.TYPES__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.TYPES__GROUP:
			return group != null && !group.isEmpty();
		case Ned2Package.TYPES__CHANNEL:
			return !getChannel().isEmpty();
		case Ned2Package.TYPES__CHANNEL_INTERFACE:
			return !getChannelInterface().isEmpty();
		case Ned2Package.TYPES__SIMPLE_MODULE:
			return !getSimpleModule().isEmpty();
		case Ned2Package.TYPES__COMPOUND_MODULE:
			return !getCompoundModule().isEmpty();
		case Ned2Package.TYPES__MODULE_INTERFACE:
			return !getModuleInterface().isEmpty();
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

} // Types
