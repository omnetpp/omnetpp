package freemarker.eclipse.editors;

import org.eclipse.swt.graphics.RGB;

/**
 * @version $Id: IColorConstants.java,v 1.1 2003/02/12 22:28:50 szegedia Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public interface IColorConstants {
	RGB COMMENT =           new RGB(128,   0,   0);
	RGB STRING=             new RGB(  0, 128,   0);
	RGB DEFAULT=            new RGB(  0,   0,   0);
	RGB DIRECTIVE=          new RGB(  0,   0, 160);
	RGB INTERPOLATION=      new RGB(255,   0, 128);
}
