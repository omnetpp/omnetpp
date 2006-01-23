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

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Expression</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Expression#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Expression#getOperator <em>Operator</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Expression#getFunction <em>Function</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Expression#getRef <em>Ref</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Expression#getConst <em>Const</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Expression#getTarget <em>Target</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression()
 * @model extendedMetaData="name='expression' kind='elementOnly'"
 * @generated
 */
public class Expression extends EObjectImpl {
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
	 * The cached value of the '{@link #getOperator() <em>Operator</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getOperator()
	 * @generated
	 * @ordered
	 */
	protected Operator operator = null;

	/**
	 * The cached value of the '{@link #getFunction() <em>Function</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFunction()
	 * @generated
	 * @ordered
	 */
	protected Function function = null;

	/**
	 * The cached value of the '{@link #getRef() <em>Ref</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getRef()
	 * @generated
	 * @ordered
	 */
	protected Ref ref = null;

	/**
	 * The cached value of the '{@link #getConst() <em>Const</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getConst()
	 * @generated
	 * @ordered
	 */
	protected Const const_ = null;

	/**
	 * The default value of the '{@link #getTarget() <em>Target</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getTarget()
	 * @generated
	 * @ordered
	 */
	protected static final String TARGET_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getTarget() <em>Target</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getTarget()
	 * @generated
	 * @ordered
	 */
	protected String target = TARGET_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Expression() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.EXPRESSION;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.EXPRESSION__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Operator</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Operator</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Operator</em>' containment reference.
	 * @see #setOperator(Operator)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression_Operator()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='operator'"
	 * @generated
	 */
	public Operator getOperator() {
		return operator;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetOperator(Operator newOperator,
			NotificationChain msgs) {
		Operator oldOperator = operator;
		operator = newOperator;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.EXPRESSION__OPERATOR,
					oldOperator, newOperator);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Expression#getOperator <em>Operator</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Operator</em>' containment reference.
	 * @see #getOperator()
	 * @generated
	 */
	public void setOperator(Operator newOperator) {
		if (newOperator != operator) {
			NotificationChain msgs = null;
			if (operator != null)
				msgs = ((InternalEObject) operator).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.EXPRESSION__OPERATOR, null, msgs);
			if (newOperator != null)
				msgs = ((InternalEObject) newOperator).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.EXPRESSION__OPERATOR, null, msgs);
			msgs = basicSetOperator(newOperator, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.EXPRESSION__OPERATOR, newOperator, newOperator));
	}

	/**
	 * Returns the value of the '<em><b>Function</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Function</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Function</em>' containment reference.
	 * @see #setFunction(Function)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression_Function()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='function'"
	 * @generated
	 */
	public Function getFunction() {
		return function;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetFunction(Function newFunction,
			NotificationChain msgs) {
		Function oldFunction = function;
		function = newFunction;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.EXPRESSION__FUNCTION,
					oldFunction, newFunction);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Expression#getFunction <em>Function</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Function</em>' containment reference.
	 * @see #getFunction()
	 * @generated
	 */
	public void setFunction(Function newFunction) {
		if (newFunction != function) {
			NotificationChain msgs = null;
			if (function != null)
				msgs = ((InternalEObject) function).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.EXPRESSION__FUNCTION, null, msgs);
			if (newFunction != null)
				msgs = ((InternalEObject) newFunction).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.EXPRESSION__FUNCTION, null, msgs);
			msgs = basicSetFunction(newFunction, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.EXPRESSION__FUNCTION, newFunction, newFunction));
	}

	/**
	 * Returns the value of the '<em><b>Ref</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Ref</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Ref</em>' containment reference.
	 * @see #setRef(Ref)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression_Ref()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='ref'"
	 * @generated
	 */
	public Ref getRef() {
		return ref;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetRef(Ref newRef, NotificationChain msgs) {
		Ref oldRef = ref;
		ref = newRef;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.EXPRESSION__REF, oldRef,
					newRef);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Expression#getRef <em>Ref</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Ref</em>' containment reference.
	 * @see #getRef()
	 * @generated
	 */
	public void setRef(Ref newRef) {
		if (newRef != ref) {
			NotificationChain msgs = null;
			if (ref != null)
				msgs = ((InternalEObject) ref).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.EXPRESSION__REF,
						null, msgs);
			if (newRef != null)
				msgs = ((InternalEObject) newRef).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.EXPRESSION__REF,
						null, msgs);
			msgs = basicSetRef(newRef, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.EXPRESSION__REF, newRef, newRef));
	}

	/**
	 * Returns the value of the '<em><b>Const</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Const</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Const</em>' containment reference.
	 * @see #setConst(Const)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression_Const()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='const'"
	 * @generated
	 */
	public Const getConst() {
		return const_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetConst(Const newConst,
			NotificationChain msgs) {
		Const oldConst = const_;
		const_ = newConst;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.EXPRESSION__CONST, oldConst,
					newConst);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Expression#getConst <em>Const</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Const</em>' containment reference.
	 * @see #getConst()
	 * @generated
	 */
	public void setConst(Const newConst) {
		if (newConst != const_) {
			NotificationChain msgs = null;
			if (const_ != null)
				msgs = ((InternalEObject) const_).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.EXPRESSION__CONST,
						null, msgs);
			if (newConst != null)
				msgs = ((InternalEObject) newConst).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.EXPRESSION__CONST,
						null, msgs);
			msgs = basicSetConst(newConst, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.EXPRESSION__CONST, newConst, newConst));
	}

	/**
	 * Returns the value of the '<em><b>Target</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Target</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Target</em>' attribute.
	 * @see #setTarget(String)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getExpression_Target()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='target'"
	 * @generated
	 */
	public String getTarget() {
		return target;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Expression#getTarget <em>Target</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Target</em>' attribute.
	 * @see #getTarget()
	 * @generated
	 */
	public void setTarget(String newTarget) {
		String oldTarget = target;
		target = newTarget;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.EXPRESSION__TARGET, oldTarget, target));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.EXPRESSION__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.EXPRESSION__OPERATOR:
			return basicSetOperator(null, msgs);
		case Ned2Package.EXPRESSION__FUNCTION:
			return basicSetFunction(null, msgs);
		case Ned2Package.EXPRESSION__REF:
			return basicSetRef(null, msgs);
		case Ned2Package.EXPRESSION__CONST:
			return basicSetConst(null, msgs);
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
		case Ned2Package.EXPRESSION__WHITESPACE:
			return getWhitespace();
		case Ned2Package.EXPRESSION__OPERATOR:
			return getOperator();
		case Ned2Package.EXPRESSION__FUNCTION:
			return getFunction();
		case Ned2Package.EXPRESSION__REF:
			return getRef();
		case Ned2Package.EXPRESSION__CONST:
			return getConst();
		case Ned2Package.EXPRESSION__TARGET:
			return getTarget();
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
		case Ned2Package.EXPRESSION__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.EXPRESSION__OPERATOR:
			setOperator((Operator) newValue);
			return;
		case Ned2Package.EXPRESSION__FUNCTION:
			setFunction((Function) newValue);
			return;
		case Ned2Package.EXPRESSION__REF:
			setRef((Ref) newValue);
			return;
		case Ned2Package.EXPRESSION__CONST:
			setConst((Const) newValue);
			return;
		case Ned2Package.EXPRESSION__TARGET:
			setTarget((String) newValue);
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
		case Ned2Package.EXPRESSION__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.EXPRESSION__OPERATOR:
			setOperator((Operator) null);
			return;
		case Ned2Package.EXPRESSION__FUNCTION:
			setFunction((Function) null);
			return;
		case Ned2Package.EXPRESSION__REF:
			setRef((Ref) null);
			return;
		case Ned2Package.EXPRESSION__CONST:
			setConst((Const) null);
			return;
		case Ned2Package.EXPRESSION__TARGET:
			setTarget(TARGET_EDEFAULT);
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
		case Ned2Package.EXPRESSION__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.EXPRESSION__OPERATOR:
			return operator != null;
		case Ned2Package.EXPRESSION__FUNCTION:
			return function != null;
		case Ned2Package.EXPRESSION__REF:
			return ref != null;
		case Ned2Package.EXPRESSION__CONST:
			return const_ != null;
		case Ned2Package.EXPRESSION__TARGET:
			return TARGET_EDEFAULT == null ? target != null : !TARGET_EDEFAULT
					.equals(target);
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
		result.append(" (target: ");
		result.append(target);
		result.append(')');
		return result.toString();
	}

} // Expression
