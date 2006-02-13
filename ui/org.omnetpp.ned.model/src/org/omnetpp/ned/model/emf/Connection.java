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
 * A representation of the model object '<em><b>Connection</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getConnAttr <em>Conn Attr</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getDisplayString <em>Display String</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getCondition <em>Condition</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getDestGate <em>Dest Gate</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getDestGateIndex <em>Dest Gate Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getDestModule <em>Dest Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getDestModuleIndex <em>Dest Module Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#isRightToLeft <em>Right To Left</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getSrcGate <em>Src Gate</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getSrcGateIndex <em>Src Gate Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getSrcModule <em>Src Module</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getSrcModuleIndex <em>Src Module Index</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.Connection#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getConnection()
 * @model extendedMetaData="name='connection' kind='elementOnly'"
 * @generated
 */
public interface Connection extends EObject {
    /**
     * Returns the value of the '<em><b>Conn Attr</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.ConnAttr}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Conn Attr</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Conn Attr</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_ConnAttr()
     * @model type="org.omnetpp.ned.model.emf.ConnAttr" containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='conn-attr' namespace='##targetNamespace'"
     * @generated
     */
    EList getConnAttr();

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_DisplayString()
     * @model containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='display-string' namespace='##targetNamespace'"
     * @generated
     */
    DisplayString getDisplayString();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getDisplayString <em>Display String</em>}' containment reference.
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
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

    /**
     * Returns the value of the '<em><b>Condition</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Condition</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Condition</em>' attribute.
     * @see #setCondition(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_Condition()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='condition'"
     * @generated
     */
    String getCondition();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getCondition <em>Condition</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Condition</em>' attribute.
     * @see #getCondition()
     * @generated
     */
    void setCondition(String value);

    /**
     * Returns the value of the '<em><b>Dest Gate</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Dest Gate</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Dest Gate</em>' attribute.
     * @see #setDestGate(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_DestGate()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='dest-gate'"
     * @generated
     */
    String getDestGate();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getDestGate <em>Dest Gate</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Dest Gate</em>' attribute.
     * @see #getDestGate()
     * @generated
     */
    void setDestGate(String value);

    /**
     * Returns the value of the '<em><b>Dest Gate Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Dest Gate Index</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Dest Gate Index</em>' attribute.
     * @see #setDestGateIndex(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_DestGateIndex()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='dest-gate-index'"
     * @generated
     */
    String getDestGateIndex();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getDestGateIndex <em>Dest Gate Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Dest Gate Index</em>' attribute.
     * @see #getDestGateIndex()
     * @generated
     */
    void setDestGateIndex(String value);

    /**
     * Returns the value of the '<em><b>Dest Gate Plusplus</b></em>' attribute.
     * The default value is <code>"false"</code>.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Dest Gate Plusplus</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Dest Gate Plusplus</em>' attribute.
     * @see #isSetDestGatePlusplus()
     * @see #unsetDestGatePlusplus()
     * @see #setDestGatePlusplus(boolean)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_DestGatePlusplus()
     * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
     *        extendedMetaData="kind='attribute' name='dest-gate-plusplus'"
     * @generated
     */
    boolean isDestGatePlusplus();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Dest Gate Plusplus</em>' attribute.
     * @see #isSetDestGatePlusplus()
     * @see #unsetDestGatePlusplus()
     * @see #isDestGatePlusplus()
     * @generated
     */
    void setDestGatePlusplus(boolean value);

    /**
     * Unsets the value of the '{@link org.omnetpp.ned.model.emf.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isSetDestGatePlusplus()
     * @see #isDestGatePlusplus()
     * @see #setDestGatePlusplus(boolean)
     * @generated
     */
    void unsetDestGatePlusplus();

    /**
     * Returns whether the value of the '{@link org.omnetpp.ned.model.emf.Connection#isDestGatePlusplus <em>Dest Gate Plusplus</em>}' attribute is set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return whether the value of the '<em>Dest Gate Plusplus</em>' attribute is set.
     * @see #unsetDestGatePlusplus()
     * @see #isDestGatePlusplus()
     * @see #setDestGatePlusplus(boolean)
     * @generated
     */
    boolean isSetDestGatePlusplus();

    /**
     * Returns the value of the '<em><b>Dest Module</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Dest Module</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Dest Module</em>' attribute.
     * @see #setDestModule(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_DestModule()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
     *        extendedMetaData="kind='attribute' name='dest-module'"
     * @generated
     */
    String getDestModule();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getDestModule <em>Dest Module</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Dest Module</em>' attribute.
     * @see #getDestModule()
     * @generated
     */
    void setDestModule(String value);

    /**
     * Returns the value of the '<em><b>Dest Module Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Dest Module Index</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Dest Module Index</em>' attribute.
     * @see #setDestModuleIndex(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_DestModuleIndex()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='dest-module-index'"
     * @generated
     */
    String getDestModuleIndex();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getDestModuleIndex <em>Dest Module Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Dest Module Index</em>' attribute.
     * @see #getDestModuleIndex()
     * @generated
     */
    void setDestModuleIndex(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getRightComment <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Right Comment</em>' attribute.
     * @see #getRightComment()
     * @generated
     */
    void setRightComment(String value);

    /**
     * Returns the value of the '<em><b>Right To Left</b></em>' attribute.
     * The default value is <code>"false"</code>.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Right To Left</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Right To Left</em>' attribute.
     * @see #isSetRightToLeft()
     * @see #unsetRightToLeft()
     * @see #setRightToLeft(boolean)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_RightToLeft()
     * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
     *        extendedMetaData="kind='attribute' name='right-to-left'"
     * @generated
     */
    boolean isRightToLeft();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#isRightToLeft <em>Right To Left</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Right To Left</em>' attribute.
     * @see #isSetRightToLeft()
     * @see #unsetRightToLeft()
     * @see #isRightToLeft()
     * @generated
     */
    void setRightToLeft(boolean value);

    /**
     * Unsets the value of the '{@link org.omnetpp.ned.model.emf.Connection#isRightToLeft <em>Right To Left</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isSetRightToLeft()
     * @see #isRightToLeft()
     * @see #setRightToLeft(boolean)
     * @generated
     */
    void unsetRightToLeft();

    /**
     * Returns whether the value of the '{@link org.omnetpp.ned.model.emf.Connection#isRightToLeft <em>Right To Left</em>}' attribute is set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return whether the value of the '<em>Right To Left</em>' attribute is set.
     * @see #unsetRightToLeft()
     * @see #isRightToLeft()
     * @see #setRightToLeft(boolean)
     * @generated
     */
    boolean isSetRightToLeft();

    /**
     * Returns the value of the '<em><b>Src Gate</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Src Gate</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Src Gate</em>' attribute.
     * @see #setSrcGate(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_SrcGate()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='src-gate'"
     * @generated
     */
    String getSrcGate();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getSrcGate <em>Src Gate</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Src Gate</em>' attribute.
     * @see #getSrcGate()
     * @generated
     */
    void setSrcGate(String value);

    /**
     * Returns the value of the '<em><b>Src Gate Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Src Gate Index</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Src Gate Index</em>' attribute.
     * @see #setSrcGateIndex(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_SrcGateIndex()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='src-gate-index'"
     * @generated
     */
    String getSrcGateIndex();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getSrcGateIndex <em>Src Gate Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Src Gate Index</em>' attribute.
     * @see #getSrcGateIndex()
     * @generated
     */
    void setSrcGateIndex(String value);

    /**
     * Returns the value of the '<em><b>Src Gate Plusplus</b></em>' attribute.
     * The default value is <code>"false"</code>.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Src Gate Plusplus</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Src Gate Plusplus</em>' attribute.
     * @see #isSetSrcGatePlusplus()
     * @see #unsetSrcGatePlusplus()
     * @see #setSrcGatePlusplus(boolean)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_SrcGatePlusplus()
     * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
     *        extendedMetaData="kind='attribute' name='src-gate-plusplus'"
     * @generated
     */
    boolean isSrcGatePlusplus();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Src Gate Plusplus</em>' attribute.
     * @see #isSetSrcGatePlusplus()
     * @see #unsetSrcGatePlusplus()
     * @see #isSrcGatePlusplus()
     * @generated
     */
    void setSrcGatePlusplus(boolean value);

    /**
     * Unsets the value of the '{@link org.omnetpp.ned.model.emf.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #isSetSrcGatePlusplus()
     * @see #isSrcGatePlusplus()
     * @see #setSrcGatePlusplus(boolean)
     * @generated
     */
    void unsetSrcGatePlusplus();

    /**
     * Returns whether the value of the '{@link org.omnetpp.ned.model.emf.Connection#isSrcGatePlusplus <em>Src Gate Plusplus</em>}' attribute is set.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return whether the value of the '<em>Src Gate Plusplus</em>' attribute is set.
     * @see #unsetSrcGatePlusplus()
     * @see #isSrcGatePlusplus()
     * @see #setSrcGatePlusplus(boolean)
     * @generated
     */
    boolean isSetSrcGatePlusplus();

    /**
     * Returns the value of the '<em><b>Src Module</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Src Module</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Src Module</em>' attribute.
     * @see #setSrcModule(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_SrcModule()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN"
     *        extendedMetaData="kind='attribute' name='src-module'"
     * @generated
     */
    String getSrcModule();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getSrcModule <em>Src Module</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Src Module</em>' attribute.
     * @see #getSrcModule()
     * @generated
     */
    void setSrcModule(String value);

    /**
     * Returns the value of the '<em><b>Src Module Index</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Src Module Index</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Src Module Index</em>' attribute.
     * @see #setSrcModuleIndex(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_SrcModuleIndex()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='src-module-index'"
     * @generated
     */
    String getSrcModuleIndex();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getSrcModuleIndex <em>Src Module Index</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Src Module Index</em>' attribute.
     * @see #getSrcModuleIndex()
     * @generated
     */
    void setSrcModuleIndex(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getConnection_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.Connection#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

} // Connection
