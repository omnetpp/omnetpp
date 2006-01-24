/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Ref</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Ref#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Ref#getExpression <em>Expression</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Ref#getModule <em>Module</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Ref#getModuleIndex <em>Module Index</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Ref#getName <em>Name</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef()
 * @model extendedMetaData="name='ref' kind='elementOnly'"
 * @generated
 */
public class Ref extends EObjectImpl {
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
	 * The cached value of the '{@link #getExpression() <em>Expression</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExpression()
	 * @generated
	 * @ordered
	 */
	protected EList expression = null;

	/**
	 * The default value of the '{@link #getModule() <em>Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getModule()
	 * @generated
	 * @ordered
	 */
	protected static final String MODULE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getModule() <em>Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getModule()
	 * @generated
	 * @ordered
	 */
	protected String module = MODULE_EDEFAULT;

	/**
	 * The default value of the '{@link #getModuleIndex() <em>Module Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getModuleIndex()
	 * @generated
	 * @ordered
	 */
	protected static final String MODULE_INDEX_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getModuleIndex() <em>Module Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getModuleIndex()
	 * @generated
	 * @ordered
	 */
	protected String moduleIndex = MODULE_INDEX_EDEFAULT;

	/**
	 * The default value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected static final String NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected String name = NAME_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Ref() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.REF;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.REF__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Expression</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Expression}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Expression</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef_Expression()
	 * @model type="org.omnetpp.ned2.model.Expression" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='expression'"
	 * @generated
	 */
	public EList getExpression() {
		if (expression == null) {
			expression = new EObjectContainmentEList(Expression.class, this,
					Ned2Package.REF__EXPRESSION);
		}
		return expression;
	}

	/**
	 * Returns the value of the '<em><b>Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Module</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Module</em>' attribute.
	 * @see #setModule(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef_Module()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='module'"
	 * @generated
	 */
	public String getModule() {
		return module;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Ref#getModule <em>Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Module</em>' attribute.
	 * @see #getModule()
	 * @generated
	 */
	public void setModule(String newModule) {
		String oldModule = module;
		module = newModule;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.REF__MODULE, oldModule, module));
	}

	/**
	 * Returns the value of the '<em><b>Module Index</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Module Index</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Module Index</em>' attribute.
	 * @see #setModuleIndex(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef_ModuleIndex()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='module-index'"
	 * @generated
	 */
	public String getModuleIndex() {
		return moduleIndex;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Ref#getModuleIndex <em>Module Index</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Module Index</em>' attribute.
	 * @see #getModuleIndex()
	 * @generated
	 */
	public void setModuleIndex(String newModuleIndex) {
		String oldModuleIndex = moduleIndex;
		moduleIndex = newModuleIndex;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.REF__MODULE_INDEX, oldModuleIndex, moduleIndex));
	}

	/**
	 * Returns the value of the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Name</em>' attribute.
	 * @see #setName(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getRef_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Ref#getName <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name</em>' attribute.
	 * @see #getName()
	 * @generated
	 */
	public void setName(String newName) {
		String oldName = name;
		name = newName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.REF__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.REF__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.REF__EXPRESSION:
			return ((InternalEList) getExpression())
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
		case Ned2Package.REF__WHITESPACE:
			return getWhitespace();
		case Ned2Package.REF__EXPRESSION:
			return getExpression();
		case Ned2Package.REF__MODULE:
			return getModule();
		case Ned2Package.REF__MODULE_INDEX:
			return getModuleIndex();
		case Ned2Package.REF__NAME:
			return getName();
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
		case Ned2Package.REF__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.REF__EXPRESSION:
			getExpression().clear();
			getExpression().addAll((Collection) newValue);
			return;
		case Ned2Package.REF__MODULE:
			setModule((String) newValue);
			return;
		case Ned2Package.REF__MODULE_INDEX:
			setModuleIndex((String) newValue);
			return;
		case Ned2Package.REF__NAME:
			setName((String) newValue);
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
		case Ned2Package.REF__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.REF__EXPRESSION:
			getExpression().clear();
			return;
		case Ned2Package.REF__MODULE:
			setModule(MODULE_EDEFAULT);
			return;
		case Ned2Package.REF__MODULE_INDEX:
			setModuleIndex(MODULE_INDEX_EDEFAULT);
			return;
		case Ned2Package.REF__NAME:
			setName(NAME_EDEFAULT);
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
		case Ned2Package.REF__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.REF__EXPRESSION:
			return expression != null && !expression.isEmpty();
		case Ned2Package.REF__MODULE:
			return MODULE_EDEFAULT == null ? module != null : !MODULE_EDEFAULT
					.equals(module);
		case Ned2Package.REF__MODULE_INDEX:
			return MODULE_INDEX_EDEFAULT == null ? moduleIndex != null
					: !MODULE_INDEX_EDEFAULT.equals(moduleIndex);
		case Ned2Package.REF__NAME:
			return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
					.equals(name);
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
		result.append(" (module: ");
		result.append(module);
		result.append(", moduleIndex: ");
		result.append(moduleIndex);
		result.append(", name: ");
		result.append(name);
		result.append(')');
		return result.toString();
	}

} // Ref
