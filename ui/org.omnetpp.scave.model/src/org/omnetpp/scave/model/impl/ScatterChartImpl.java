/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Scatter Chart</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ScatterChartImpl#getXDataModule <em>XData Module</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ScatterChartImpl#getXDataName <em>XData Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ScatterChartImpl#isAverageReplications <em>Average Replications</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ScatterChartImpl extends ChartImpl implements ScatterChart {
	/**
	 * The default value of the '{@link #getXDataModule() <em>XData Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXDataModule()
	 * @generated
	 * @ordered
	 */
	protected static final String XDATA_MODULE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getXDataModule() <em>XData Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXDataModule()
	 * @generated
	 * @ordered
	 */
	protected String xDataModule = XDATA_MODULE_EDEFAULT;

	/**
	 * The default value of the '{@link #getXDataName() <em>XData Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXDataName()
	 * @generated
	 * @ordered
	 */
	protected static final String XDATA_NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getXDataName() <em>XData Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXDataName()
	 * @generated
	 * @ordered
	 */
	protected String xDataName = XDATA_NAME_EDEFAULT;

	/**
	 * The default value of the '{@link #isAverageReplications() <em>Average Replications</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isAverageReplications()
	 * @generated
	 * @ordered
	 */
	protected static final boolean AVERAGE_REPLICATIONS_EDEFAULT = true;

	/**
	 * The cached value of the '{@link #isAverageReplications() <em>Average Replications</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isAverageReplications()
	 * @generated
	 * @ordered
	 */
	protected boolean averageReplications = AVERAGE_REPLICATIONS_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ScatterChartImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return ScaveModelPackage.Literals.SCATTER_CHART;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getXDataModule() {
		return xDataModule;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setXDataModule(String newXDataModule) {
		String oldXDataModule = xDataModule;
		xDataModule = newXDataModule;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SCATTER_CHART__XDATA_MODULE, oldXDataModule, xDataModule));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getXDataName() {
		return xDataName;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setXDataName(String newXDataName) {
		String oldXDataName = xDataName;
		xDataName = newXDataName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SCATTER_CHART__XDATA_NAME, oldXDataName, xDataName));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isAverageReplications() {
		return averageReplications;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setAverageReplications(boolean newAverageReplications) {
		boolean oldAverageReplications = averageReplications;
		averageReplications = newAverageReplications;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS, oldAverageReplications, averageReplications));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.SCATTER_CHART__XDATA_MODULE:
				return getXDataModule();
			case ScaveModelPackage.SCATTER_CHART__XDATA_NAME:
				return getXDataName();
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				return isAverageReplications() ? Boolean.TRUE : Boolean.FALSE;
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case ScaveModelPackage.SCATTER_CHART__XDATA_MODULE:
				setXDataModule((String)newValue);
				return;
			case ScaveModelPackage.SCATTER_CHART__XDATA_NAME:
				setXDataName((String)newValue);
				return;
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				setAverageReplications(((Boolean)newValue).booleanValue());
				return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eUnset(int featureID) {
		switch (featureID) {
			case ScaveModelPackage.SCATTER_CHART__XDATA_MODULE:
				setXDataModule(XDATA_MODULE_EDEFAULT);
				return;
			case ScaveModelPackage.SCATTER_CHART__XDATA_NAME:
				setXDataName(XDATA_NAME_EDEFAULT);
				return;
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				setAverageReplications(AVERAGE_REPLICATIONS_EDEFAULT);
				return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public boolean eIsSet(int featureID) {
		switch (featureID) {
			case ScaveModelPackage.SCATTER_CHART__XDATA_MODULE:
				return XDATA_MODULE_EDEFAULT == null ? xDataModule != null : !XDATA_MODULE_EDEFAULT.equals(xDataModule);
			case ScaveModelPackage.SCATTER_CHART__XDATA_NAME:
				return XDATA_NAME_EDEFAULT == null ? xDataName != null : !XDATA_NAME_EDEFAULT.equals(xDataName);
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				return averageReplications != AVERAGE_REPLICATIONS_EDEFAULT;
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (xDataModule: ");
		result.append(xDataModule);
		result.append(", xDataName: ");
		result.append(xDataName);
		result.append(", averageReplications: ");
		result.append(averageReplications);
		result.append(')');
		return result.toString();
	}

} //ScatterChartImpl