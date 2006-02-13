/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

import org.eclipse.emf.ecore.util.FeatureMap;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Connections</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#getConnection <em>Connection</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#getForLoop <em>For Loop</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connections#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getConnections()
 * @model extendedMetaData="name='connections' kind='elementOnly'"
 * @generated
 */
public interface Connections extends EObject {
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_Group()
     * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
     *        extendedMetaData="kind='group' name='group:0'"
     * @generated
     */
    FeatureMap getGroup();

    /**
     * Returns the value of the '<em><b>Connection</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Connection}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Connection</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Connection</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_Connection()
     * @model type="org.omnetpp.ned.model.emf.Connection" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='connection' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getConnection();

    /**
     * Returns the value of the '<em><b>For Loop</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.ForLoop}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>For Loop</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>For Loop</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_ForLoop()
     * @model type="org.omnetpp.ned.model.emf.ForLoop" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='for-loop' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getForLoop();

    /**
     * Returns the value of the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Banner Comment</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Banner Comment</em>' attribute.
     * @see #setBannerComment(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connections#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

    /**
     * Returns the value of the '<em><b>Check Unconnected</b></em>' attribute.
     * The default value is <code>"true"</code>.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Check Unconnected</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Check Unconnected</em>' attribute.
     * @see #isSetCheckUnconnected()
     * @see #unsetCheckUnconnected()
     * @see #setCheckUnconnected(boolean)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_CheckUnconnected()
     * @model default="true" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
     *        extendedMetaData="kind='attribute' name='check-unconnected'"
     * @generated
     */
    boolean isCheckUnconnected();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Check Unconnected</em>' attribute.
     * @see #isSetCheckUnconnected()
     * @see #unsetCheckUnconnected()
     * @see #isCheckUnconnected()
     * @generated
     */
    void setCheckUnconnected(boolean value);

    /**
     * Unsets the value of the '{@link org.omnetpp.ned.model.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isSetCheckUnconnected()
     * @see #isCheckUnconnected()
     * @see #setCheckUnconnected(boolean)
     * @generated
     */
    void unsetCheckUnconnected();

    /**
     * Returns whether the value of the '{@link org.omnetpp.ned.model.emf.Connections#isCheckUnconnected <em>Check Unconnected</em>}' attribute is set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return whether the value of the '<em>Check Unconnected</em>' attribute is set.
     * @see #unsetCheckUnconnected()
     * @see #isCheckUnconnected()
     * @see #setCheckUnconnected(boolean)
     * @generated
     */
    boolean isSetCheckUnconnected();

    /**
     * Returns the value of the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Right Comment</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Right Comment</em>' attribute.
     * @see #setRightComment(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connections#getRightComment <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Right Comment</em>' attribute.
     * @see #getRightComment()
     * @generated
     */
    void setRightComment(String value);

    /**
     * Returns the value of the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Trailing Comment</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Trailing Comment</em>' attribute.
     * @see #setTrailingComment(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnections_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connections#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

} // Connections
