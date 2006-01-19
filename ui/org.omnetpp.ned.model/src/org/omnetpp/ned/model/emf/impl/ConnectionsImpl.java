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

import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned.model.emf.Connections;
import org.omnetpp.ned.model.emf.NedPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Connections</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#getConnection <em>Connection</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#getForLoop <em>For Loop</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#isCheckUnconnected <em>Check Unconnected</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.ConnectionsImpl#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ConnectionsImpl extends EObjectImpl implements Connections {
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
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ConnectionsImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected EClass eStaticClass() {
        return NedPackage.eINSTANCE.getConnections();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public FeatureMap getGroup() {
        if (group == null) {
            group = new BasicFeatureMap(this, NedPackage.CONNECTIONS__GROUP);
        }
        return group;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getConnection() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getConnections_Connection());
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getForLoop() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getConnections_ForLoop());
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTIONS__BANNER_COMMENT, oldBannerComment, bannerComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean isCheckUnconnected() {
        return checkUnconnected;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setCheckUnconnected(boolean newCheckUnconnected) {
        boolean oldCheckUnconnected = checkUnconnected;
        checkUnconnected = newCheckUnconnected;
        boolean oldCheckUnconnectedESet = checkUnconnectedESet;
        checkUnconnectedESet = true;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTIONS__CHECK_UNCONNECTED, oldCheckUnconnected, checkUnconnected, !oldCheckUnconnectedESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void unsetCheckUnconnected() {
        boolean oldCheckUnconnected = checkUnconnected;
        boolean oldCheckUnconnectedESet = checkUnconnectedESet;
        checkUnconnected = CHECK_UNCONNECTED_EDEFAULT;
        checkUnconnectedESet = false;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.UNSET, NedPackage.CONNECTIONS__CHECK_UNCONNECTED, oldCheckUnconnected, CHECK_UNCONNECTED_EDEFAULT, oldCheckUnconnectedESet));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTIONS__RIGHT_COMMENT, oldRightComment, rightComment));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.CONNECTIONS__TRAILING_COMMENT, oldTrailingComment, trailingComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, Class baseClass, NotificationChain msgs) {
        if (featureID >= 0) {
            switch (eDerivedStructuralFeatureID(featureID, baseClass)) {
                case NedPackage.CONNECTIONS__GROUP:
                    return ((InternalEList)getGroup()).basicRemove(otherEnd, msgs);
                case NedPackage.CONNECTIONS__CONNECTION:
                    return ((InternalEList)getConnection()).basicRemove(otherEnd, msgs);
                case NedPackage.CONNECTIONS__FOR_LOOP:
                    return ((InternalEList)getForLoop()).basicRemove(otherEnd, msgs);
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
            case NedPackage.CONNECTIONS__GROUP:
                return getGroup();
            case NedPackage.CONNECTIONS__CONNECTION:
                return getConnection();
            case NedPackage.CONNECTIONS__FOR_LOOP:
                return getForLoop();
            case NedPackage.CONNECTIONS__BANNER_COMMENT:
                return getBannerComment();
            case NedPackage.CONNECTIONS__CHECK_UNCONNECTED:
                return isCheckUnconnected() ? Boolean.TRUE : Boolean.FALSE;
            case NedPackage.CONNECTIONS__RIGHT_COMMENT:
                return getRightComment();
            case NedPackage.CONNECTIONS__TRAILING_COMMENT:
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
            case NedPackage.CONNECTIONS__GROUP:
                getGroup().clear();
                getGroup().addAll((Collection)newValue);
                return;
            case NedPackage.CONNECTIONS__CONNECTION:
                getConnection().clear();
                getConnection().addAll((Collection)newValue);
                return;
            case NedPackage.CONNECTIONS__FOR_LOOP:
                getForLoop().clear();
                getForLoop().addAll((Collection)newValue);
                return;
            case NedPackage.CONNECTIONS__BANNER_COMMENT:
                setBannerComment((String)newValue);
                return;
            case NedPackage.CONNECTIONS__CHECK_UNCONNECTED:
                setCheckUnconnected(((Boolean)newValue).booleanValue());
                return;
            case NedPackage.CONNECTIONS__RIGHT_COMMENT:
                setRightComment((String)newValue);
                return;
            case NedPackage.CONNECTIONS__TRAILING_COMMENT:
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
            case NedPackage.CONNECTIONS__GROUP:
                getGroup().clear();
                return;
            case NedPackage.CONNECTIONS__CONNECTION:
                getConnection().clear();
                return;
            case NedPackage.CONNECTIONS__FOR_LOOP:
                getForLoop().clear();
                return;
            case NedPackage.CONNECTIONS__BANNER_COMMENT:
                setBannerComment(BANNER_COMMENT_EDEFAULT);
                return;
            case NedPackage.CONNECTIONS__CHECK_UNCONNECTED:
                unsetCheckUnconnected();
                return;
            case NedPackage.CONNECTIONS__RIGHT_COMMENT:
                setRightComment(RIGHT_COMMENT_EDEFAULT);
                return;
            case NedPackage.CONNECTIONS__TRAILING_COMMENT:
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
            case NedPackage.CONNECTIONS__GROUP:
                return group != null && !group.isEmpty();
            case NedPackage.CONNECTIONS__CONNECTION:
                return !getConnection().isEmpty();
            case NedPackage.CONNECTIONS__FOR_LOOP:
                return !getForLoop().isEmpty();
            case NedPackage.CONNECTIONS__BANNER_COMMENT:
                return BANNER_COMMENT_EDEFAULT == null ? bannerComment != null : !BANNER_COMMENT_EDEFAULT.equals(bannerComment);
            case NedPackage.CONNECTIONS__CHECK_UNCONNECTED:
                return isSetCheckUnconnected();
            case NedPackage.CONNECTIONS__RIGHT_COMMENT:
                return RIGHT_COMMENT_EDEFAULT == null ? rightComment != null : !RIGHT_COMMENT_EDEFAULT.equals(rightComment);
            case NedPackage.CONNECTIONS__TRAILING_COMMENT:
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
        result.append(" (group: ");
        result.append(group);
        result.append(", bannerComment: ");
        result.append(bannerComment);
        result.append(", checkUnconnected: ");
        if (checkUnconnectedESet) result.append(checkUnconnected); else result.append("<unset>");
        result.append(", rightComment: ");
        result.append(rightComment);
        result.append(", trailingComment: ");
        result.append(trailingComment);
        result.append(')');
        return result.toString();
    }

} //ConnectionsImpl
