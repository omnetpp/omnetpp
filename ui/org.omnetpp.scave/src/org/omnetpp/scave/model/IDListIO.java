package org.omnetpp.scave.model;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringReader;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.eclipse.core.runtime.IProgressMonitor;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;

import org.omnetpp.scave.engine.IDList;

public class IDListIO {
	
	static String xslt =
        "<xsl:stylesheet version='1.0' " +
        "  xmlns:xsl='http://www.w3.org/1999/XSL/Transform' " +
        "  xmlns:xalan='http://xml.apache.org/xslt' " +
        "  exclude-result-prefixes='xalan'>" +
        "  <xsl:output method='xml' indent='yes' xalan:indent-amount='4'/>" +
        "  <xsl:template match='@*|node()'>" +
        "    <xsl:copy>" +
        "      <xsl:apply-templates select='@*|node()'/>" +
        "    </xsl:copy>" +
        "  </xsl:template>" +
        "</xsl:stylesheet>";
	

	public static void save(IDList idlist, String fileName, IProgressMonitor progressMonitor) {
		try {
			// Use the parser as a SAX source for input
			XMLReader saxReader = new IDListSAXEventGenerator(idlist, progressMonitor);
			SAXSource source = new SAXSource(saxReader, null);
			FileOutputStream out = new FileOutputStream(fileName);
			StreamResult result = new StreamResult(out);

			long t0 = System.currentTimeMillis();
			TransformerFactory transformerFactory = TransformerFactory.newInstance();
			//transformerFactory.setAttribute("indent-number", 2);
			//transformer.setOutputProperty(OutputKeys.METHOD, "xml");
			//transformer.setOutputProperty(OutputKeys.INDENT, "yes");
			//transformer.setOutputProperty("{http://xml.apache.org/xalan}indent-amount", "2");
			//Transformer transformer = transformerFactory.newTransformer(new StreamSource(new StringReader(xslt)));			
			Transformer transformer = transformerFactory.newTransformer();
			transformer.transform(source, result);
			System.out.println("saving "+fileName+": " + (System.currentTimeMillis()-t0));

		} catch (TransformerConfigurationException e) {
			throw new RuntimeException("internal error: "+e.getMessage(), e);
		} catch (TransformerException e) {
			throw new RuntimeException("error writing XML: "+e.getMessage(), e);
		} catch (FileNotFoundException e) {
			throw new RuntimeException(e);
		}
	}

	public static IDList load(File file, IProgressMonitor progressMonitor) {
		try {
			// TODO use progressMonitor
			long t0 = System.currentTimeMillis();
			SAXParserFactory spfactory = SAXParserFactory.newInstance();
			SAXParser parser = spfactory.newSAXParser();
			IDListSAXHandler a = new IDListSAXHandler();
			parser.parse(file, a);
			System.out.println("loading "+file.getName()+": " + (System.currentTimeMillis()-t0));
			return a.getResult();

		} catch (ParserConfigurationException e) {
			throw new RuntimeException("internal error: "+e.getMessage(), e);
		} catch (SAXException e) {
			throw new RuntimeException("error parsing XML: "+e.getMessage(), e);
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
	}

}
