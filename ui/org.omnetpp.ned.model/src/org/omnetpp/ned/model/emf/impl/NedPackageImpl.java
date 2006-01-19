/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf.impl;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

import org.eclipse.emf.ecore.impl.EPackageImpl;

import org.eclipse.emf.ecore.xml.type.XMLTypePackage;

import org.eclipse.emf.ecore.xml.type.impl.XMLTypePackageImpl;

import org.omnetpp.ned.model.emf.Channel;
import org.omnetpp.ned.model.emf.ChannelAttr;
import org.omnetpp.ned.model.emf.CompoundModule;
import org.omnetpp.ned.model.emf.ConnAttr;
import org.omnetpp.ned.model.emf.Connection;
import org.omnetpp.ned.model.emf.Connections;
import org.omnetpp.ned.model.emf.DisplayString;
import org.omnetpp.ned.model.emf.ForLoop;
import org.omnetpp.ned.model.emf.Gate;
import org.omnetpp.ned.model.emf.Gates;
import org.omnetpp.ned.model.emf.Gatesize;
import org.omnetpp.ned.model.emf.Gatesizes;
import org.omnetpp.ned.model.emf.Import;
import org.omnetpp.ned.model.emf.NedFactory;
import org.omnetpp.ned.model.emf.NedFile;
import org.omnetpp.ned.model.emf.NedPackage;
import org.omnetpp.ned.model.emf.Network;
import org.omnetpp.ned.model.emf.Param;
import org.omnetpp.ned.model.emf.Params;
import org.omnetpp.ned.model.emf.SimpleModule;
import org.omnetpp.ned.model.emf.Submodule;
import org.omnetpp.ned.model.emf.Submodules;
import org.omnetpp.ned.model.emf.Substparam;
import org.omnetpp.ned.model.emf.Substparams;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Package</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class NedPackageImpl extends EPackageImpl implements NedPackage {
    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass channelEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass channelAttrEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass compoundModuleEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass connAttrEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass connectionEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass connectionsEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass displayStringEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass forLoopEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass gateEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass gatesEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass gatesizeEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass gatesizesEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass importEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass nedFileEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass networkEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass paramEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass paramsEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass simpleModuleEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass submoduleEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass submodulesEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass substparamEClass = null;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private EClass substparamsEClass = null;

    /**
     * Creates an instance of the model <b>Package</b>, registered with
     * {@link org.eclipse.emf.ecore.EPackage.Registry EPackage.Registry} by the package
     * package URI value.
     * <p>Note: the correct way to create the package is via the static
     * factory method {@link #init init()}, which also performs
     * initialization of the package, or returns the registered package,
     * if one already exists.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.eclipse.emf.ecore.EPackage.Registry
     * @see org.omnetpp.ned.model.emf.NedPackage#eNS_URI
     * @see #init()
     * @generated
     */
    private NedPackageImpl() {
        super(eNS_URI, NedFactory.eINSTANCE);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private static boolean isInited = false;

    /**
     * Creates, registers, and initializes the <b>Package</b> for this
     * model, and for any others upon which it depends.  Simple
     * dependencies are satisfied by calling this method on all
     * dependent packages before doing anything else.  This method drives
     * initialization for interdependent packages directly, in parallel
     * with this package, itself.
     * <p>Of this package and its interdependencies, all packages which
     * have not yet been registered by their URI values are first created
     * and registered.  The packages are then initialized in two steps:
     * meta-model objects for all of the packages are created before any
     * are initialized, since one package's meta-model objects may refer to
     * those of another.
     * <p>Invocation of this method will not affect any packages that have
     * already been initialized.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #eNS_URI
     * @see #createPackageContents()
     * @see #initializePackageContents()
     * @generated
     */
    public static NedPackage init() {
        if (isInited) return (NedPackage)EPackage.Registry.INSTANCE.getEPackage(NedPackage.eNS_URI);

        // Obtain or create and register package
        NedPackageImpl theNedPackage = (NedPackageImpl)(EPackage.Registry.INSTANCE.getEPackage(eNS_URI) instanceof NedPackageImpl ? EPackage.Registry.INSTANCE.getEPackage(eNS_URI) : new NedPackageImpl());

        isInited = true;

        // Initialize simple dependencies
        XMLTypePackageImpl.init();

        // Create package meta-data objects
        theNedPackage.createPackageContents();

        // Initialize created meta-data
        theNedPackage.initializePackageContents();

        // Mark meta-data to indicate it can't be changed
        theNedPackage.freeze();

        return theNedPackage;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getChannel() {
        return channelEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getChannel_ChannelAttr() {
        return (EReference)channelEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getChannel_DisplayString() {
        return (EReference)channelEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannel_BannerComment() {
        return (EAttribute)channelEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannel_Name() {
        return (EAttribute)channelEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannel_RightComment() {
        return (EAttribute)channelEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannel_TrailingComment() {
        return (EAttribute)channelEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getChannelAttr() {
        return channelAttrEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannelAttr_BannerComment() {
        return (EAttribute)channelAttrEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannelAttr_Name() {
        return (EAttribute)channelAttrEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannelAttr_RightComment() {
        return (EAttribute)channelAttrEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannelAttr_TrailingComment() {
        return (EAttribute)channelAttrEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getChannelAttr_Value() {
        return (EAttribute)channelAttrEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getCompoundModule() {
        return compoundModuleEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getCompoundModule_Params() {
        return (EReference)compoundModuleEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getCompoundModule_Gates() {
        return (EReference)compoundModuleEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getCompoundModule_Submodules() {
        return (EReference)compoundModuleEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getCompoundModule_Connections() {
        return (EReference)compoundModuleEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getCompoundModule_DisplayString() {
        return (EReference)compoundModuleEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getCompoundModule_BannerComment() {
        return (EAttribute)compoundModuleEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getCompoundModule_Name() {
        return (EAttribute)compoundModuleEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getCompoundModule_RightComment() {
        return (EAttribute)compoundModuleEClass.getEStructuralFeatures().get(7);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getCompoundModule_TrailingComment() {
        return (EAttribute)compoundModuleEClass.getEStructuralFeatures().get(8);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getConnAttr() {
        return connAttrEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnAttr_BannerComment() {
        return (EAttribute)connAttrEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnAttr_Name() {
        return (EAttribute)connAttrEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnAttr_RightComment() {
        return (EAttribute)connAttrEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnAttr_TrailingComment() {
        return (EAttribute)connAttrEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnAttr_Value() {
        return (EAttribute)connAttrEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getConnection() {
        return connectionEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getConnection_ConnAttr() {
        return (EReference)connectionEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getConnection_DisplayString() {
        return (EReference)connectionEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_BannerComment() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_Condition() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_DestGate() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_DestGateIndex() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_DestGatePlusplus() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_DestModule() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(7);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_DestModuleIndex() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(8);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_RightComment() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(9);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_RightToLeft() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(10);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_SrcGate() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(11);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_SrcGateIndex() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(12);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_SrcGatePlusplus() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(13);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_SrcModule() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(14);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_SrcModuleIndex() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(15);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnection_TrailingComment() {
        return (EAttribute)connectionEClass.getEStructuralFeatures().get(16);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getConnections() {
        return connectionsEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnections_Group() {
        return (EAttribute)connectionsEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getConnections_Connection() {
        return (EReference)connectionsEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getConnections_ForLoop() {
        return (EReference)connectionsEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnections_BannerComment() {
        return (EAttribute)connectionsEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnections_CheckUnconnected() {
        return (EAttribute)connectionsEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnections_RightComment() {
        return (EAttribute)connectionsEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getConnections_TrailingComment() {
        return (EAttribute)connectionsEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getDisplayString() {
        return displayStringEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getDisplayString_BannerComment() {
        return (EAttribute)displayStringEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getDisplayString_RightComment() {
        return (EAttribute)displayStringEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getDisplayString_TrailingComment() {
        return (EAttribute)displayStringEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getDisplayString_Value() {
        return (EAttribute)displayStringEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getForLoop() {
        return forLoopEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getForLoop_ForLoop() {
        return (EReference)forLoopEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getForLoop_Connection() {
        return (EReference)forLoopEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getForLoop_BannerComment() {
        return (EAttribute)forLoopEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getForLoop_FromValue() {
        return (EAttribute)forLoopEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getForLoop_ParamName() {
        return (EAttribute)forLoopEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getForLoop_RightComment() {
        return (EAttribute)forLoopEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getForLoop_ToValue() {
        return (EAttribute)forLoopEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getForLoop_TrailingComment() {
        return (EAttribute)forLoopEClass.getEStructuralFeatures().get(7);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getGate() {
        return gateEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGate_BannerComment() {
        return (EAttribute)gateEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGate_Name() {
        return (EAttribute)gateEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGate_Output() {
        return (EAttribute)gateEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGate_RightComment() {
        return (EAttribute)gateEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGate_TrailingComment() {
        return (EAttribute)gateEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGate_Vector() {
        return (EAttribute)gateEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getGates() {
        return gatesEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getGates_Gate() {
        return (EReference)gatesEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGates_BannerComment() {
        return (EAttribute)gatesEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGates_RightComment() {
        return (EAttribute)gatesEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGates_TrailingComment() {
        return (EAttribute)gatesEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getGatesize() {
        return gatesizeEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesize_BannerComment() {
        return (EAttribute)gatesizeEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesize_Name() {
        return (EAttribute)gatesizeEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesize_RightComment() {
        return (EAttribute)gatesizeEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesize_TrailingComment() {
        return (EAttribute)gatesizeEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesize_VectorSize() {
        return (EAttribute)gatesizeEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getGatesizes() {
        return gatesizesEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getGatesizes_Gatesize() {
        return (EReference)gatesizesEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesizes_BannerComment() {
        return (EAttribute)gatesizesEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesizes_Condition() {
        return (EAttribute)gatesizesEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesizes_RightComment() {
        return (EAttribute)gatesizesEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getGatesizes_TrailingComment() {
        return (EAttribute)gatesizesEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getImport() {
        return importEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getImport_BannerComment() {
        return (EAttribute)importEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getImport_Filename() {
        return (EAttribute)importEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getImport_RightComment() {
        return (EAttribute)importEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getImport_TrailingComment() {
        return (EAttribute)importEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getNedFile() {
        return nedFileEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNedFile_Group() {
        return (EAttribute)nedFileEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getNedFile_Import() {
        return (EReference)nedFileEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getNedFile_Channel() {
        return (EReference)nedFileEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getNedFile_SimpleModule() {
        return (EReference)nedFileEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getNedFile_CompoundModule() {
        return (EReference)nedFileEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getNedFile_Network() {
        return (EReference)nedFileEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNedFile_BannerComment() {
        return (EAttribute)nedFileEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNedFile_RightComment() {
        return (EAttribute)nedFileEClass.getEStructuralFeatures().get(7);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNedFile_TrailingComment() {
        return (EAttribute)nedFileEClass.getEStructuralFeatures().get(8);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getNetwork() {
        return networkEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getNetwork_Substparams() {
        return (EReference)networkEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNetwork_BannerComment() {
        return (EAttribute)networkEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNetwork_Name() {
        return (EAttribute)networkEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNetwork_RightComment() {
        return (EAttribute)networkEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNetwork_TrailingComment() {
        return (EAttribute)networkEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getNetwork_TypeName() {
        return (EAttribute)networkEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getParam() {
        return paramEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParam_BannerComment() {
        return (EAttribute)paramEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParam_DataType() {
        return (EAttribute)paramEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParam_Name() {
        return (EAttribute)paramEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParam_RightComment() {
        return (EAttribute)paramEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParam_TrailingComment() {
        return (EAttribute)paramEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getParams() {
        return paramsEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getParams_Param() {
        return (EReference)paramsEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParams_BannerComment() {
        return (EAttribute)paramsEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParams_RightComment() {
        return (EAttribute)paramsEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getParams_TrailingComment() {
        return (EAttribute)paramsEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getSimpleModule() {
        return simpleModuleEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSimpleModule_Params() {
        return (EReference)simpleModuleEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSimpleModule_Gates() {
        return (EReference)simpleModuleEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSimpleModule_DisplayString() {
        return (EReference)simpleModuleEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSimpleModule_BannerComment() {
        return (EAttribute)simpleModuleEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSimpleModule_Name() {
        return (EAttribute)simpleModuleEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSimpleModule_RightComment() {
        return (EAttribute)simpleModuleEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSimpleModule_TrailingComment() {
        return (EAttribute)simpleModuleEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getSubmodule() {
        return submoduleEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSubmodule_Substparams() {
        return (EReference)submoduleEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSubmodule_Gatesizes() {
        return (EReference)submoduleEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSubmodule_DisplayString() {
        return (EReference)submoduleEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_BannerComment() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_LikeParam() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_Name() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(5);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_RightComment() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(6);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_TrailingComment() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(7);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_TypeName() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(8);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodule_VectorSize() {
        return (EAttribute)submoduleEClass.getEStructuralFeatures().get(9);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getSubmodules() {
        return submodulesEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSubmodules_Submodule() {
        return (EReference)submodulesEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodules_BannerComment() {
        return (EAttribute)submodulesEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodules_RightComment() {
        return (EAttribute)submodulesEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubmodules_TrailingComment() {
        return (EAttribute)submodulesEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getSubstparam() {
        return substparamEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparam_BannerComment() {
        return (EAttribute)substparamEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparam_Name() {
        return (EAttribute)substparamEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparam_RightComment() {
        return (EAttribute)substparamEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparam_TrailingComment() {
        return (EAttribute)substparamEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparam_Value() {
        return (EAttribute)substparamEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EClass getSubstparams() {
        return substparamsEClass;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EReference getSubstparams_Substparam() {
        return (EReference)substparamsEClass.getEStructuralFeatures().get(0);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparams_BannerComment() {
        return (EAttribute)substparamsEClass.getEStructuralFeatures().get(1);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparams_Condition() {
        return (EAttribute)substparamsEClass.getEStructuralFeatures().get(2);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparams_RightComment() {
        return (EAttribute)substparamsEClass.getEStructuralFeatures().get(3);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EAttribute getSubstparams_TrailingComment() {
        return (EAttribute)substparamsEClass.getEStructuralFeatures().get(4);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NedFactory getNedFactory() {
        return (NedFactory)getEFactoryInstance();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private boolean isCreated = false;

    /**
     * Creates the meta-model objects for the package.  This method is
     * guarded to have no affect on any invocation but its first.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void createPackageContents() {
        if (isCreated) return;
        isCreated = true;

        // Create classes and their features
        channelEClass = createEClass(CHANNEL);
        createEReference(channelEClass, CHANNEL__CHANNEL_ATTR);
        createEReference(channelEClass, CHANNEL__DISPLAY_STRING);
        createEAttribute(channelEClass, CHANNEL__BANNER_COMMENT);
        createEAttribute(channelEClass, CHANNEL__NAME);
        createEAttribute(channelEClass, CHANNEL__RIGHT_COMMENT);
        createEAttribute(channelEClass, CHANNEL__TRAILING_COMMENT);

        channelAttrEClass = createEClass(CHANNEL_ATTR);
        createEAttribute(channelAttrEClass, CHANNEL_ATTR__BANNER_COMMENT);
        createEAttribute(channelAttrEClass, CHANNEL_ATTR__NAME);
        createEAttribute(channelAttrEClass, CHANNEL_ATTR__RIGHT_COMMENT);
        createEAttribute(channelAttrEClass, CHANNEL_ATTR__TRAILING_COMMENT);
        createEAttribute(channelAttrEClass, CHANNEL_ATTR__VALUE);

        compoundModuleEClass = createEClass(COMPOUND_MODULE);
        createEReference(compoundModuleEClass, COMPOUND_MODULE__PARAMS);
        createEReference(compoundModuleEClass, COMPOUND_MODULE__GATES);
        createEReference(compoundModuleEClass, COMPOUND_MODULE__SUBMODULES);
        createEReference(compoundModuleEClass, COMPOUND_MODULE__CONNECTIONS);
        createEReference(compoundModuleEClass, COMPOUND_MODULE__DISPLAY_STRING);
        createEAttribute(compoundModuleEClass, COMPOUND_MODULE__BANNER_COMMENT);
        createEAttribute(compoundModuleEClass, COMPOUND_MODULE__NAME);
        createEAttribute(compoundModuleEClass, COMPOUND_MODULE__RIGHT_COMMENT);
        createEAttribute(compoundModuleEClass, COMPOUND_MODULE__TRAILING_COMMENT);

        connAttrEClass = createEClass(CONN_ATTR);
        createEAttribute(connAttrEClass, CONN_ATTR__BANNER_COMMENT);
        createEAttribute(connAttrEClass, CONN_ATTR__NAME);
        createEAttribute(connAttrEClass, CONN_ATTR__RIGHT_COMMENT);
        createEAttribute(connAttrEClass, CONN_ATTR__TRAILING_COMMENT);
        createEAttribute(connAttrEClass, CONN_ATTR__VALUE);

        connectionEClass = createEClass(CONNECTION);
        createEReference(connectionEClass, CONNECTION__CONN_ATTR);
        createEReference(connectionEClass, CONNECTION__DISPLAY_STRING);
        createEAttribute(connectionEClass, CONNECTION__BANNER_COMMENT);
        createEAttribute(connectionEClass, CONNECTION__CONDITION);
        createEAttribute(connectionEClass, CONNECTION__DEST_GATE);
        createEAttribute(connectionEClass, CONNECTION__DEST_GATE_INDEX);
        createEAttribute(connectionEClass, CONNECTION__DEST_GATE_PLUSPLUS);
        createEAttribute(connectionEClass, CONNECTION__DEST_MODULE);
        createEAttribute(connectionEClass, CONNECTION__DEST_MODULE_INDEX);
        createEAttribute(connectionEClass, CONNECTION__RIGHT_COMMENT);
        createEAttribute(connectionEClass, CONNECTION__RIGHT_TO_LEFT);
        createEAttribute(connectionEClass, CONNECTION__SRC_GATE);
        createEAttribute(connectionEClass, CONNECTION__SRC_GATE_INDEX);
        createEAttribute(connectionEClass, CONNECTION__SRC_GATE_PLUSPLUS);
        createEAttribute(connectionEClass, CONNECTION__SRC_MODULE);
        createEAttribute(connectionEClass, CONNECTION__SRC_MODULE_INDEX);
        createEAttribute(connectionEClass, CONNECTION__TRAILING_COMMENT);

        connectionsEClass = createEClass(CONNECTIONS);
        createEAttribute(connectionsEClass, CONNECTIONS__GROUP);
        createEReference(connectionsEClass, CONNECTIONS__CONNECTION);
        createEReference(connectionsEClass, CONNECTIONS__FOR_LOOP);
        createEAttribute(connectionsEClass, CONNECTIONS__BANNER_COMMENT);
        createEAttribute(connectionsEClass, CONNECTIONS__CHECK_UNCONNECTED);
        createEAttribute(connectionsEClass, CONNECTIONS__RIGHT_COMMENT);
        createEAttribute(connectionsEClass, CONNECTIONS__TRAILING_COMMENT);

        displayStringEClass = createEClass(DISPLAY_STRING);
        createEAttribute(displayStringEClass, DISPLAY_STRING__BANNER_COMMENT);
        createEAttribute(displayStringEClass, DISPLAY_STRING__RIGHT_COMMENT);
        createEAttribute(displayStringEClass, DISPLAY_STRING__TRAILING_COMMENT);
        createEAttribute(displayStringEClass, DISPLAY_STRING__VALUE);

        forLoopEClass = createEClass(FOR_LOOP);
        createEReference(forLoopEClass, FOR_LOOP__FOR_LOOP);
        createEReference(forLoopEClass, FOR_LOOP__CONNECTION);
        createEAttribute(forLoopEClass, FOR_LOOP__BANNER_COMMENT);
        createEAttribute(forLoopEClass, FOR_LOOP__FROM_VALUE);
        createEAttribute(forLoopEClass, FOR_LOOP__PARAM_NAME);
        createEAttribute(forLoopEClass, FOR_LOOP__RIGHT_COMMENT);
        createEAttribute(forLoopEClass, FOR_LOOP__TO_VALUE);
        createEAttribute(forLoopEClass, FOR_LOOP__TRAILING_COMMENT);

        gateEClass = createEClass(GATE);
        createEAttribute(gateEClass, GATE__BANNER_COMMENT);
        createEAttribute(gateEClass, GATE__NAME);
        createEAttribute(gateEClass, GATE__OUTPUT);
        createEAttribute(gateEClass, GATE__RIGHT_COMMENT);
        createEAttribute(gateEClass, GATE__TRAILING_COMMENT);
        createEAttribute(gateEClass, GATE__VECTOR);

        gatesEClass = createEClass(GATES);
        createEReference(gatesEClass, GATES__GATE);
        createEAttribute(gatesEClass, GATES__BANNER_COMMENT);
        createEAttribute(gatesEClass, GATES__RIGHT_COMMENT);
        createEAttribute(gatesEClass, GATES__TRAILING_COMMENT);

        gatesizeEClass = createEClass(GATESIZE);
        createEAttribute(gatesizeEClass, GATESIZE__BANNER_COMMENT);
        createEAttribute(gatesizeEClass, GATESIZE__NAME);
        createEAttribute(gatesizeEClass, GATESIZE__RIGHT_COMMENT);
        createEAttribute(gatesizeEClass, GATESIZE__TRAILING_COMMENT);
        createEAttribute(gatesizeEClass, GATESIZE__VECTOR_SIZE);

        gatesizesEClass = createEClass(GATESIZES);
        createEReference(gatesizesEClass, GATESIZES__GATESIZE);
        createEAttribute(gatesizesEClass, GATESIZES__BANNER_COMMENT);
        createEAttribute(gatesizesEClass, GATESIZES__CONDITION);
        createEAttribute(gatesizesEClass, GATESIZES__RIGHT_COMMENT);
        createEAttribute(gatesizesEClass, GATESIZES__TRAILING_COMMENT);

        importEClass = createEClass(IMPORT);
        createEAttribute(importEClass, IMPORT__BANNER_COMMENT);
        createEAttribute(importEClass, IMPORT__FILENAME);
        createEAttribute(importEClass, IMPORT__RIGHT_COMMENT);
        createEAttribute(importEClass, IMPORT__TRAILING_COMMENT);

        nedFileEClass = createEClass(NED_FILE);
        createEAttribute(nedFileEClass, NED_FILE__GROUP);
        createEReference(nedFileEClass, NED_FILE__IMPORT);
        createEReference(nedFileEClass, NED_FILE__CHANNEL);
        createEReference(nedFileEClass, NED_FILE__SIMPLE_MODULE);
        createEReference(nedFileEClass, NED_FILE__COMPOUND_MODULE);
        createEReference(nedFileEClass, NED_FILE__NETWORK);
        createEAttribute(nedFileEClass, NED_FILE__BANNER_COMMENT);
        createEAttribute(nedFileEClass, NED_FILE__RIGHT_COMMENT);
        createEAttribute(nedFileEClass, NED_FILE__TRAILING_COMMENT);

        networkEClass = createEClass(NETWORK);
        createEReference(networkEClass, NETWORK__SUBSTPARAMS);
        createEAttribute(networkEClass, NETWORK__BANNER_COMMENT);
        createEAttribute(networkEClass, NETWORK__NAME);
        createEAttribute(networkEClass, NETWORK__RIGHT_COMMENT);
        createEAttribute(networkEClass, NETWORK__TRAILING_COMMENT);
        createEAttribute(networkEClass, NETWORK__TYPE_NAME);

        paramEClass = createEClass(PARAM);
        createEAttribute(paramEClass, PARAM__BANNER_COMMENT);
        createEAttribute(paramEClass, PARAM__DATA_TYPE);
        createEAttribute(paramEClass, PARAM__NAME);
        createEAttribute(paramEClass, PARAM__RIGHT_COMMENT);
        createEAttribute(paramEClass, PARAM__TRAILING_COMMENT);

        paramsEClass = createEClass(PARAMS);
        createEReference(paramsEClass, PARAMS__PARAM);
        createEAttribute(paramsEClass, PARAMS__BANNER_COMMENT);
        createEAttribute(paramsEClass, PARAMS__RIGHT_COMMENT);
        createEAttribute(paramsEClass, PARAMS__TRAILING_COMMENT);

        simpleModuleEClass = createEClass(SIMPLE_MODULE);
        createEReference(simpleModuleEClass, SIMPLE_MODULE__PARAMS);
        createEReference(simpleModuleEClass, SIMPLE_MODULE__GATES);
        createEReference(simpleModuleEClass, SIMPLE_MODULE__DISPLAY_STRING);
        createEAttribute(simpleModuleEClass, SIMPLE_MODULE__BANNER_COMMENT);
        createEAttribute(simpleModuleEClass, SIMPLE_MODULE__NAME);
        createEAttribute(simpleModuleEClass, SIMPLE_MODULE__RIGHT_COMMENT);
        createEAttribute(simpleModuleEClass, SIMPLE_MODULE__TRAILING_COMMENT);

        submoduleEClass = createEClass(SUBMODULE);
        createEReference(submoduleEClass, SUBMODULE__SUBSTPARAMS);
        createEReference(submoduleEClass, SUBMODULE__GATESIZES);
        createEReference(submoduleEClass, SUBMODULE__DISPLAY_STRING);
        createEAttribute(submoduleEClass, SUBMODULE__BANNER_COMMENT);
        createEAttribute(submoduleEClass, SUBMODULE__LIKE_PARAM);
        createEAttribute(submoduleEClass, SUBMODULE__NAME);
        createEAttribute(submoduleEClass, SUBMODULE__RIGHT_COMMENT);
        createEAttribute(submoduleEClass, SUBMODULE__TRAILING_COMMENT);
        createEAttribute(submoduleEClass, SUBMODULE__TYPE_NAME);
        createEAttribute(submoduleEClass, SUBMODULE__VECTOR_SIZE);

        submodulesEClass = createEClass(SUBMODULES);
        createEReference(submodulesEClass, SUBMODULES__SUBMODULE);
        createEAttribute(submodulesEClass, SUBMODULES__BANNER_COMMENT);
        createEAttribute(submodulesEClass, SUBMODULES__RIGHT_COMMENT);
        createEAttribute(submodulesEClass, SUBMODULES__TRAILING_COMMENT);

        substparamEClass = createEClass(SUBSTPARAM);
        createEAttribute(substparamEClass, SUBSTPARAM__BANNER_COMMENT);
        createEAttribute(substparamEClass, SUBSTPARAM__NAME);
        createEAttribute(substparamEClass, SUBSTPARAM__RIGHT_COMMENT);
        createEAttribute(substparamEClass, SUBSTPARAM__TRAILING_COMMENT);
        createEAttribute(substparamEClass, SUBSTPARAM__VALUE);

        substparamsEClass = createEClass(SUBSTPARAMS);
        createEReference(substparamsEClass, SUBSTPARAMS__SUBSTPARAM);
        createEAttribute(substparamsEClass, SUBSTPARAMS__BANNER_COMMENT);
        createEAttribute(substparamsEClass, SUBSTPARAMS__CONDITION);
        createEAttribute(substparamsEClass, SUBSTPARAMS__RIGHT_COMMENT);
        createEAttribute(substparamsEClass, SUBSTPARAMS__TRAILING_COMMENT);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private boolean isInitialized = false;

    /**
     * Complete the initialization of the package and its meta-model.  This
     * method is guarded to have no affect on any invocation but its first.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void initializePackageContents() {
        if (isInitialized) return;
        isInitialized = true;

        // Initialize package
        setName(eNAME);
        setNsPrefix(eNS_PREFIX);
        setNsURI(eNS_URI);

        // Obtain other dependent packages
        XMLTypePackageImpl theXMLTypePackage = (XMLTypePackageImpl)EPackage.Registry.INSTANCE.getEPackage(XMLTypePackage.eNS_URI);

        // Add supertypes to classes

        // Initialize classes and features; add operations and parameters
        initEClass(channelEClass, Channel.class, "Channel", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getChannel_ChannelAttr(), this.getChannelAttr(), null, "channelAttr", null, 0, -1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getChannel_DisplayString(), this.getDisplayString(), null, "displayString", null, 0, 1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannel_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannel_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannel_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannel_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Channel.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(channelAttrEClass, ChannelAttr.class, "ChannelAttr", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getChannelAttr_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, ChannelAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannelAttr_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, ChannelAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannelAttr_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, ChannelAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannelAttr_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, ChannelAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getChannelAttr_Value(), theXMLTypePackage.getString(), "value", null, 1, 1, ChannelAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(compoundModuleEClass, CompoundModule.class, "CompoundModule", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getCompoundModule_Params(), this.getParams(), null, "params", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getCompoundModule_Gates(), this.getGates(), null, "gates", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getCompoundModule_Submodules(), this.getSubmodules(), null, "submodules", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getCompoundModule_Connections(), this.getConnections(), null, "connections", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getCompoundModule_DisplayString(), this.getDisplayString(), null, "displayString", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getCompoundModule_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getCompoundModule_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getCompoundModule_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getCompoundModule_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, CompoundModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(connAttrEClass, ConnAttr.class, "ConnAttr", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getConnAttr_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, ConnAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnAttr_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, ConnAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnAttr_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, ConnAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnAttr_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, ConnAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnAttr_Value(), theXMLTypePackage.getString(), "value", null, 1, 1, ConnAttr.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(connectionEClass, Connection.class, "Connection", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getConnection_ConnAttr(), this.getConnAttr(), null, "connAttr", null, 0, -1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getConnection_DisplayString(), this.getDisplayString(), null, "displayString", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_Condition(), theXMLTypePackage.getString(), "condition", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_DestGate(), theXMLTypePackage.getNMTOKEN(), "destGate", null, 1, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_DestGateIndex(), theXMLTypePackage.getString(), "destGateIndex", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_DestGatePlusplus(), theXMLTypePackage.getBoolean(), "destGatePlusplus", "false", 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_DestModule(), theXMLTypePackage.getNMTOKEN(), "destModule", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_DestModuleIndex(), theXMLTypePackage.getString(), "destModuleIndex", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_RightToLeft(), theXMLTypePackage.getBoolean(), "rightToLeft", "false", 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_SrcGate(), theXMLTypePackage.getNMTOKEN(), "srcGate", null, 1, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_SrcGateIndex(), theXMLTypePackage.getString(), "srcGateIndex", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_SrcGatePlusplus(), theXMLTypePackage.getBoolean(), "srcGatePlusplus", "false", 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_SrcModule(), theXMLTypePackage.getNMTOKEN(), "srcModule", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_SrcModuleIndex(), theXMLTypePackage.getString(), "srcModuleIndex", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnection_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Connection.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(connectionsEClass, Connections.class, "Connections", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getConnections_Group(), ecorePackage.getEFeatureMapEntry(), "group", null, 0, -1, Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getConnections_Connection(), this.getConnection(), null, "connection", null, 0, -1, Connections.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEReference(getConnections_ForLoop(), this.getForLoop(), null, "forLoop", null, 0, -1, Connections.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnections_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnections_CheckUnconnected(), theXMLTypePackage.getBoolean(), "checkUnconnected", "true", 0, 1, Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnections_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getConnections_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Connections.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(displayStringEClass, DisplayString.class, "DisplayString", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getDisplayString_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, DisplayString.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getDisplayString_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, DisplayString.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getDisplayString_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, DisplayString.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getDisplayString_Value(), theXMLTypePackage.getString(), "value", null, 0, 1, DisplayString.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(forLoopEClass, ForLoop.class, "ForLoop", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getForLoop_ForLoop(), this.getForLoop(), null, "forLoop", null, 0, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getForLoop_Connection(), this.getConnection(), null, "connection", null, 0, -1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getForLoop_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getForLoop_FromValue(), theXMLTypePackage.getString(), "fromValue", null, 1, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getForLoop_ParamName(), theXMLTypePackage.getNMTOKEN(), "paramName", null, 1, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getForLoop_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getForLoop_ToValue(), theXMLTypePackage.getString(), "toValue", null, 1, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getForLoop_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, ForLoop.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(gateEClass, Gate.class, "Gate", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getGate_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGate_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGate_Output(), theXMLTypePackage.getBoolean(), "output", null, 1, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGate_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGate_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGate_Vector(), theXMLTypePackage.getBoolean(), "vector", "false", 0, 1, Gate.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(gatesEClass, Gates.class, "Gates", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getGates_Gate(), this.getGate(), null, "gate", null, 0, -1, Gates.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGates_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Gates.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGates_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Gates.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGates_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Gates.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(gatesizeEClass, Gatesize.class, "Gatesize", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getGatesize_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Gatesize.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesize_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Gatesize.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesize_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Gatesize.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesize_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Gatesize.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesize_VectorSize(), theXMLTypePackage.getString(), "vectorSize", null, 1, 1, Gatesize.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(gatesizesEClass, Gatesizes.class, "Gatesizes", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getGatesizes_Gatesize(), this.getGatesize(), null, "gatesize", null, 0, -1, Gatesizes.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesizes_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Gatesizes.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesizes_Condition(), theXMLTypePackage.getString(), "condition", null, 0, 1, Gatesizes.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesizes_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Gatesizes.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getGatesizes_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Gatesizes.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(importEClass, Import.class, "Import", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getImport_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Import.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getImport_Filename(), theXMLTypePackage.getString(), "filename", null, 1, 1, Import.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getImport_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Import.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getImport_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Import.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(nedFileEClass, NedFile.class, "NedFile", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getNedFile_Group(), ecorePackage.getEFeatureMapEntry(), "group", null, 0, -1, NedFile.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getNedFile_Import(), this.getImport(), null, "import", null, 0, -1, NedFile.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEReference(getNedFile_Channel(), this.getChannel(), null, "channel", null, 0, -1, NedFile.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEReference(getNedFile_SimpleModule(), this.getSimpleModule(), null, "simpleModule", null, 0, -1, NedFile.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEReference(getNedFile_CompoundModule(), this.getCompoundModule(), null, "compoundModule", null, 0, -1, NedFile.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEReference(getNedFile_Network(), this.getNetwork(), null, "network", null, 0, -1, NedFile.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
        initEAttribute(getNedFile_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, NedFile.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNedFile_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, NedFile.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNedFile_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, NedFile.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(networkEClass, Network.class, "Network", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getNetwork_Substparams(), this.getSubstparams(), null, "substparams", null, 0, 1, Network.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNetwork_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Network.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNetwork_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Network.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNetwork_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Network.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNetwork_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Network.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getNetwork_TypeName(), theXMLTypePackage.getNMTOKEN(), "typeName", null, 1, 1, Network.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(paramEClass, Param.class, "Param", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getParam_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParam_DataType(), theXMLTypePackage.getString(), "dataType", "numeric", 0, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParam_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParam_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParam_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Param.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(paramsEClass, Params.class, "Params", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getParams_Param(), this.getParam(), null, "param", null, 0, -1, Params.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParams_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Params.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParams_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Params.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getParams_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Params.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(simpleModuleEClass, SimpleModule.class, "SimpleModule", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getSimpleModule_Params(), this.getParams(), null, "params", null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getSimpleModule_Gates(), this.getGates(), null, "gates", null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getSimpleModule_DisplayString(), this.getDisplayString(), null, "displayString", null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSimpleModule_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSimpleModule_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSimpleModule_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSimpleModule_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, SimpleModule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(submoduleEClass, Submodule.class, "Submodule", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getSubmodule_Substparams(), this.getSubstparams(), null, "substparams", null, 0, -1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getSubmodule_Gatesizes(), this.getGatesizes(), null, "gatesizes", null, 0, -1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEReference(getSubmodule_DisplayString(), this.getDisplayString(), null, "displayString", null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_LikeParam(), theXMLTypePackage.getNMTOKEN(), "likeParam", null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_TypeName(), theXMLTypePackage.getNMTOKEN(), "typeName", null, 1, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodule_VectorSize(), theXMLTypePackage.getString(), "vectorSize", null, 0, 1, Submodule.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(submodulesEClass, Submodules.class, "Submodules", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getSubmodules_Submodule(), this.getSubmodule(), null, "submodule", null, 0, -1, Submodules.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodules_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Submodules.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodules_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Submodules.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubmodules_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Submodules.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(substparamEClass, Substparam.class, "Substparam", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEAttribute(getSubstparam_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Substparam.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparam_Name(), theXMLTypePackage.getNMTOKEN(), "name", null, 1, 1, Substparam.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparam_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Substparam.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparam_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Substparam.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparam_Value(), theXMLTypePackage.getString(), "value", null, 1, 1, Substparam.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        initEClass(substparamsEClass, Substparams.class, "Substparams", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
        initEReference(getSubstparams_Substparam(), this.getSubstparam(), null, "substparam", null, 0, -1, Substparams.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparams_BannerComment(), theXMLTypePackage.getString(), "bannerComment", null, 0, 1, Substparams.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparams_Condition(), theXMLTypePackage.getString(), "condition", null, 0, 1, Substparams.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparams_RightComment(), theXMLTypePackage.getString(), "rightComment", null, 0, 1, Substparams.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
        initEAttribute(getSubstparams_TrailingComment(), theXMLTypePackage.getString(), "trailingComment", null, 0, 1, Substparams.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, !IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

        // Create resource
        createResource(eNS_URI);

        // Create annotations
        // http:///org/eclipse/emf/ecore/util/ExtendedMetaData
        createExtendedMetaDataAnnotations();
    }

    /**
     * Initializes the annotations for <b>http:///org/eclipse/emf/ecore/util/ExtendedMetaData</b>.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected void createExtendedMetaDataAnnotations() {
        String source = "http:///org/eclipse/emf/ecore/util/ExtendedMetaData";		
        addAnnotation
          (channelEClass, 
           source, 
           new String[] {
             "name", "channel",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getChannel_ChannelAttr(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "channel-attr",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getChannel_DisplayString(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "display-string",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getChannel_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getChannel_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getChannel_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getChannel_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (channelAttrEClass, 
           source, 
           new String[] {
             "name", "channel-attr",
             "kind", "empty"
           });		
        addAnnotation
          (getChannelAttr_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getChannelAttr_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getChannelAttr_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getChannelAttr_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getChannelAttr_Value(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "value"
           });		
        addAnnotation
          (compoundModuleEClass, 
           source, 
           new String[] {
             "name", "compound-module",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getCompoundModule_Params(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "params",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getCompoundModule_Gates(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "gates",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getCompoundModule_Submodules(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "submodules",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getCompoundModule_Connections(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "connections",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getCompoundModule_DisplayString(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "display-string",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getCompoundModule_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getCompoundModule_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getCompoundModule_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getCompoundModule_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (connAttrEClass, 
           source, 
           new String[] {
             "name", "conn-attr",
             "kind", "empty"
           });		
        addAnnotation
          (getConnAttr_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getConnAttr_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getConnAttr_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getConnAttr_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getConnAttr_Value(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "value"
           });		
        addAnnotation
          (connectionEClass, 
           source, 
           new String[] {
             "name", "connection",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getConnection_ConnAttr(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "conn-attr",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getConnection_DisplayString(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "display-string",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getConnection_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getConnection_Condition(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "condition"
           });		
        addAnnotation
          (getConnection_DestGate(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "dest-gate"
           });		
        addAnnotation
          (getConnection_DestGateIndex(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "dest-gate-index"
           });		
        addAnnotation
          (getConnection_DestGatePlusplus(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "dest-gate-plusplus"
           });		
        addAnnotation
          (getConnection_DestModule(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "dest-module"
           });		
        addAnnotation
          (getConnection_DestModuleIndex(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "dest-module-index"
           });		
        addAnnotation
          (getConnection_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getConnection_RightToLeft(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-to-left"
           });		
        addAnnotation
          (getConnection_SrcGate(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "src-gate"
           });		
        addAnnotation
          (getConnection_SrcGateIndex(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "src-gate-index"
           });		
        addAnnotation
          (getConnection_SrcGatePlusplus(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "src-gate-plusplus"
           });		
        addAnnotation
          (getConnection_SrcModule(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "src-module"
           });		
        addAnnotation
          (getConnection_SrcModuleIndex(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "src-module-index"
           });		
        addAnnotation
          (getConnection_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (connectionsEClass, 
           source, 
           new String[] {
             "name", "connections",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getConnections_Group(), 
           source, 
           new String[] {
             "kind", "group",
             "name", "group:0"
           });		
        addAnnotation
          (getConnections_Connection(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "connection",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getConnections_ForLoop(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "for-loop",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getConnections_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getConnections_CheckUnconnected(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "check-unconnected"
           });		
        addAnnotation
          (getConnections_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getConnections_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (displayStringEClass, 
           source, 
           new String[] {
             "name", "display-string",
             "kind", "empty"
           });		
        addAnnotation
          (getDisplayString_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getDisplayString_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getDisplayString_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getDisplayString_Value(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "value"
           });		
        addAnnotation
          (forLoopEClass, 
           source, 
           new String[] {
             "name", "for-loop",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getForLoop_ForLoop(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "for-loop",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getForLoop_Connection(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "connection",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getForLoop_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getForLoop_FromValue(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "from-value"
           });		
        addAnnotation
          (getForLoop_ParamName(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "param-name"
           });		
        addAnnotation
          (getForLoop_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getForLoop_ToValue(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "to-value"
           });		
        addAnnotation
          (getForLoop_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (gateEClass, 
           source, 
           new String[] {
             "name", "gate",
             "kind", "empty"
           });		
        addAnnotation
          (getGate_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getGate_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getGate_Output(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "output"
           });		
        addAnnotation
          (getGate_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getGate_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getGate_Vector(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "vector"
           });		
        addAnnotation
          (gatesEClass, 
           source, 
           new String[] {
             "name", "gates",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getGates_Gate(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "gate",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getGates_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getGates_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getGates_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (gatesizeEClass, 
           source, 
           new String[] {
             "name", "gatesize",
             "kind", "empty"
           });		
        addAnnotation
          (getGatesize_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getGatesize_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getGatesize_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getGatesize_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getGatesize_VectorSize(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "vector-size"
           });		
        addAnnotation
          (gatesizesEClass, 
           source, 
           new String[] {
             "name", "gatesizes",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getGatesizes_Gatesize(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "gatesize",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getGatesizes_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getGatesizes_Condition(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "condition"
           });		
        addAnnotation
          (getGatesizes_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getGatesizes_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (importEClass, 
           source, 
           new String[] {
             "name", "import",
             "kind", "empty"
           });		
        addAnnotation
          (getImport_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getImport_Filename(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "filename"
           });		
        addAnnotation
          (getImport_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getImport_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });			
        addAnnotation
          (nedFileEClass, 
           source, 
           new String[] {
             "name", "ned-file",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getNedFile_Group(), 
           source, 
           new String[] {
             "kind", "group",
             "name", "group:0"
           });		
        addAnnotation
          (getNedFile_Import(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "import",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getNedFile_Channel(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "channel",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getNedFile_SimpleModule(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "simple-module",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getNedFile_CompoundModule(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "compound-module",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getNedFile_Network(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "network",
             "namespace", "##targetNamespace",
             "group", "#group:0"
           });		
        addAnnotation
          (getNedFile_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getNedFile_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getNedFile_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (networkEClass, 
           source, 
           new String[] {
             "name", "network",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getNetwork_Substparams(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "substparams",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getNetwork_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getNetwork_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getNetwork_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getNetwork_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getNetwork_TypeName(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "type-name"
           });		
        addAnnotation
          (paramEClass, 
           source, 
           new String[] {
             "name", "param",
             "kind", "empty"
           });		
        addAnnotation
          (getParam_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getParam_DataType(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "data-type"
           });		
        addAnnotation
          (getParam_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getParam_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getParam_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (paramsEClass, 
           source, 
           new String[] {
             "name", "params",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getParams_Param(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "param",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getParams_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getParams_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getParams_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (simpleModuleEClass, 
           source, 
           new String[] {
             "name", "simple-module",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getSimpleModule_Params(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "params",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSimpleModule_Gates(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "gates",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSimpleModule_DisplayString(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "display-string",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSimpleModule_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getSimpleModule_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getSimpleModule_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getSimpleModule_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (submoduleEClass, 
           source, 
           new String[] {
             "name", "submodule",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getSubmodule_Substparams(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "substparams",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSubmodule_Gatesizes(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "gatesizes",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSubmodule_DisplayString(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "display-string",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSubmodule_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getSubmodule_LikeParam(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "like-param"
           });		
        addAnnotation
          (getSubmodule_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getSubmodule_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getSubmodule_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getSubmodule_TypeName(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "type-name"
           });		
        addAnnotation
          (getSubmodule_VectorSize(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "vector-size"
           });		
        addAnnotation
          (submodulesEClass, 
           source, 
           new String[] {
             "name", "submodules",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getSubmodules_Submodule(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "submodule",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSubmodules_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getSubmodules_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getSubmodules_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (substparamEClass, 
           source, 
           new String[] {
             "name", "substparam",
             "kind", "empty"
           });		
        addAnnotation
          (getSubstparam_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getSubstparam_Name(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "name"
           });		
        addAnnotation
          (getSubstparam_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getSubstparam_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });		
        addAnnotation
          (getSubstparam_Value(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "value"
           });		
        addAnnotation
          (substparamsEClass, 
           source, 
           new String[] {
             "name", "substparams",
             "kind", "elementOnly"
           });		
        addAnnotation
          (getSubstparams_Substparam(), 
           source, 
           new String[] {
             "kind", "element",
             "name", "substparam",
             "namespace", "##targetNamespace"
           });		
        addAnnotation
          (getSubstparams_BannerComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "banner-comment"
           });		
        addAnnotation
          (getSubstparams_Condition(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "condition"
           });		
        addAnnotation
          (getSubstparams_RightComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "right-comment"
           });		
        addAnnotation
          (getSubstparams_TrailingComment(), 
           source, 
           new String[] {
             "kind", "attribute",
             "name", "trailing-comment"
           });
    }

} //NedPackageImpl
