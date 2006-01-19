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

import org.omnetpp.ned.model.emf.DisplayString;
import org.omnetpp.ned.model.emf.Gatesizes;
import org.omnetpp.ned.model.emf.NedPackage;
import org.omnetpp.ned.model.emf.Submodule;
import org.omnetpp.ned.model.emf.Substparams;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Submodule</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getSubstparams <em>Substparams</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getGatesizes <em>Gatesizes</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getDisplayString <em>Display String</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getLikeParam <em>Like Param</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getTrailingComment <em>Trailing Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getTypeName <em>Type Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.SubmoduleImpl#getVectorSize <em>Vector Size</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class SubmoduleImpl extends EObjectImpl implements Submodule {
    /**
     * The cached value of the '{@link #getSubstparams() <em>Substparams</em>}' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getSubstparams()
     * @generated
     * @ordered
     */
    protected EList substparams = null;

    /**
     * The cached value of the '{@link #getGatesizes() <em>Gatesizes</em>}' containment reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getGatesizes()
     * @generated
     * @ordered
     */
    protected EList gatesizes = null;

    /**
     * The cached value of the '{@link #getDisplayString() <em>Display String</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getDisplayString()
     * @generated
     * @ordered
     */
    protected DisplayString displayString = null;

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
     * The default value of the '{@link #getTypeName() <em>Type Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTypeName()
     * @generated
     * @ordered
     */
    protected static final String TYPE_NAME_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getTypeName() <em>Type Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getTypeName()
     * @generated
     * @ordered
     */
    protected String typeName = TYPE_NAME_EDEFAULT;

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
    protected SubmoduleImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected EClass eStaticClass() {
        return NedPackage.eINSTANCE.getSubmodule();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getSubstparams() {
        if (substparams == null) {
            substparams = new EObjectContainmentEList(Substparams.class, this, NedPackage.SUBMODULE__SUBSTPARAMS);
        }
        return substparams;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getGatesizes() {
        if (gatesizes == null) {
            gatesizes = new EObjectContainmentEList(Gatesizes.class, this, NedPackage.SUBMODULE__GATESIZES);
        }
        return gatesizes;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public DisplayString getDisplayString() {
        return displayString;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetDisplayString(DisplayString newDisplayString, NotificationChain msgs) {
        DisplayString oldDisplayString = displayString;
        displayString = newDisplayString;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__DISPLAY_STRING, oldDisplayString, newDisplayString);
            if (msgs == null) msgs = notification; else msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setDisplayString(DisplayString newDisplayString) {
        if (newDisplayString != displayString) {
            NotificationChain msgs = null;
            if (displayString != null)
                msgs = ((InternalEObject)displayString).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - NedPackage.SUBMODULE__DISPLAY_STRING, null, msgs);
            if (newDisplayString != null)
                msgs = ((InternalEObject)newDisplayString).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - NedPackage.SUBMODULE__DISPLAY_STRING, null, msgs);
            msgs = basicSetDisplayString(newDisplayString, msgs);
            if (msgs != null) msgs.dispatch();
        }
        else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__DISPLAY_STRING, newDisplayString, newDisplayString));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__BANNER_COMMENT, oldBannerComment, bannerComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getLikeParam() {
        return likeParam;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setLikeParam(String newLikeParam) {
        String oldLikeParam = likeParam;
        likeParam = newLikeParam;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__LIKE_PARAM, oldLikeParam, likeParam));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getName() {
        return name;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setName(String newName) {
        String oldName = name;
        name = newName;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__NAME, oldName, name));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__RIGHT_COMMENT, oldRightComment, rightComment));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__TRAILING_COMMENT, oldTrailingComment, trailingComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getTypeName() {
        return typeName;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setTypeName(String newTypeName) {
        String oldTypeName = typeName;
        typeName = newTypeName;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__TYPE_NAME, oldTypeName, typeName));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getVectorSize() {
        return vectorSize;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setVectorSize(String newVectorSize) {
        String oldVectorSize = vectorSize;
        vectorSize = newVectorSize;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.SUBMODULE__VECTOR_SIZE, oldVectorSize, vectorSize));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, Class baseClass, NotificationChain msgs) {
        if (featureID >= 0) {
            switch (eDerivedStructuralFeatureID(featureID, baseClass)) {
                case NedPackage.SUBMODULE__SUBSTPARAMS:
                    return ((InternalEList)getSubstparams()).basicRemove(otherEnd, msgs);
                case NedPackage.SUBMODULE__GATESIZES:
                    return ((InternalEList)getGatesizes()).basicRemove(otherEnd, msgs);
                case NedPackage.SUBMODULE__DISPLAY_STRING:
                    return basicSetDisplayString(null, msgs);
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
            case NedPackage.SUBMODULE__SUBSTPARAMS:
                return getSubstparams();
            case NedPackage.SUBMODULE__GATESIZES:
                return getGatesizes();
            case NedPackage.SUBMODULE__DISPLAY_STRING:
                return getDisplayString();
            case NedPackage.SUBMODULE__BANNER_COMMENT:
                return getBannerComment();
            case NedPackage.SUBMODULE__LIKE_PARAM:
                return getLikeParam();
            case NedPackage.SUBMODULE__NAME:
                return getName();
            case NedPackage.SUBMODULE__RIGHT_COMMENT:
                return getRightComment();
            case NedPackage.SUBMODULE__TRAILING_COMMENT:
                return getTrailingComment();
            case NedPackage.SUBMODULE__TYPE_NAME:
                return getTypeName();
            case NedPackage.SUBMODULE__VECTOR_SIZE:
                return getVectorSize();
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
            case NedPackage.SUBMODULE__SUBSTPARAMS:
                getSubstparams().clear();
                getSubstparams().addAll((Collection)newValue);
                return;
            case NedPackage.SUBMODULE__GATESIZES:
                getGatesizes().clear();
                getGatesizes().addAll((Collection)newValue);
                return;
            case NedPackage.SUBMODULE__DISPLAY_STRING:
                setDisplayString((DisplayString)newValue);
                return;
            case NedPackage.SUBMODULE__BANNER_COMMENT:
                setBannerComment((String)newValue);
                return;
            case NedPackage.SUBMODULE__LIKE_PARAM:
                setLikeParam((String)newValue);
                return;
            case NedPackage.SUBMODULE__NAME:
                setName((String)newValue);
                return;
            case NedPackage.SUBMODULE__RIGHT_COMMENT:
                setRightComment((String)newValue);
                return;
            case NedPackage.SUBMODULE__TRAILING_COMMENT:
                setTrailingComment((String)newValue);
                return;
            case NedPackage.SUBMODULE__TYPE_NAME:
                setTypeName((String)newValue);
                return;
            case NedPackage.SUBMODULE__VECTOR_SIZE:
                setVectorSize((String)newValue);
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
            case NedPackage.SUBMODULE__SUBSTPARAMS:
                getSubstparams().clear();
                return;
            case NedPackage.SUBMODULE__GATESIZES:
                getGatesizes().clear();
                return;
            case NedPackage.SUBMODULE__DISPLAY_STRING:
                setDisplayString((DisplayString)null);
                return;
            case NedPackage.SUBMODULE__BANNER_COMMENT:
                setBannerComment(BANNER_COMMENT_EDEFAULT);
                return;
            case NedPackage.SUBMODULE__LIKE_PARAM:
                setLikeParam(LIKE_PARAM_EDEFAULT);
                return;
            case NedPackage.SUBMODULE__NAME:
                setName(NAME_EDEFAULT);
                return;
            case NedPackage.SUBMODULE__RIGHT_COMMENT:
                setRightComment(RIGHT_COMMENT_EDEFAULT);
                return;
            case NedPackage.SUBMODULE__TRAILING_COMMENT:
                setTrailingComment(TRAILING_COMMENT_EDEFAULT);
                return;
            case NedPackage.SUBMODULE__TYPE_NAME:
                setTypeName(TYPE_NAME_EDEFAULT);
                return;
            case NedPackage.SUBMODULE__VECTOR_SIZE:
                setVectorSize(VECTOR_SIZE_EDEFAULT);
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
            case NedPackage.SUBMODULE__SUBSTPARAMS:
                return substparams != null && !substparams.isEmpty();
            case NedPackage.SUBMODULE__GATESIZES:
                return gatesizes != null && !gatesizes.isEmpty();
            case NedPackage.SUBMODULE__DISPLAY_STRING:
                return displayString != null;
            case NedPackage.SUBMODULE__BANNER_COMMENT:
                return BANNER_COMMENT_EDEFAULT == null ? bannerComment != null : !BANNER_COMMENT_EDEFAULT.equals(bannerComment);
            case NedPackage.SUBMODULE__LIKE_PARAM:
                return LIKE_PARAM_EDEFAULT == null ? likeParam != null : !LIKE_PARAM_EDEFAULT.equals(likeParam);
            case NedPackage.SUBMODULE__NAME:
                return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
            case NedPackage.SUBMODULE__RIGHT_COMMENT:
                return RIGHT_COMMENT_EDEFAULT == null ? rightComment != null : !RIGHT_COMMENT_EDEFAULT.equals(rightComment);
            case NedPackage.SUBMODULE__TRAILING_COMMENT:
                return TRAILING_COMMENT_EDEFAULT == null ? trailingComment != null : !TRAILING_COMMENT_EDEFAULT.equals(trailingComment);
            case NedPackage.SUBMODULE__TYPE_NAME:
                return TYPE_NAME_EDEFAULT == null ? typeName != null : !TYPE_NAME_EDEFAULT.equals(typeName);
            case NedPackage.SUBMODULE__VECTOR_SIZE:
                return VECTOR_SIZE_EDEFAULT == null ? vectorSize != null : !VECTOR_SIZE_EDEFAULT.equals(vectorSize);
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
        result.append(", likeParam: ");
        result.append(likeParam);
        result.append(", name: ");
        result.append(name);
        result.append(", rightComment: ");
        result.append(rightComment);
        result.append(", trailingComment: ");
        result.append(trailingComment);
        result.append(", typeName: ");
        result.append(typeName);
        result.append(", vectorSize: ");
        result.append(vectorSize);
        result.append(')');
        return result.toString();
    }

} //SubmoduleImpl
