/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned.model.emf.Connection;
import org.omnetpp.ned.model.emf.ForLoop;
import org.omnetpp.ned.model.emf.NedPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>For Loop</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getForLoop <em>For Loop</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getConnection <em>Connection</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getFromValue <em>From Value</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getParamName <em>Param Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getToValue <em>To Value</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ForLoopImpl#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ForLoopImpl extends EObjectImpl implements ForLoop {
    /**
     * The cached value of the '{@link #getForLoop() <em>For Loop</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getForLoop()
     * @generated
     * @ordered
     */
    protected ForLoop forLoop = null;

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
     * The default value of the '{@link #getBannerComment() <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getBannerComment()
     * @generated
     * @ordered
     */
    protected static final String BANNER_COMMENT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getBannerComment() <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getBannerComment()
     * @generated
     * @ordered
     */
    protected String bannerComment = BANNER_COMMENT_EDEFAULT;

    /**
     * The default value of the '{@link #getFromValue() <em>From Value</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getFromValue()
     * @generated
     * @ordered
     */
    protected static final String FROM_VALUE_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getFromValue() <em>From Value</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getFromValue()
     * @generated
     * @ordered
     */
    protected String fromValue = FROM_VALUE_EDEFAULT;

    /**
     * The default value of the '{@link #getParamName() <em>Param Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getParamName()
     * @generated
     * @ordered
     */
    protected static final String PARAM_NAME_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getParamName() <em>Param Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getParamName()
     * @generated
     * @ordered
     */
    protected String paramName = PARAM_NAME_EDEFAULT;

    /**
     * The default value of the '{@link #getRightComment() <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getRightComment()
     * @generated
     * @ordered
     */
    protected static final String RIGHT_COMMENT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getRightComment() <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getRightComment()
     * @generated
     * @ordered
     */
    protected String rightComment = RIGHT_COMMENT_EDEFAULT;

    /**
     * The default value of the '{@link #getToValue() <em>To Value</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getToValue()
     * @generated
     * @ordered
     */
    protected static final String TO_VALUE_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getToValue() <em>To Value</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getToValue()
     * @generated
     * @ordered
     */
    protected String toValue = TO_VALUE_EDEFAULT;

    /**
     * The default value of the '{@link #getTrailingComment() <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTrailingComment()
     * @generated
     * @ordered
     */
    protected static final String TRAILING_COMMENT_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getTrailingComment() <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTrailingComment()
     * @generated
     * @ordered
     */
    protected String trailingComment = TRAILING_COMMENT_EDEFAULT;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ForLoopImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected EClass eStaticClass() {
        return NedPackage.eINSTANCE.getForLoop();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ForLoop getForLoop() {
        return forLoop;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetForLoop(ForLoop newForLoop, NotificationChain msgs) {
        ForLoop oldForLoop = forLoop;
        forLoop = newForLoop;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__FOR_LOOP, oldForLoop, newForLoop);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setForLoop(ForLoop newForLoop) {
        if (newForLoop != forLoop) {
            NotificationChain msgs = null;
            if (forLoop != null)
                msgs = ((InternalEObject)forLoop).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.FOR_LOOP__FOR_LOOP, null, msgs);
            if (newForLoop != null)
                msgs = ((InternalEObject)newForLoop).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.FOR_LOOP__FOR_LOOP, null, msgs);
            msgs = basicSetForLoop(newForLoop, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__FOR_LOOP, newForLoop, newForLoop));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getConnection() {
        if (connection == null) {
            connection = new EObjectContainmentEList(Connection.class, this, NedPackage.FOR_LOOP__CONNECTION);
        }
        return connection;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getBannerComment() {
        return bannerComment;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setBannerComment(String newBannerComment) {
        String oldBannerComment = bannerComment;
        bannerComment = newBannerComment;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__BANNER_COMMENT, oldBannerComment, bannerComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getFromValue() {
        return fromValue;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setFromValue(String newFromValue) {
        String oldFromValue = fromValue;
        fromValue = newFromValue;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__FROM_VALUE, oldFromValue, fromValue));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getParamName() {
        return paramName;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setParamName(String newParamName) {
        String oldParamName = paramName;
        paramName = newParamName;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__PARAM_NAME, oldParamName, paramName));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getRightComment() {
        return rightComment;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setRightComment(String newRightComment) {
        String oldRightComment = rightComment;
        rightComment = newRightComment;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__RIGHT_COMMENT, oldRightComment, rightComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getToValue() {
        return toValue;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setToValue(String newToValue) {
        String oldToValue = toValue;
        toValue = newToValue;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__TO_VALUE, oldToValue, toValue));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getTrailingComment() {
        return trailingComment;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setTrailingComment(String newTrailingComment) {
        String oldTrailingComment = trailingComment;
        trailingComment = newTrailingComment;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.FOR_LOOP__TRAILING_COMMENT, oldTrailingComment, trailingComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, Class baseClass, NotificationChain msgs) {
        if (featureID >= 0) {
            switch (eDerivedStructuralFeatureID(featureID, baseClass)) {
                case NedPackage.FOR_LOOP__FOR_LOOP:
                    return basicSetForLoop(null, msgs);
                case NedPackage.FOR_LOOP__CONNECTION:
                    return ((InternalEList)getConnection()).basicRemove(otherEnd, msgs);
                default:
                    return eDynamicInverseRemove(otherEnd, featureID, baseClass, msgs);
            }
        }
        return eBasicSetContainer(null, featureID, msgs);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Object eGet(EStructuralFeature eFeature, boolean resolve) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.FOR_LOOP__FOR_LOOP:
                return getForLoop();
            case NedPackage.FOR_LOOP__CONNECTION:
                return getConnection();
            case NedPackage.FOR_LOOP__BANNER_COMMENT:
                return getBannerComment();
            case NedPackage.FOR_LOOP__FROM_VALUE:
                return getFromValue();
            case NedPackage.FOR_LOOP__PARAM_NAME:
                return getParamName();
            case NedPackage.FOR_LOOP__RIGHT_COMMENT:
                return getRightComment();
            case NedPackage.FOR_LOOP__TO_VALUE:
                return getToValue();
            case NedPackage.FOR_LOOP__TRAILING_COMMENT:
                return getTrailingComment();
        }
        return eDynamicGet(eFeature, resolve);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void eSet(EStructuralFeature eFeature, Object newValue) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.FOR_LOOP__FOR_LOOP:
                setForLoop((ForLoop)newValue);
                return;
            case NedPackage.FOR_LOOP__CONNECTION:
                getConnection().clear();
                getConnection().addAll((Collection)newValue);
                return;
            case NedPackage.FOR_LOOP__BANNER_COMMENT:
                setBannerComment((String)newValue);
                return;
            case NedPackage.FOR_LOOP__FROM_VALUE:
                setFromValue((String)newValue);
                return;
            case NedPackage.FOR_LOOP__PARAM_NAME:
                setParamName((String)newValue);
                return;
            case NedPackage.FOR_LOOP__RIGHT_COMMENT:
                setRightComment((String)newValue);
                return;
            case NedPackage.FOR_LOOP__TO_VALUE:
                setToValue((String)newValue);
                return;
            case NedPackage.FOR_LOOP__TRAILING_COMMENT:
                setTrailingComment((String)newValue);
                return;
        }
        eDynamicSet(eFeature, newValue);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void eUnset(EStructuralFeature eFeature) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.FOR_LOOP__FOR_LOOP:
                setForLoop((ForLoop)null);
                return;
            case NedPackage.FOR_LOOP__CONNECTION:
                getConnection().clear();
                return;
            case NedPackage.FOR_LOOP__BANNER_COMMENT:
                setBannerComment(BANNER_COMMENT_EDEFAULT);
                return;
            case NedPackage.FOR_LOOP__FROM_VALUE:
                setFromValue(FROM_VALUE_EDEFAULT);
                return;
            case NedPackage.FOR_LOOP__PARAM_NAME:
                setParamName(PARAM_NAME_EDEFAULT);
                return;
            case NedPackage.FOR_LOOP__RIGHT_COMMENT:
                setRightComment(RIGHT_COMMENT_EDEFAULT);
                return;
            case NedPackage.FOR_LOOP__TO_VALUE:
                setToValue(TO_VALUE_EDEFAULT);
                return;
            case NedPackage.FOR_LOOP__TRAILING_COMMENT:
                setTrailingComment(TRAILING_COMMENT_EDEFAULT);
                return;
        }
        eDynamicUnset(eFeature);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean eIsSet(EStructuralFeature eFeature) {
        switch (eDerivedStructuralFeatureID(eFeature)) {
            case NedPackage.FOR_LOOP__FOR_LOOP:
                return forLoop != null;
            case NedPackage.FOR_LOOP__CONNECTION:
                return connection != null && !connection.isEmpty();
            case NedPackage.FOR_LOOP__BANNER_COMMENT:
                return BANNER_COMMENT_EDEFAULT == null ? bannerComment != null : !BANNER_COMMENT_EDEFAULT.equals(bannerComment);
            case NedPackage.FOR_LOOP__FROM_VALUE:
                return FROM_VALUE_EDEFAULT == null ? fromValue != null : !FROM_VALUE_EDEFAULT.equals(fromValue);
            case NedPackage.FOR_LOOP__PARAM_NAME:
                return PARAM_NAME_EDEFAULT == null ? paramName != null : !PARAM_NAME_EDEFAULT.equals(paramName);
            case NedPackage.FOR_LOOP__RIGHT_COMMENT:
                return RIGHT_COMMENT_EDEFAULT == null ? rightComment != null : !RIGHT_COMMENT_EDEFAULT.equals(rightComment);
            case NedPackage.FOR_LOOP__TO_VALUE:
                return TO_VALUE_EDEFAULT == null ? toValue != null : !TO_VALUE_EDEFAULT.equals(toValue);
            case NedPackage.FOR_LOOP__TRAILING_COMMENT:
                return TRAILING_COMMENT_EDEFAULT == null ? trailingComment != null : !TRAILING_COMMENT_EDEFAULT.equals(trailingComment);
        }
        return eDynamicIsSet(eFeature);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String toString() {
        if (eIsProxy()) return super.toString();

        StringBuffer result = new StringBuffer(super.toString());
        result.append(" (bannerComment: ");
        result.append(bannerComment);
        result.append(", fromValue: ");
        result.append(fromValue);
        result.append(", paramName: ");
        result.append(paramName);
        result.append(", rightComment: ");
        result.append(rightComment);
        result.append(", toValue: ");
        result.append(toValue);
        result.append(", trailingComment: ");
        result.append(trailingComment);
        result.append(')');
        return result.toString();
    }

} //ForLoopImpl
