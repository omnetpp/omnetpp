package org.omnetpp.scave.model;

import java.io.IOException;
import java.util.HashMap;

import org.eclipse.core.runtime.IProgressMonitor;
import org.xml.sax.ContentHandler;
import org.xml.sax.DTDHandler;
import org.xml.sax.EntityResolver;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.AttributesImpl;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.plugin.ScavePlugin;

/**
 * Transforms an IDList into a series of SAX events, used for saving it
 * into an XML file.
 */
public class IDListSAXEventGenerator implements XMLReader, IDListXMLConsts {

	private ContentHandler handler;

	private IDList data;  // the data we'll write out

	private IProgressMonitor progressMonitor;

	public IDListSAXEventGenerator(IDList data, IProgressMonitor progressMonitor) {
		this.data = data;
		this.progressMonitor = progressMonitor;
	}

	/**
	 * Generate SAX events. Parameter 'input' gets ignored (we get the data
	 * via the constructor).
	 */
	public void parse(InputSource input) throws IOException, SAXException
	{
		if (handler == null)
			throw new SAXException("No content handler");

		String nsu = ""; // NamespaceURI
        char[] indent = "\n         ".toCharArray();
		AttributesImpl atts = new AttributesImpl();
		ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        HashMap map = new HashMap();
        int lastId = 1;

        // TODO use progressMonitor

		// Note: We're ignoring setDocumentLocator() as well
		handler.startDocument();
		handler.ignorableWhitespace(indent, 0, 1);
		atts.addAttribute(nsu, ATT_VERSION, ATT_VERSION, "", FILEFORMAT_VERSION);
		handler.startElement(nsu, EL_SCAVE, EL_SCAVE, atts);

        // write the list of files
		handler.ignorableWhitespace(indent, 0, 3);
		atts.clear();
		handler.startElement(nsu, EL_FILES, EL_FILES, atts);
		FileList files = resultFileManager.getUniqueFiles(data);
		int sz = (int)files.size();
		for (int i=0; i<sz; i++) {
			File file = files.get(i);
			atts.clear();
			atts.addAttribute(nsu, ATT_FILENAME, ATT_FILENAME, "", file.getFilePath());
			String fileType = file.getFileType()==File.SCALAR_FILE ? "scalar" : file.getFileType()==File.VECTOR_FILE ? "vector" : "?";
			atts.addAttribute(nsu, ATT_TYPE, ATT_TYPE, "", fileType);

			handler.ignorableWhitespace(indent, 0, 5);
			handler.startElement(nsu, EL_FILE, EL_FILE, atts);

			// write a list of runs in this file
			RunList runs = resultFileManager.getRunsInFile(file);
			int sz2 = (int)runs.size();
			for (int j=0; j<sz2; j++) {
				Run run = runs.get(j);
				map.put(run,String.valueOf(lastId));
				atts.clear();
				atts.addAttribute(nsu, ATT_ID, ATT_ID, "", String.valueOf(lastId++));
				atts.addAttribute(nsu, ATT_NAME, ATT_NAME, "", run.getRunName());

				handler.ignorableWhitespace(indent, 0, 7);
				handler.startElement(nsu, EL_RUN, EL_RUN, atts);
				handler.endElement(nsu, EL_RUN, EL_RUN);
			}
			handler.ignorableWhitespace(indent, 0, 5);
			handler.endElement(nsu, EL_FILE, EL_FILE);
		}
		handler.ignorableWhitespace(indent, 0, 3);
		handler.endElement(nsu, EL_FILES, EL_FILES);

        // write the list of vectors/scalars
		handler.ignorableWhitespace(indent, 0, 3);
		atts.clear();
		handler.startElement(nsu, EL_DATASET, EL_DATASET, atts);
		sz = (int)data.size();
		for (int i=0; i<sz; i++) {
			ResultItem d = resultFileManager.getItem(data.get(i));
			atts.clear();
			atts.addAttribute(nsu, ATT_RUN, ATT_RUN, "", (String)map.get(d.getRun()));
			atts.addAttribute(nsu, ATT_MOD, ATT_MOD, "", d.getModuleName());
			atts.addAttribute(nsu, ATT_NAME, ATT_NAME, "", d.getName());

			handler.ignorableWhitespace(indent, 0, 5);
			handler.startElement(nsu, EL_INCLUDE, EL_INCLUDE, atts);
			handler.endElement(nsu, EL_INCLUDE, EL_INCLUDE);
		}
		handler.ignorableWhitespace(indent, 0, 3);
		handler.endElement(nsu, EL_DATASET, EL_DATASET);

		handler.ignorableWhitespace(indent, 0, 1);
		handler.endElement(nsu, EL_SCAVE, EL_SCAVE);
		handler.endDocument();
	}

	/** Allow an application to register a content event handler. */
	public void setContentHandler(ContentHandler handler) {
		this.handler = handler;
	}

	/** Return the current content handler. */
	public ContentHandler getContentHandler() {
		return this.handler;
	}

	private ErrorHandler errorHandler;

	/** Allow an application to register an error event handler. */
	public void setErrorHandler(ErrorHandler handler) {
		errorHandler = handler;
	}

	/** Return the current error handler. */
	public ErrorHandler getErrorHandler() {
		return errorHandler;
	}

	/** Parse an XML document from a system identifier (URI). */
	public void parse(String systemId) throws IOException, SAXException {
	}

	/** Return the current DTD handler. */
	public DTDHandler getDTDHandler() {
		return null;
	}

	/** Return the current entity resolver. */
	public EntityResolver getEntityResolver() {
		return null;
	}

	/** Allow an application to register an entity resolver. */
	public void setEntityResolver(EntityResolver resolver) {
	}

	/** Allow an application to register a DTD event handler. */
	public void setDTDHandler(DTDHandler handler) {
	}

	/** Look up the value of a property. */
	public Object getProperty(java.lang.String name) {
		return null;
	}

	/** Set the value of a property. */
	public void setProperty(java.lang.String name, java.lang.Object value) {
	}

	/** Set the state of a feature. */
	public void setFeature(java.lang.String name, boolean value) {
	}

	/** Look up the value of a feature. */
	public boolean getFeature(java.lang.String name) {
		return false;
	}
}
