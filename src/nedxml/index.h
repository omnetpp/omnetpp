//==========================================================================
//  INDEX.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Defines modules for documentation generation. Nothing useful for the
//  C++ compiler.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/**
 * @mainpage \opp NEDXML API Reference
 *
 * This reference documents NEDXML -- a library for parsing NED and MSG files,
 * and much more. The result of parsing is a DOM-like object tree, which
 * you can export as an XML file (and import it back as well), generate C++
 * code from it (a la nedc), or -- possibly after modifying it -- convert
 * it back to the original NED/MSG format.
 *
 * For a start, read the @ref Overview, or you can go to one of the more
 * specific topics:
 *
 * - @ref DTD
 * - @ref Data
 * - @ref NEDParser
 * - @ref XMLParser
 * - @ref XMLGenerator
 * - @ref Validation
 * - @ref NEDCompiler
 * - @ref NEDGenerator
 * - @ref CppGenerator
 *
 */

/**
 * @defgroup Overview  Overview
 *
 * XML is ideal as an alternative representation of NED. By supporting XML,
 * NED becomes more interoperable with other systems, and it is be possible
 * to process it with standard tools. (Note that XML is unsuitable
 * though as the <i>only</i> representation of NED information because of its
 * relatively poor readability.)
 *
 * As an example, let's see a short NED code fragment and its XML representation:
 * <pre>
 * module FDDINode
 *     parameters:
 *         address : string;
 *     gates:
 *         in: net_in;
 *         out: net_out;
 *     submodules:
 *         MAC: FDDI_MAC
 *             parameters:
 *                 mac_address = address;
 *                 promiscuous = false;
 *         ...
 * endmodule
 * </pre>
 *
 * The XML version is a bit less readable but straightforward:
 *
 * <pre>
 * @verbatim
 * <?xml version="1.0" ?>
 * <nedfile filename="fddi.ned">
 *     <module name="FDDINode">
 *         <params>
 *             <param name="address" datatype="string"/>
 *         </params>
 *         <gates>
 *             <gate gatetype="in" isvector="false" name="net_in" />
 *             <gate gatetype="out" isvector="false" name="net_out" />
 *         </gates>
 *         <submodules>
 *             <submodule name="MAC" typename="FDDI_MAC">
 *                 <substparams>
 *                     <substparam name="mac_address" value="address"/>
 *                     <substparam name="promiscuous" value="false"/>
 *                 </substparams>
 *             </submodule>
 *             ...
 *
 * @endverbatim
 * </pre>
 *
 * The above XML fragment should be quite self-explanatory for those familiar
 * with NED.
 *
 * A DTD (document type descriptor) exists to describe the format of valid
 * NED XML documents.
 *
 * The NED-XML infrastructure is centered around data classes that are
 * the in-memory representation of NED. The data structure is an object tree,
 * where each XML element (e.g. &lt;param name="address" datatype="string"/&gt;)
 * is represented by an object. The central class is NEDElement, which provides
 * DOM-like generic methods to navigate the tree, enumerate and query attributes,
 * etc. Each XML element has a corresponding class, subclassed from NEDElement,
 * e.g. for the &lt;param&gt; element the class is called ParamNode. Specific
 * element classes provide a stricter, more typed interface to access the data
 * tree.
 *
 * Several components build around the data classes:
 *    - <i>input filters</i> build a NEDXML object tree using data from some external
 *      data source. Currently available input filters are <b>NED parser</b>
 *      and <b>XML parser</b>.
 *    - <i>output filters</i> take a NEDXML object tree and transform it into some
 *      other format. Currently available output filters are <b>NED generator</b>,
 *      <b>XML generator</b>, and <b>C++ code generator</b> (for nedc).
 *    - <i>validators</i> check that a NEDXML object tree contains valid information,
 *      that is, it conforms to the DTD and other, stricter rules. Currently implemented
 *      validators consist of several stages which can check the tree up to the level
 *      required by nedc.
 *
 * An exciting "output filter", <b>network builder</b> is implemented as an optional
 * part of the simulation library. Network builder can set up a simulation model
 * on the fly, provided that all simple modules are present in the executable.
 *
 * Based on the infrastructure, a NED compiler can be assembled from a NED
 * parser and a C++ code generator component. GNED can utilize the NED parser
 * and NED generator components. Both nedc and GNED will be able to import and
 * export XML by just adding the XML parser and XML generator components. The
 * infrastructure makes it easier to build models dynamically. For example, when
 * building networks from data coming from a database, one might
 * let the database query produce XML (several databases are already capable
 * of that), then apply an XSLT transformation to convert to NED-XML if
 * needed. Then one might apply the network builder to set up the network directly;
 * or use the NED generator to create NED source for debugging purposes.
 *
 * The central idea is to have a <i>common data structure</i> for representing NED,
 * and this data stucture can <i>act as a hub</i> for connecting different data formats,
 * data sources and applications.
 */


/**
 * @defgroup DTD  DTD
 *
 * The DTD that describes NED XML files:
 *
 * @include ned.dtd
 */

/**
 * @defgroup Data  Data classes
 *
 */

/**
 * @defgroup NEDParser NED Parsing
 */

/**
 * @defgroup XMLParser  XML Parsing
 */

/**
 * @defgroup XMLGenerator Generating XML
 */

/**
 * @defgroup Validation  Validation
 *
 */

/**
 * @defgroup NEDCompiler  Controlling the compilation process
 *
 */

/**
 * @defgroup NEDGenerator  Generating NED
 *
 */

/**
 * @defgroup CppGenerator Generating C++ code
 *
 */



