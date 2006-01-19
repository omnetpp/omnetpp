/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Submodule</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getSubstparams <em>Substparams</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getGatesizes <em>Gatesizes</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getDisplayString <em>Display String</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getLikeParam <em>Like Param</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getTrailingComment <em>Trailing Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getTypeName <em>Type Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Submodule#getVectorSize <em>Vector Size</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule()
 * @model extendedMetaData="name='submodule' kind='elementOnly'"
 * @generated
 */
public interface Submodule extends EObject {
    /**
     * Returns the value of the '<em><b>Substparams</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Substparams}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Substparams</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Substparams</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_Substparams()
     * @model type="org.omnetpp.ned.model.emf.Substparams" containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='substparams' namespace='##targetNamespace'"
     * @generated
     */
    EList getSubstparams();

    /**
     * Returns the value of the '<em><b>Gatesizes</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Gatesizes}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Gatesizes</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Gatesizes</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_Gatesizes()
     * @model type="org.omnetpp.ned.model.emf.Gatesizes" containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='gatesizes' namespace='##targetNamespace'"
     * @generated
     */
    EList getGatesizes();

    /**
     * Returns the value of the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Display String</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Display String</em>' containment reference.
     * @see #setDisplayString(DisplayString)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_DisplayString()
     * @model containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='display-string' namespace='##targetNamespace'"
     * @generated
     */
    DisplayString getDisplayString();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getDisplayString <em>Display String</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Display String</em>' containment reference.
     * @see #getDisplayString()
     * @generated
     */
    void setDisplayString(DisplayString value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_LikeParam()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
     *        extendedMetaData="kind='attribute' name='like-param'"
     * @generated
     */
    String getLikeParam();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getLikeParam <em>Like Param</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Like Param</em>' attribute.
     * @see #getLikeParam()
     * @generated
     */
    void setLikeParam(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_Name()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='name'"
     * @generated
     */
    String getName();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getName <em>Name</em>}' attribute.
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getRightComment <em>Right Comment</em>}' attribute.
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

    /**
     * Returns the value of the '<em><b>Type Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Type Name</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Type Name</em>' attribute.
     * @see #setTypeName(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_TypeName()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='type-name'"
     * @generated
     */
    String getTypeName();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getTypeName <em>Type Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Type Name</em>' attribute.
     * @see #getTypeName()
     * @generated
     */
    void setTypeName(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getSubmodule_VectorSize()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='vector-size'"
     * @generated
     */
    String getVectorSize();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Submodule#getVectorSize <em>Vector Size</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Vector Size</em>' attribute.
     * @see #getVectorSize()
     * @generated
     */
    void setVectorSize(String value);

} // Submodule
