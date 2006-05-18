/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SetOperation;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Set Operation</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getFilenamePattern <em>Filename Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getRunNamePattern <em>Run Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getExperimentNamePattern <em>Experiment Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getMeasurementNamePattern <em>Measurement Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getReplicationNamePattern <em>Replication Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getModuleNamePattern <em>Module Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getNamePattern <em>Name Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getSourceDataset <em>Source Dataset</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getFromRunsWhere <em>From Runs Where</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public abstract class SetOperationImpl extends EObjectImpl implements SetOperation {
	/**
	 * The default value of the '{@link #getFilenamePattern() <em>Filename Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFilenamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String FILENAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getFilenamePattern() <em>Filename Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFilenamePattern()
	 * @generated
	 * @ordered
	 */
	protected String filenamePattern = FILENAME_PATTERN_EDEFAULT;

	/**
	 * The default value of the '{@link #getRunNamePattern() <em>Run Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getRunNamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String RUN_NAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getRunNamePattern() <em>Run Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getRunNamePattern()
	 * @generated
	 * @ordered
	 */
	protected String runNamePattern = RUN_NAME_PATTERN_EDEFAULT;

	/**
	 * The default value of the '{@link #getExperimentNamePattern() <em>Experiment Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExperimentNamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String EXPERIMENT_NAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getExperimentNamePattern() <em>Experiment Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExperimentNamePattern()
	 * @generated
	 * @ordered
	 */
	protected String experimentNamePattern = EXPERIMENT_NAME_PATTERN_EDEFAULT;

	/**
	 * The default value of the '{@link #getMeasurementNamePattern() <em>Measurement Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMeasurementNamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String MEASUREMENT_NAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getMeasurementNamePattern() <em>Measurement Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMeasurementNamePattern()
	 * @generated
	 * @ordered
	 */
	protected String measurementNamePattern = MEASUREMENT_NAME_PATTERN_EDEFAULT;

	/**
	 * The default value of the '{@link #getReplicationNamePattern() <em>Replication Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getReplicationNamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String REPLICATION_NAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getReplicationNamePattern() <em>Replication Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getReplicationNamePattern()
	 * @generated
	 * @ordered
	 */
	protected String replicationNamePattern = REPLICATION_NAME_PATTERN_EDEFAULT;

	/**
	 * The default value of the '{@link #getModuleNamePattern() <em>Module Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getModuleNamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String MODULE_NAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getModuleNamePattern() <em>Module Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getModuleNamePattern()
	 * @generated
	 * @ordered
	 */
	protected String moduleNamePattern = MODULE_NAME_PATTERN_EDEFAULT;

	/**
	 * The default value of the '{@link #getNamePattern() <em>Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getNamePattern()
	 * @generated
	 * @ordered
	 */
	protected static final String NAME_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getNamePattern() <em>Name Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getNamePattern()
	 * @generated
	 * @ordered
	 */
	protected String namePattern = NAME_PATTERN_EDEFAULT;

	/**
	 * The cached value of the '{@link #getSourceDataset() <em>Source Dataset</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSourceDataset()
	 * @generated
	 * @ordered
	 */
	protected Dataset sourceDataset = null;

	/**
	 * The default value of the '{@link #getFromRunsWhere() <em>From Runs Where</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFromRunsWhere()
	 * @generated
	 * @ordered
	 */
	protected static final String FROM_RUNS_WHERE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getFromRunsWhere() <em>From Runs Where</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFromRunsWhere()
	 * @generated
	 * @ordered
	 */
	protected String fromRunsWhere = FROM_RUNS_WHERE_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected SetOperationImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return ScaveModelPackage.Literals.SET_OPERATION;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getFilenamePattern() {
		return filenamePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setFilenamePattern(String newFilenamePattern) {
		String oldFilenamePattern = filenamePattern;
		filenamePattern = newFilenamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN, oldFilenamePattern, filenamePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getRunNamePattern() {
		return runNamePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setRunNamePattern(String newRunNamePattern) {
		String oldRunNamePattern = runNamePattern;
		runNamePattern = newRunNamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN, oldRunNamePattern, runNamePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getExperimentNamePattern() {
		return experimentNamePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setExperimentNamePattern(String newExperimentNamePattern) {
		String oldExperimentNamePattern = experimentNamePattern;
		experimentNamePattern = newExperimentNamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN, oldExperimentNamePattern, experimentNamePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getMeasurementNamePattern() {
		return measurementNamePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setMeasurementNamePattern(String newMeasurementNamePattern) {
		String oldMeasurementNamePattern = measurementNamePattern;
		measurementNamePattern = newMeasurementNamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN, oldMeasurementNamePattern, measurementNamePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getReplicationNamePattern() {
		return replicationNamePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setReplicationNamePattern(String newReplicationNamePattern) {
		String oldReplicationNamePattern = replicationNamePattern;
		replicationNamePattern = newReplicationNamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN, oldReplicationNamePattern, replicationNamePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getModuleNamePattern() {
		return moduleNamePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setModuleNamePattern(String newModuleNamePattern) {
		String oldModuleNamePattern = moduleNamePattern;
		moduleNamePattern = newModuleNamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN, oldModuleNamePattern, moduleNamePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getNamePattern() {
		return namePattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setNamePattern(String newNamePattern) {
		String oldNamePattern = namePattern;
		namePattern = newNamePattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__NAME_PATTERN, oldNamePattern, namePattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Dataset getSourceDataset() {
		if (sourceDataset != null && sourceDataset.eIsProxy()) {
			InternalEObject oldSourceDataset = (InternalEObject)sourceDataset;
			sourceDataset = (Dataset)eResolveProxy(oldSourceDataset);
			if (sourceDataset != oldSourceDataset) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ScaveModelPackage.SET_OPERATION__SOURCE_DATASET, oldSourceDataset, sourceDataset));
			}
		}
		return sourceDataset;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Dataset basicGetSourceDataset() {
		return sourceDataset;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSourceDataset(Dataset newSourceDataset) {
		Dataset oldSourceDataset = sourceDataset;
		sourceDataset = newSourceDataset;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__SOURCE_DATASET, oldSourceDataset, sourceDataset));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getFromRunsWhere() {
		return fromRunsWhere;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setFromRunsWhere(String newFromRunsWhere) {
		String oldFromRunsWhere = fromRunsWhere;
		fromRunsWhere = newFromRunsWhere;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE, oldFromRunsWhere, fromRunsWhere));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN:
				return getFilenamePattern();
			case ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN:
				return getRunNamePattern();
			case ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN:
				return getExperimentNamePattern();
			case ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN:
				return getMeasurementNamePattern();
			case ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN:
				return getReplicationNamePattern();
			case ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN:
				return getModuleNamePattern();
			case ScaveModelPackage.SET_OPERATION__NAME_PATTERN:
				return getNamePattern();
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				if (resolve) return getSourceDataset();
				return basicGetSourceDataset();
			case ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE:
				return getFromRunsWhere();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN:
				setFilenamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN:
				setRunNamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN:
				setExperimentNamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN:
				setMeasurementNamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN:
				setReplicationNamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN:
				setModuleNamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__NAME_PATTERN:
				setNamePattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				setSourceDataset((Dataset)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE:
				setFromRunsWhere((String)newValue);
				return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eUnset(int featureID) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN:
				setFilenamePattern(FILENAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN:
				setRunNamePattern(RUN_NAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN:
				setExperimentNamePattern(EXPERIMENT_NAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN:
				setMeasurementNamePattern(MEASUREMENT_NAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN:
				setReplicationNamePattern(REPLICATION_NAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN:
				setModuleNamePattern(MODULE_NAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__NAME_PATTERN:
				setNamePattern(NAME_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				setSourceDataset((Dataset)null);
				return;
			case ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE:
				setFromRunsWhere(FROM_RUNS_WHERE_EDEFAULT);
				return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean eIsSet(int featureID) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__FILENAME_PATTERN:
				return FILENAME_PATTERN_EDEFAULT == null ? filenamePattern != null : !FILENAME_PATTERN_EDEFAULT.equals(filenamePattern);
			case ScaveModelPackage.SET_OPERATION__RUN_NAME_PATTERN:
				return RUN_NAME_PATTERN_EDEFAULT == null ? runNamePattern != null : !RUN_NAME_PATTERN_EDEFAULT.equals(runNamePattern);
			case ScaveModelPackage.SET_OPERATION__EXPERIMENT_NAME_PATTERN:
				return EXPERIMENT_NAME_PATTERN_EDEFAULT == null ? experimentNamePattern != null : !EXPERIMENT_NAME_PATTERN_EDEFAULT.equals(experimentNamePattern);
			case ScaveModelPackage.SET_OPERATION__MEASUREMENT_NAME_PATTERN:
				return MEASUREMENT_NAME_PATTERN_EDEFAULT == null ? measurementNamePattern != null : !MEASUREMENT_NAME_PATTERN_EDEFAULT.equals(measurementNamePattern);
			case ScaveModelPackage.SET_OPERATION__REPLICATION_NAME_PATTERN:
				return REPLICATION_NAME_PATTERN_EDEFAULT == null ? replicationNamePattern != null : !REPLICATION_NAME_PATTERN_EDEFAULT.equals(replicationNamePattern);
			case ScaveModelPackage.SET_OPERATION__MODULE_NAME_PATTERN:
				return MODULE_NAME_PATTERN_EDEFAULT == null ? moduleNamePattern != null : !MODULE_NAME_PATTERN_EDEFAULT.equals(moduleNamePattern);
			case ScaveModelPackage.SET_OPERATION__NAME_PATTERN:
				return NAME_PATTERN_EDEFAULT == null ? namePattern != null : !NAME_PATTERN_EDEFAULT.equals(namePattern);
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				return sourceDataset != null;
			case ScaveModelPackage.SET_OPERATION__FROM_RUNS_WHERE:
				return FROM_RUNS_WHERE_EDEFAULT == null ? fromRunsWhere != null : !FROM_RUNS_WHERE_EDEFAULT.equals(fromRunsWhere);
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (filenamePattern: ");
		result.append(filenamePattern);
		result.append(", runNamePattern: ");
		result.append(runNamePattern);
		result.append(", experimentNamePattern: ");
		result.append(experimentNamePattern);
		result.append(", measurementNamePattern: ");
		result.append(measurementNamePattern);
		result.append(", replicationNamePattern: ");
		result.append(replicationNamePattern);
		result.append(", moduleNamePattern: ");
		result.append(moduleNamePattern);
		result.append(", namePattern: ");
		result.append(namePattern);
		result.append(", fromRunsWhere: ");
		result.append(fromRunsWhere);
		result.append(')');
		return result.toString();
	}

} //SetOperationImpl