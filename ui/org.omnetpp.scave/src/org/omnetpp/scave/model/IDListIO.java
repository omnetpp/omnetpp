package org.omnetpp.scave.model;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamResult;

import org.eclipse.core.runtime.IProgressMonitor;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;

import org.omnetpp.scave.engine.IDList;

public class IDListIO {

	public static void save(IDList idlist, String fileName, IProgressMonitor progressMonitor) {
		try {
			// Use the parser as a SAX source for input
			XMLReader saxReader = new IDListSAXEventGenerator(idlist, progressMonitor);
			SAXSource source = new SAXSource(saxReader, null);
			FileOutputStream out = new FileOutputStream(fileName);
			StreamResult result = new StreamResult(out);

			long t0 = System.currentTimeMillis();
			TransformerFactory transformerFactory = TransformerFactory.newInstance();
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
