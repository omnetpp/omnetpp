/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Gatesize</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.Gatesize#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Gatesize#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Gatesize#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Gatesize#getTrailingComment <em>Trailing Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Gatesize#getVectorSize <em>Vector Size</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getGatesize()
 * @model extendedMetaData="name='gatesize' kind='empty'"
 * @generated
 */
public interface Gatesize extends EObject {
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getGatesize_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Gatesize#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getGatesize_Name()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='name'"
     * @generated
     */
    String getName();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Gatesize#getName <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Name</em>' attribute.
     * @see #getName()
     * @generated
     */
    void setName(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getGatesize_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Gatesize#getRightComment <em>Right Comment</em>}' attribute.
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getGatesize_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Gatesize#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getGatesize_VectorSize()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
     *        extendedMetaData="kind='attribute' name='vector-size'"
     * @generated
     */
    String getVectorSize();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Gatesize#getVectorSize <em>Vector Size</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Vector Size</em>' attribute.
     * @see #getVectorSize()
     * @generated
     */
    void setVectorSize(String value);

} // Gatesize
