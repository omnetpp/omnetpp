package com.simulcraft.test.gui.core;

import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.reflect.CodeSignature;

/**
 * Based on GetInfo.aj, TJP Example project, AJDT. 
 */
public aspect Tracing {
	   static final void println(String s){ System.out.println(s); }

//	   Object around(): execution(public * org.omnetpp.test.gui.access.*.*(..)) {
//	      println("Intercepted message: " + thisJoinPointStaticPart.getSignature().getName());
//	      println("in class: " + thisJoinPointStaticPart.getSignature().getDeclaringType().getName());
//	      printParameters(thisJoinPoint);
//	      println("Running original method: \n" );
//	      Object result = proceed();
//	      println("  result: " + result );
//	      return result;
//	   }

	   static protected void printParameters(JoinPoint jp) {
	      println("Arguments: " );
	      Object[] args = jp.getArgs();
	      String[] names = ((CodeSignature)jp.getSignature()).getParameterNames();
	      Class[] types = ((CodeSignature)jp.getSignature()).getParameterTypes();
	      for (int i = 0; i < args.length; i++) {
	         println("  "  + i + ". " + names[i] + " : " + types[i].getName() + " = " + args[i]);
	      }
	   }

}
