/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.tests;

import junit.textui.TestRunner;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * <!-- begin-user-doc -->
 * A test case for the model object '<em><b>Chart</b></em>'.
 * <!-- end-user-doc -->
 * @generated
 */
public class ChartTest extends DatasetItemTest {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static void main(String[] args) {
		TestRunner.run(ChartTest.class);
	}

	/**
	 * Constructs a new Chart test case with the given name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ChartTest(String name) {
		super(name);
	}

	/**
	 * Returns the fixture for this Chart test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	private Chart getFixture() {
		return (Chart)fixture;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#setUp()
	 * @generated
	 */
	protected void setUp() throws Exception {
		setFixture(ScaveModelFactory.eINSTANCE.createChart());
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#tearDown()
	 * @generated
	 */
	protected void tearDown() throws Exception {
		setFixture(null);
	}

} //ChartTest
