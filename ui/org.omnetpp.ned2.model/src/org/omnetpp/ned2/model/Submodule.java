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
 * A representation of the model object '<em><b>Submodule</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getExpression <em>Expression</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getParameters <em>Parameters</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getGates <em>Gates</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getLikeParam <em>Like Param</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getLikeType <em>Like Type</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getType <em>Type</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodule#getVectorSize <em>Vector Size</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule()
 * @model extendedMetaData="name='submodule' kind='elementOnly'"
 * @generated
 */
public class Submodule extends EObjectImpl {
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
	 * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getParameters()
	 * @generated
	 * @ordered
	 */
	protected Parameters parameters = null;

	/**
	 * The cached value of the '{@link #getGates() <em>Gates</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGates()
	 * @generated
	 * @ordered
	 */
	protected Gates gates = null;

	/**
	 * The default value of the '{@link #getLikeParam() <em>Like Param</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLikeParam()
	 * @generated
	 * @ordered
	 */
	protected static final String LIKE_PARAM_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getLikeParam() <em>Like Param</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLikeParam()
	 * @generated
	 * @ordered
	 */
	protected String likeParam = LIKE_PARAM_EDEFAULT;

	/**
	 * The default value of the '{@link #getLikeType() <em>Like Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLikeType()
	 * @generated
	 * @ordered
	 */
	protected static final String LIKE_TYPE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getLikeType() <em>Like Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLikeType()
	 * @generated
	 * @ordered
	 */
	protected String likeType = LIKE_TYPE_EDEFAULT;

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
	 * The default value of the '{@link #getType() <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getType()
	 * @generated
	 * @ordered
	 */
	protected static final String TYPE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getType() <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getType()
	 * @generated
	 * @ordered
	 */
	protected String type = TYPE_EDEFAULT;

	/**
	 * The default value of the '{@link #getVectorSize() <em>Vector Size</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getVectorSize()
	 * @generated
	 * @ordered
	 */
	protected static final String VECTOR_SIZE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getVectorSize() <em>Vector Size</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getVectorSize()
	 * @generated
	 * @ordered
	 */
	protected String vectorSize = VECTOR_SIZE_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Submodule() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.SUBMODULE;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.SUBMODULE__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Expression</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Expression}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Expression</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_Expression()
	 * @model type="org.omnetpp.ned2.model.Expression" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='expression'"
	 * @generated
	 */
	public EList getExpression() {
		if (expression == null) {
			expression = new EObjectContainmentEList(Expression.class, this,
					Ned2Package.SUBMODULE__EXPRESSION);
		}
		return expression;
	}

	/**
	 * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Parameters</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Parameters</em>' containment reference.
	 * @see #setParameters(Parameters)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_Parameters()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='parameters'"
	 * @generated
	 */
	public Parameters getParameters() {
		return parameters;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetParameters(Parameters newParameters,
			NotificationChain msgs) {
		Parameters oldParameters = parameters;
		parameters = newParameters;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.SUBMODULE__PARAMETERS,
					oldParameters, newParameters);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getParameters <em>Parameters</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Parameters</em>' containment reference.
	 * @see #getParameters()
	 * @generated
	 */
	public void setParameters(Parameters newParameters) {
		if (newParameters != parameters) {
			NotificationChain msgs = null;
			if (parameters != null)
				msgs = ((InternalEObject) parameters)
						.eInverseRemove(this, EOPPOSITE_FEATURE_BASE
								- Ned2Package.SUBMODULE__PARAMETERS, null, msgs);
			if (newParameters != null)
				msgs = ((InternalEObject) newParameters)
						.eInverseAdd(this, EOPPOSITE_FEATURE_BASE
								- Ned2Package.SUBMODULE__PARAMETERS, null, msgs);
			msgs = basicSetParameters(newParameters, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SUBMODULE__PARAMETERS, newParameters,
					newParameters));
	}

	/**
	 * Returns the value of the '<em><b>Gates</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Gates</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Gates</em>' containment reference.
	 * @see #setGates(Gates)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_Gates()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='gates'"
	 * @generated
	 */
	public Gates getGates() {
		return gates;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetGates(Gates newGates,
			NotificationChain msgs) {
		Gates oldGates = gates;
		gates = newGates;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.SUBMODULE__GATES, oldGates,
					newGates);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getGates <em>Gates</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Gates</em>' containment reference.
	 * @see #getGates()
	 * @generated
	 */
	public void setGates(Gates newGates) {
		if (newGates != gates) {
			NotificationChain msgs = null;
			if (gates != null)
				msgs = ((InternalEObject) gates).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.SUBMODULE__GATES,
						null, msgs);
			if (newGates != null)
				msgs = ((InternalEObject) newGates).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.SUBMODULE__GATES,
						null, msgs);
			msgs = basicSetGates(newGates, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SUBMODULE__GATES, newGates, newGates));
	}

	/**
	 * Returns the value of the '<em><b>Like Param</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Like Param</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Like Param</em>' attribute.
	 * @see #setLikeParam(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_LikeParam()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='like-param'"
	 * @generated
	 */
	public String getLikeParam() {
		return likeParam;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getLikeParam <em>Like Param</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Like Param</em>' attribute.
	 * @see #getLikeParam()
	 * @generated
	 */
	public void setLikeParam(String newLikeParam) {
		String oldLikeParam = likeParam;
		likeParam = newLikeParam;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SUBMODULE__LIKE_PARAM, oldLikeParam, likeParam));
	}

	/**
	 * Returns the value of the '<em><b>Like Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Like Type</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Like Type</em>' attribute.
	 * @see #setLikeType(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_LikeType()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
	 *        extendedMetaData="kind='attribute' name='like-type'"
	 * @generated
	 */
	public String getLikeType() {
		return likeType;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getLikeType <em>Like Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Like Type</em>' attribute.
	 * @see #getLikeType()
	 * @generated
	 */
	public void setLikeType(String newLikeType) {
		String oldLikeType = likeType;
		likeType = newLikeType;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SUBMODULE__LIKE_TYPE, oldLikeType, likeType));
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getName <em>Name</em>}' attribute.
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
					Ned2Package.SUBMODULE__NAME, oldName, name));
	}

	/**
	 * Returns the value of the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Type</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Type</em>' attribute.
	 * @see #setType(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_Type()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
	 *        extendedMetaData="kind='attribute' name='type'"
	 * @generated
	 */
	public String getType() {
		return type;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getType <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Type</em>' attribute.
	 * @see #getType()
	 * @generated
	 */
	public void setType(String newType) {
		String oldType = type;
		type = newType;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SUBMODULE__TYPE, oldType, type));
	}

	/**
	 * Returns the value of the '<em><b>Vector Size</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Vector Size</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Vector Size</em>' attribute.
	 * @see #setVectorSize(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodule_VectorSize()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='vector-size'"
	 * @generated
	 */
	public String getVectorSize() {
		return vectorSize;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Submodule#getVectorSize <em>Vector Size</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Vector Size</em>' attribute.
	 * @see #getVectorSize()
	 * @generated
	 */
	public void setVectorSize(String newVectorSize) {
		String oldVectorSize = vectorSize;
		vectorSize = newVectorSize;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SUBMODULE__VECTOR_SIZE, oldVectorSize,
					vectorSize));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.SUBMODULE__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.SUBMODULE__EXPRESSION:
			return ((InternalEList) getExpression())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.SUBMODULE__PARAMETERS:
			return basicSetParameters(null, msgs);
		case Ned2Package.SUBMODULE__GATES:
			return basicSetGates(null, msgs);
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
		case Ned2Package.SUBMODULE__WHITESPACE:
			return getWhitespace();
		case Ned2Package.SUBMODULE__EXPRESSION:
			return getExpression();
		case Ned2Package.SUBMODULE__PARAMETERS:
			return getParameters();
		case Ned2Package.SUBMODULE__GATES:
			return getGates();
		case Ned2Package.SUBMODULE__LIKE_PARAM:
			return getLikeParam();
		case Ned2Package.SUBMODULE__LIKE_TYPE:
			return getLikeType();
		case Ned2Package.SUBMODULE__NAME:
			return getName();
		case Ned2Package.SUBMODULE__TYPE:
			return getType();
		case Ned2Package.SUBMODULE__VECTOR_SIZE:
			return getVectorSize();
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
		case Ned2Package.SUBMODULE__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.SUBMODULE__EXPRESSION:
			getExpression().clear();
			getExpression().addAll((Collection) newValue);
			return;
		case Ned2Package.SUBMODULE__PARAMETERS:
			setParameters((Parameters) newValue);
			return;
		case Ned2Package.SUBMODULE__GATES:
			setGates((Gates) newValue);
			return;
		case Ned2Package.SUBMODULE__LIKE_PARAM:
			setLikeParam((String) newValue);
			return;
		case Ned2Package.SUBMODULE__LIKE_TYPE:
			setLikeType((String) newValue);
			return;
		case Ned2Package.SUBMODULE__NAME:
			setName((String) newValue);
			return;
		case Ned2Package.SUBMODULE__TYPE:
			setType((String) newValue);
			return;
		case Ned2Package.SUBMODULE__VECTOR_SIZE:
			setVectorSize((String) newValue);
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
		case Ned2Package.SUBMODULE__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.SUBMODULE__EXPRESSION:
			getExpression().clear();
			return;
		case Ned2Package.SUBMODULE__PARAMETERS:
			setParameters((Parameters) null);
			return;
		case Ned2Package.SUBMODULE__GATES:
			setGates((Gates) null);
			return;
		case Ned2Package.SUBMODULE__LIKE_PARAM:
			setLikeParam(LIKE_PARAM_EDEFAULT);
			return;
		case Ned2Package.SUBMODULE__LIKE_TYPE:
			setLikeType(LIKE_TYPE_EDEFAULT);
			return;
		case Ned2Package.SUBMODULE__NAME:
			setName(NAME_EDEFAULT);
			return;
		case Ned2Package.SUBMODULE__TYPE:
			setType(TYPE_EDEFAULT);
			return;
		case Ned2Package.SUBMODULE__VECTOR_SIZE:
			setVectorSize(VECTOR_SIZE_EDEFAULT);
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
		case Ned2Package.SUBMODULE__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.SUBMODULE__EXPRESSION:
			return expression != null && !expression.isEmpty();
		case Ned2Package.SUBMODULE__PARAMETERS:
			return parameters != null;
		case Ned2Package.SUBMODULE__GATES:
			return gates != null;
		case Ned2Package.SUBMODULE__LIKE_PARAM:
			return LIKE_PARAM_EDEFAULT == null ? likeParam != null
					: !LIKE_PARAM_EDEFAULT.equals(likeParam);
		case Ned2Package.SUBMODULE__LIKE_TYPE:
			return LIKE_TYPE_EDEFAULT == null ? likeType != null
					: !LIKE_TYPE_EDEFAULT.equals(likeType);
		case Ned2Package.SUBMODULE__NAME:
			return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
					.equals(name);
		case Ned2Package.SUBMODULE__TYPE:
			return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT
					.equals(type);
		case Ned2Package.SUBMODULE__VECTOR_SIZE:
			return VECTOR_SIZE_EDEFAULT == null ? vectorSize != null
					: !VECTOR_SIZE_EDEFAULT.equals(vectorSize);
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
		result.append(" (likeParam: ");
		result.append(likeParam);
		result.append(", likeType: ");
		result.append(likeType);
		result.append(", name: ");
		result.append(name);
		result.append(", type: ");
		result.append(type);
		result.append(", vectorSize: ");
		result.append(vectorSize);
		result.append(')');
		return result.toString();
	}

} // Submodule
