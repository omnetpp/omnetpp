/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned.model.emf.NedPackage
 * @generated
 */
public interface NedFactory extends EFactory {
    /**
     * The singleton instance of the factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    NedFactory eINSTANCE = new org.omnetpp.ned.model.emf.impl.NedFactoryImpl();

    /**
     * Returns a new object of class '<em>Channel</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Channel</em>'.
     * @generated
     */
    Channel createChannel();

    /**
     * Returns a new object of class '<em>Channel Attr</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Channel Attr</em>'.
     * @generated
     */
    ChannelAttr createChannelAttr();

    /**
     * Returns a new object of class '<em>Compound Module</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Compound Module</em>'.
     * @generated
     */
    CompoundModule createCompoundModule();

    /**
     * Returns a new object of class '<em>Conn Attr</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Conn Attr</em>'.
     * @generated
     */
    ConnAttr createConnAttr();

    /**
     * Returns a new object of class '<em>Connection</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Connection</em>'.
     * @generated
     */
    Connection createConnection();

    /**
     * Returns a new object of class '<em>Connections</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Connections</em>'.
     * @generated
     */
    Connections createConnections();

    /**
     * Returns a new object of class '<em>Display String</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Display String</em>'.
     * @generated
     */
    DisplayString createDisplayString();

    /**
     * Returns a new object of class '<em>For Loop</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>For Loop</em>'.
     * @generated
     */
    ForLoop createForLoop();

    /**
     * Returns a new object of class '<em>Gate</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Gate</em>'.
     * @generated
     */
    Gate createGate();

    /**
     * Returns a new object of class '<em>Gates</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Gates</em>'.
     * @generated
     */
    Gates createGates();

    /**
     * Returns a new object of class '<em>Gatesize</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Gatesize</em>'.
     * @generated
     */
    Gatesize createGatesize();

    /**
     * Returns a new object of class '<em>Gatesizes</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Gatesizes</em>'.
     * @generated
     */
    Gatesizes createGatesizes();

    /**
     * Returns a new object of class '<em>Import</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Import</em>'.
     * @generated
     */
    Import createImport();

    /**
     * Returns a new object of class '<em>File</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>File</em>'.
     * @generated
     */
    NedFile createNedFile();

    /**
     * Returns a new object of class '<em>Network</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Network</em>'.
     * @generated
     */
    Network createNetwork();

    /**
     * Returns a new object of class '<em>Param</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Param</em>'.
     * @generated
     */
    Param createParam();

    /**
     * Returns a new object of class '<em>Params</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Params</em>'.
     * @generated
     */
    Params createParams();

    /**
     * Returns a new object of class '<em>Simple Module</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Simple Module</em>'.
     * @generated
     */
    SimpleModule createSimpleModule();

    /**
     * Returns a new object of class '<em>Submodule</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Submodule</em>'.
     * @generated
     */
    Submodule createSubmodule();

    /**
     * Returns a new object of class '<em>Submodules</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Submodules</em>'.
     * @generated
     */
    Submodules createSubmodules();

    /**
     * Returns a new object of class '<em>Substparam</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Substparam</em>'.
     * @generated
     */
    Substparam createSubstparam();

    /**
     * Returns a new object of class '<em>Substparams</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return a new object of class '<em>Substparams</em>'.
     * @generated
     */
    Substparams createSubstparams();

    /**
     * Returns the package supported by this factory.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the package supported by this factory.
     * @generated
     */
    NedPackage getNedPackage();

} //NedFactory
