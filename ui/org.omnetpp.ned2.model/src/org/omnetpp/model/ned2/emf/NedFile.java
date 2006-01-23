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
 * A representation of the model object '<em><b>Ned File</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getImport <em>Import</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getPropertydef <em>Propertydef</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getProperty <em>Property</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getChannel <em>Channel</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getChannelInterface <em>Channel Interface</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getSimpleModule <em>Simple Module</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getCompoundModule <em>Compound Module</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getModuleInterface <em>Module Interface</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getFilename <em>Filename</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.NedFile#getPackage <em>Package</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile()
 * @model extendedMetaData="name='ned-file' kind='elementOnly'"
 * @generated
 */
public class NedFile extends EObjectImpl {
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
	 * The default value of the '{@link #getFilename() <em>Filename</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFilename()
	 * @generated
	 * @ordered
	 */
	protected static final String FILENAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getFilename() <em>Filename</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFilename()
	 * @generated
	 * @ordered
	 */
	protected String filename = FILENAME_EDEFAULT;

	/**
	 * The default value of the '{@link #getPackage() <em>Package</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPackage()
	 * @generated
	 * @ordered
	 */
	protected static final String PACKAGE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getPackage() <em>Package</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPackage()
	 * @generated
	 * @ordered
	 */
	protected String package_ = PACKAGE_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected NedFile() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.NED_FILE;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.NED_FILE__WHITESPACE);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Group()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
	 *        extendedMetaData="kind='group' name='group:1'"
	 * @generated
	 */
	public FeatureMap getGroup() {
		if (group == null) {
			group = new BasicFeatureMap(this, Ned2Package.NED_FILE__GROUP);
		}
		return group;
	}

	/**
	 * Returns the value of the '<em><b>Import</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Import}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Import</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Import</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Import()
	 * @model type="org.omnetpp.model.ned2.emf.Import" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='import' group='#group:1'"
	 * @generated
	 */
	public EList getImport() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__IMPORT);
	}

	/**
	 * Returns the value of the '<em><b>Propertydef</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Propertydef}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Propertydef</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Propertydef</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Propertydef()
	 * @model type="org.omnetpp.model.ned2.emf.Propertydef" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='propertydef' group='#group:1'"
	 * @generated
	 */
	public EList getPropertydef() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__PROPERTYDEF);
	}

	/**
	 * Returns the value of the '<em><b>Property</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Property}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Property</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Property</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Property()
	 * @model type="org.omnetpp.model.ned2.emf.Property" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='property' group='#group:1'"
	 * @generated
	 */
	public EList getProperty() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__PROPERTY);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Channel()
	 * @model type="org.omnetpp.model.ned2.emf.Channel" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='channel' group='#group:1'"
	 * @generated
	 */
	public EList getChannel() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__CHANNEL);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_ChannelInterface()
	 * @model type="org.omnetpp.model.ned2.emf.ChannelInterface" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='channel-interface' group='#group:1'"
	 * @generated
	 */
	public EList getChannelInterface() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__CHANNEL_INTERFACE);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_SimpleModule()
	 * @model type="org.omnetpp.model.ned2.emf.SimpleModule" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='simple-module' group='#group:1'"
	 * @generated
	 */
	public EList getSimpleModule() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__SIMPLE_MODULE);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_CompoundModule()
	 * @model type="org.omnetpp.model.ned2.emf.CompoundModule" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='compound-module' group='#group:1'"
	 * @generated
	 */
	public EList getCompoundModule() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__COMPOUND_MODULE);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_ModuleInterface()
	 * @model type="org.omnetpp.model.ned2.emf.ModuleInterface" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='module-interface' group='#group:1'"
	 * @generated
	 */
	public EList getModuleInterface() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.NED_FILE__MODULE_INTERFACE);
	}

	/**
	 * Returns the value of the '<em><b>Filename</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Filename</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Filename</em>' attribute.
	 * @see #setFilename(String)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Filename()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
	 *        extendedMetaData="kind='attribute' name='filename'"
	 * @generated
	 */
	public String getFilename() {
		return filename;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.NedFile#getFilename <em>Filename</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Filename</em>' attribute.
	 * @see #getFilename()
	 * @generated
	 */
	public void setFilename(String newFilename) {
		String oldFilename = filename;
		filename = newFilename;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.NED_FILE__FILENAME, oldFilename, filename));
	}

	/**
	 * Returns the value of the '<em><b>Package</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Package</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Package</em>' attribute.
	 * @see #setPackage(String)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getNedFile_Package()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='package'"
	 * @generated
	 */
	public String getPackage() {
		return package_;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.NedFile#getPackage <em>Package</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Package</em>' attribute.
	 * @see #getPackage()
	 * @generated
	 */
	public void setPackage(String newPackage) {
		String oldPackage = package_;
		package_ = newPackage;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.NED_FILE__PACKAGE, oldPackage, package_));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.NED_FILE__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.NED_FILE__GROUP:
			return ((InternalEList) getGroup()).basicRemove(otherEnd, msgs);
		case Ned2Package.NED_FILE__IMPORT:
			return ((InternalEList) getImport()).basicRemove(otherEnd, msgs);
		case Ned2Package.NED_FILE__PROPERTYDEF:
			return ((InternalEList) getPropertydef()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.NED_FILE__PROPERTY:
			return ((InternalEList) getProperty()).basicRemove(otherEnd, msgs);
		case Ned2Package.NED_FILE__CHANNEL:
			return ((InternalEList) getChannel()).basicRemove(otherEnd, msgs);
		case Ned2Package.NED_FILE__CHANNEL_INTERFACE:
			return ((InternalEList) getChannelInterface()).basicRemove(
					otherEnd, msgs);
		case Ned2Package.NED_FILE__SIMPLE_MODULE:
			return ((InternalEList) getSimpleModule()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.NED_FILE__COMPOUND_MODULE:
			return ((InternalEList) getCompoundModule()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.NED_FILE__MODULE_INTERFACE:
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
		case Ned2Package.NED_FILE__WHITESPACE:
			return getWhitespace();
		case Ned2Package.NED_FILE__GROUP:
			if (coreType)
				return getGroup();
			return ((FeatureMap.Internal) getGroup()).getWrapper();
		case Ned2Package.NED_FILE__IMPORT:
			return getImport();
		case Ned2Package.NED_FILE__PROPERTYDEF:
			return getPropertydef();
		case Ned2Package.NED_FILE__PROPERTY:
			return getProperty();
		case Ned2Package.NED_FILE__CHANNEL:
			return getChannel();
		case Ned2Package.NED_FILE__CHANNEL_INTERFACE:
			return getChannelInterface();
		case Ned2Package.NED_FILE__SIMPLE_MODULE:
			return getSimpleModule();
		case Ned2Package.NED_FILE__COMPOUND_MODULE:
			return getCompoundModule();
		case Ned2Package.NED_FILE__MODULE_INTERFACE:
			return getModuleInterface();
		case Ned2Package.NED_FILE__FILENAME:
			return getFilename();
		case Ned2Package.NED_FILE__PACKAGE:
			return getPackage();
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
		case Ned2Package.NED_FILE__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__GROUP:
			((FeatureMap.Internal) getGroup()).set(newValue);
			return;
		case Ned2Package.NED_FILE__IMPORT:
			getImport().clear();
			getImport().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__PROPERTYDEF:
			getPropertydef().clear();
			getPropertydef().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__PROPERTY:
			getProperty().clear();
			getProperty().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__CHANNEL:
			getChannel().clear();
			getChannel().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__CHANNEL_INTERFACE:
			getChannelInterface().clear();
			getChannelInterface().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__SIMPLE_MODULE:
			getSimpleModule().clear();
			getSimpleModule().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__COMPOUND_MODULE:
			getCompoundModule().clear();
			getCompoundModule().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__MODULE_INTERFACE:
			getModuleInterface().clear();
			getModuleInterface().addAll((Collection) newValue);
			return;
		case Ned2Package.NED_FILE__FILENAME:
			setFilename((String) newValue);
			return;
		case Ned2Package.NED_FILE__PACKAGE:
			setPackage((String) newValue);
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
		case Ned2Package.NED_FILE__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.NED_FILE__GROUP:
			getGroup().clear();
			return;
		case Ned2Package.NED_FILE__IMPORT:
			getImport().clear();
			return;
		case Ned2Package.NED_FILE__PROPERTYDEF:
			getPropertydef().clear();
			return;
		case Ned2Package.NED_FILE__PROPERTY:
			getProperty().clear();
			return;
		case Ned2Package.NED_FILE__CHANNEL:
			getChannel().clear();
			return;
		case Ned2Package.NED_FILE__CHANNEL_INTERFACE:
			getChannelInterface().clear();
			return;
		case Ned2Package.NED_FILE__SIMPLE_MODULE:
			getSimpleModule().clear();
			return;
		case Ned2Package.NED_FILE__COMPOUND_MODULE:
			getCompoundModule().clear();
			return;
		case Ned2Package.NED_FILE__MODULE_INTERFACE:
			getModuleInterface().clear();
			return;
		case Ned2Package.NED_FILE__FILENAME:
			setFilename(FILENAME_EDEFAULT);
			return;
		case Ned2Package.NED_FILE__PACKAGE:
			setPackage(PACKAGE_EDEFAULT);
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
		case Ned2Package.NED_FILE__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.NED_FILE__GROUP:
			return group != null && !group.isEmpty();
		case Ned2Package.NED_FILE__IMPORT:
			return !getImport().isEmpty();
		case Ned2Package.NED_FILE__PROPERTYDEF:
			return !getPropertydef().isEmpty();
		case Ned2Package.NED_FILE__PROPERTY:
			return !getProperty().isEmpty();
		case Ned2Package.NED_FILE__CHANNEL:
			return !getChannel().isEmpty();
		case Ned2Package.NED_FILE__CHANNEL_INTERFACE:
			return !getChannelInterface().isEmpty();
		case Ned2Package.NED_FILE__SIMPLE_MODULE:
			return !getSimpleModule().isEmpty();
		case Ned2Package.NED_FILE__COMPOUND_MODULE:
			return !getCompoundModule().isEmpty();
		case Ned2Package.NED_FILE__MODULE_INTERFACE:
			return !getModuleInterface().isEmpty();
		case Ned2Package.NED_FILE__FILENAME:
			return FILENAME_EDEFAULT == null ? filename != null
					: !FILENAME_EDEFAULT.equals(filename);
		case Ned2Package.NED_FILE__PACKAGE:
			return PACKAGE_EDEFAULT == null ? package_ != null
					: !PACKAGE_EDEFAULT.equals(package_);
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
		result.append(", filename: ");
		result.append(filename);
		result.append(", package: ");
		result.append(package_);
		result.append(')');
		return result.toString();
	}

} // NedFile
