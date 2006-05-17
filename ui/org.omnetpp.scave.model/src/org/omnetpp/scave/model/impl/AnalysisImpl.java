/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Analysis</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.AnalysisImpl#getDatasets <em>Datasets</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.AnalysisImpl#getChartPages <em>Chart Pages</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class AnalysisImpl extends EObjectImpl implements Analysis {
	/**
	 * The cached value of the '{@link #getDatasets() <em>Datasets</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDatasets()
	 * @generated
	 * @ordered
	 */
	protected EList datasets = null;

	/**
	 * The cached value of the '{@link #getChartPages() <em>Chart Pages</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getChartPages()
	 * @generated
	 * @ordered
	 */
	protected EList chartPages = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected AnalysisImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return ScaveModelPackage.Literals.ANALYSIS;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList getDatasets() {
		if (datasets == null) {
			datasets = new EObjectContainmentEList(Dataset.class, this, ScaveModelPackage.ANALYSIS__DATASETS);
		}
		return datasets;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList getChartPages() {
		if (chartPages == null) {
			chartPages = new EObjectContainmentEList(ChartSheet.class, this, ScaveModelPackage.ANALYSIS__CHART_PAGES);
		}
		return chartPages;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case ScaveModelPackage.ANALYSIS__DATASETS:
				return ((InternalEList)getDatasets()).basicRemove(otherEnd, msgs);
			case ScaveModelPackage.ANALYSIS__CHART_PAGES:
				return ((InternalEList)getChartPages()).basicRemove(otherEnd, msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.ANALYSIS__DATASETS:
				return getDatasets();
			case ScaveModelPackage.ANALYSIS__CHART_PAGES:
				return getChartPages();
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
			case ScaveModelPackage.ANALYSIS__DATASETS:
				getDatasets().clear();
				getDatasets().addAll((Collection)newValue);
				return;
			case ScaveModelPackage.ANALYSIS__CHART_PAGES:
				getChartPages().clear();
				getChartPages().addAll((Collection)newValue);
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
			case ScaveModelPackage.ANALYSIS__DATASETS:
				getDatasets().clear();
				return;
			case ScaveModelPackage.ANALYSIS__CHART_PAGES:
				getChartPages().clear();
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
			case ScaveModelPackage.ANALYSIS__DATASETS:
				return datasets != null && !datasets.isEmpty();
			case ScaveModelPackage.ANALYSIS__CHART_PAGES:
				return chartPages != null && !chartPages.isEmpty();
		}
		return super.eIsSet(featureID);
	}

} //AnalysisImpl