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
 * A representation of the model object '<em><b>File</b></em>'.
 * <!-- end-user-doc -->
 *
 * <!-- begin-model-doc -->
 * Represents a NED file
 * <!-- end-model-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getImport <em>Import</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getChannel <em>Channel</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getSimpleModule <em>Simple Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getCompoundModule <em>Compound Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getNetwork <em>Network</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.NedFile#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile()
 * @model extendedMetaData="name='ned-file' kind='elementOnly'"
 * @generated
 */
public interface NedFile extends EObject {
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_Group()
     * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
     *        extendedMetaData="kind='group' name='group:0'"
     * @generated
     */
    FeatureMap getGroup();

    /**
     * Returns the value of the '<em><b>Import</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Import}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Import</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Import</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_Import()
     * @model type="org.omnetpp.ned.model.emf.Import" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='import' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getImport();

    /**
     * Returns the value of the '<em><b>Channel</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Channel}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Channel</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Channel</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_Channel()
     * @model type="org.omnetpp.ned.model.emf.Channel" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='channel' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getChannel();

    /**
     * Returns the value of the '<em><b>Simple Module</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.SimpleModule}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Simple Module</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Simple Module</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_SimpleModule()
     * @model type="org.omnetpp.ned.model.emf.SimpleModule" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='simple-module' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getSimpleModule();

    /**
     * Returns the value of the '<em><b>Compound Module</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.CompoundModule}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Compound Module</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Compound Module</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_CompoundModule()
     * @model type="org.omnetpp.ned.model.emf.CompoundModule" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='compound-module' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getCompoundModule();

    /**
     * Returns the value of the '<em><b>Network</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Network}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Network</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Network</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_Network()
     * @model type="org.omnetpp.ned.model.emf.Network" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
     *        extendedMetaData="kind='element' name='network' namespace='##targetNamespace' group='#group:0'"
     * @generated
     */
    EList getNetwork();

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.NedFile#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.NedFile#getRightComment <em>Right Comment</em>}' attribute.
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getNedFile_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.NedFile#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

} // NedFile
