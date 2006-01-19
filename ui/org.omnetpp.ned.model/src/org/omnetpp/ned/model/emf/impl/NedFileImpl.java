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

import org.omnetpp.ned.model.emf.NedFile;
import org.omnetpp.ned.model.emf.NedPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>File</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getImport <em>Import</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getChannel <em>Channel</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getSimpleModule <em>Simple Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getCompoundModule <em>Compound Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getNetwork <em>Network</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.impl.NedFileImpl#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class NedFileImpl extends EObjectImpl implements NedFile {
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
    protected NedFileImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected EClass eStaticClass() {
        return NedPackage.eINSTANCE.getNedFile();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public FeatureMap getGroup() {
        if (group == null) {
            group = new BasicFeatureMap(this, NedPackage.NED_FILE__GROUP);
        }
        return group;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getImport() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getNedFile_Import());
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getChannel() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getNedFile_Channel());
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getSimpleModule() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getNedFile_SimpleModule());
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getCompoundModule() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getNedFile_CompoundModule());
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EList getNetwork() {
        return ((FeatureMap)getGroup()).list(NedPackage.eINSTANCE.getNedFile_Network());
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.NED_FILE__BANNER_COMMENT, oldBannerComment, bannerComment));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.NED_FILE__RIGHT_COMMENT, oldRightComment, rightComment));
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
            eNotify(new ENotificationImpl(this, Notification.SET, NedPackage.NED_FILE__TRAILING_COMMENT, oldTrailingComment, trailingComment));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, Class baseClass, NotificationChain msgs) {
        if (featureID >= 0) {
            switch (eDerivedStructuralFeatureID(featureID, baseClass)) {
                case NedPackage.NED_FILE__GROUP:
                    return ((InternalEList)getGroup()).basicRemove(otherEnd, msgs);
                case NedPackage.NED_FILE__IMPORT:
                    return ((InternalEList)getImport()).basicRemove(otherEnd, msgs);
                case NedPackage.NED_FILE__CHANNEL:
                    return ((InternalEList)getChannel()).basicRemove(otherEnd, msgs);
                case NedPackage.NED_FILE__SIMPLE_MODULE:
                    return ((InternalEList)getSimpleModule()).basicRemove(otherEnd, msgs);
                case NedPackage.NED_FILE__COMPOUND_MODULE:
                    return ((InternalEList)getCompoundModule()).basicRemove(otherEnd, msgs);
                case NedPackage.NED_FILE__NETWORK:
                    return ((InternalEList)getNetwork()).basicRemove(otherEnd, msgs);
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
            case NedPackage.NED_FILE__GROUP:
                return getGroup();
            case NedPackage.NED_FILE__IMPORT:
                return getImport();
            case NedPackage.NED_FILE__CHANNEL:
                return getChannel();
            case NedPackage.NED_FILE__SIMPLE_MODULE:
                return getSimpleModule();
            case NedPackage.NED_FILE__COMPOUND_MODULE:
                return getCompoundModule();
            case NedPackage.NED_FILE__NETWORK:
                return getNetwork();
            case NedPackage.NED_FILE__BANNER_COMMENT:
                return getBannerComment();
            case NedPackage.NED_FILE__RIGHT_COMMENT:
                return getRightComment();
            case NedPackage.NED_FILE__TRAILING_COMMENT:
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
            case NedPackage.NED_FILE__GROUP:
                getGroup().clear();
                getGroup().addAll((Collection)newValue);
                return;
            case NedPackage.NED_FILE__IMPORT:
                getImport().clear();
                getImport().addAll((Collection)newValue);
                return;
            case NedPackage.NED_FILE__CHANNEL:
                getChannel().clear();
                getChannel().addAll((Collection)newValue);
                return;
            case NedPackage.NED_FILE__SIMPLE_MODULE:
                getSimpleModule().clear();
                getSimpleModule().addAll((Collection)newValue);
                return;
            case NedPackage.NED_FILE__COMPOUND_MODULE:
                getCompoundModule().clear();
                getCompoundModule().addAll((Collection)newValue);
                return;
            case NedPackage.NED_FILE__NETWORK:
                getNetwork().clear();
                getNetwork().addAll((Collection)newValue);
                return;
            case NedPackage.NED_FILE__BANNER_COMMENT:
                setBannerComment((String)newValue);
                return;
            case NedPackage.NED_FILE__RIGHT_COMMENT:
                setRightComment((String)newValue);
                return;
            case NedPackage.NED_FILE__TRAILING_COMMENT:
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
            case NedPackage.NED_FILE__GROUP:
                getGroup().clear();
                return;
            case NedPackage.NED_FILE__IMPORT:
                getImport().clear();
                return;
            case NedPackage.NED_FILE__CHANNEL:
                getChannel().clear();
                return;
            case NedPackage.NED_FILE__SIMPLE_MODULE:
                getSimpleModule().clear();
                return;
            case NedPackage.NED_FILE__COMPOUND_MODULE:
                getCompoundModule().clear();
                return;
            case NedPackage.NED_FILE__NETWORK:
                getNetwork().clear();
                return;
            case NedPackage.NED_FILE__BANNER_COMMENT:
                setBannerComment(BANNER_COMMENT_EDEFAULT);
                return;
            case NedPackage.NED_FILE__RIGHT_COMMENT:
                setRightComment(RIGHT_COMMENT_EDEFAULT);
                return;
            case NedPackage.NED_FILE__TRAILING_COMMENT:
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
            case NedPackage.NED_FILE__GROUP:
                return group != null && !group.isEmpty();
            case NedPackage.NED_FILE__IMPORT:
                return !getImport().isEmpty();
            case NedPackage.NED_FILE__CHANNEL:
                return !getChannel().isEmpty();
            case NedPackage.NED_FILE__SIMPLE_MODULE:
                return !getSimpleModule().isEmpty();
            case NedPackage.NED_FILE__COMPOUND_MODULE:
                return !getCompoundModule().isEmpty();
            case NedPackage.NED_FILE__NETWORK:
                return !getNetwork().isEmpty();
            case NedPackage.NED_FILE__BANNER_COMMENT:
                return BANNER_COMMENT_EDEFAULT == null ? bannerComment != null : !BANNER_COMMENT_EDEFAULT.equals(bannerComment);
            case NedPackage.NED_FILE__RIGHT_COMMENT:
                return RIGHT_COMMENT_EDEFAULT == null ? rightComment != null : !RIGHT_COMMENT_EDEFAULT.equals(rightComment);
            case NedPackage.NED_FILE__TRAILING_COMMENT:
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
        result.append(", rightComment: ");
        result.append(rightComment);
        result.append(", trailingComment: ");
        result.append(trailingComment);
        result.append(')');
        return result.toString();
    }

} //NedFileImpl
