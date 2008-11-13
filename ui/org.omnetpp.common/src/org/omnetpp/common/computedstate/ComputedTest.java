package org.omnetpp.common.computedstate;

import org.omnetpp.common.Debug;

/*
import java.util.Map;

import org.omnetpp.ned2.model.INEDElement;

    private allPropertiesState ComputedState<HashMap<String, INEDElement>>;

    public Map<String, INEDElement> getAllProperties() {
	   return allPropertiesState.xxx(new IComputation() {
 		   public Object compute() {
			   HashMap copy = getParent().getAllProperties();
			   copy.add(self.getOwnProperties());
			   return copy;
		   }
    	});


	   return allPropertiesState.xxx(new IComputation() {
 		   public void compute() {
			   HashMap copy = getParent().getAllProperties();
			   copy.add(self.getOwnProperties());
			   allPropertiesState.setValue(copy);
		   }
    	});
*/
/*
     @Override
    protected void refreshVisuals() {
 	   return visualsState.xxx(new IComputation() {
 		   public Object compute() {
		        // define the properties that determine the visual appearence
		    	SubmoduleNodeEx submNode = (SubmoduleNodeEx)getModel();

		    	// set module name and vector size
		    	String nameToDisplay = submNode.getName();
		    	// add [size] if it's a module vector


				...
				return null;
 */

public class ComputedTest {
   private ComputedState<Integer> aState = new ComputedState<Integer>();

   // optionally initialize here
   private ComputedState<Integer> bState =
	   new ComputedState<Integer>(new IComputation() {
 		   public Object compute() {
 			   return getA() + 1;
 		   }
	   });

   private ComputedState<Integer> xState = new ComputedState<Integer>();

   private ComputedState<Integer> cState = new ComputedState<Integer>();

   private ComputedState<Integer> dState = new ComputedState<Integer>();

   public int getA() {
	   return aState.getValue();
   }

   public void setA(int a) {
	   aState.setValue(a);
   }

   public int getB() {
	   // return bState.recomputeIfInvalid();

	   return bState.recomputeIfInvalidAs(new IComputation() {
 		   public Object compute() {
 			   Debug.println("Recomputing");
 			   return getA() + 1;
 		   }
	   });
   }

   public int getX() {
	   return xState.recomputeIfInvalidAs(new IComputation() {
 		   public Object compute() {
 			   Debug.println("Recomputing");
 			   return getA() + getB();
 		   }
	   });
   }

   public int getC() {
	   return cState.recomputeIfInvalidAs(new IComputation() {
 		   public Object compute() {
 			   return getD() + 1;
 		   }
	   });
   }

   public int getD() {
	   return dState.recomputeIfInvalidAs(new IComputation() {
 		   public Object compute() {
 			   return getC() + 1;
 		   }
	   });
   }

   public static void main(String[] args) {
	   final ComputedTest test1 = new ComputedTest();

	   test1.setA(1);
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());

	   test1.setA(2);
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());

	   test1.setA(10);
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());
	   Debug.println("a: " + test1.getA() + " b: " + test1.getB() + " x: " + test1.getX());

	   Debug.println("c: " + test1.getC() + " d: " + test1.getD());
   }
}
