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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

namespace omnetpp {

namespace nedxml {

/**
 * @mainpage \opp NEDXML API Reference
 *
 * This reference documents NEDXML -- a library for parsing NED and MSG files,
 * and much more. The result of parsing is a DOM-like abstract syntax tree.
 * The parsed form can be converted back into NED/MSG source (with a side effect
 * of pretty-printing it), can be serialized into XML, restored from XML, and
 * (in the case of MSG files) also serves as input for C++ code generation.
 *
 * Choose one of the more following topics:
 *
 * - @ref Overview
 * - @ref Example
 * - @ref DTD
 * - @ref Data
 * - @ref NedParser
 * - @ref XMLParser
 * - @ref XMLGenerator
 * - @ref Validation
 * - @ref NedResources
 * - @ref NedGenerator
 * - @ref CppGenerator
 *
 */

/**
 * @defgroup Overview  Overview
 *
 * @brief Overview of the library.
 *
 *
 * The NED-XML infrastructure is centered around data classes that are
 * the in-memory representation of NED. The data structure is an object tree,
 * where each XML element (e.g. &lt;param name="address" datatype="string"/&gt;)
 * is represented by an object. The central class is ASTNode, which provides
 * DOM-like generic methods to navigate the tree, enumerate and query attributes,
 * etc. Each XML element has a corresponding class, subclassed from ASTNode,
 * e.g. for the &lt;param&gt; element the class is called ParamElement. Specific
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
 * @defgroup Example  Example
 *
 * @brief An example NED file and its XML representation.
 *
 * A short NED code fragment:
 *
 * <pre>
 * @verbatim
 * //
 * // This file is part of an OMNeT++/OMNEST simulation example.
 * //
 *
 * //
 * // Generates jobs (messages) with the given interarrival time.
 * //
 * simple Source
 * {
 *     parameters:
 *         volatile double sendIaTime @unit(s);
 *         @display("i=block/source");
 *     gates:
 *         output out;
 * }
 * @endverbatim
 * </pre>
 *
 * The corresponding XML:
 *
 * <pre>
 * @verbatim
 * <?xml version="1.0" encoding="ISO-8859-1"?>
 *
 * <ned-file filename="Source1.ned">
 *     <comment locid="banner" content="//&#10;// This file is part of an OMNeT++/OMNEST simulation example.&#10;//&#10;&#10;&#10;"/>
 *     <simple-module name="Source">
 *         <comment locid="trailing" content="&#10;&#10;&#10;"/>
 *         <comment locid="banner" content="//&#10;// Generates jobs (messages) with the given interarrival time.&#10;//&#10;"/>
 *         <parameters>
 *             <param type="double" is-volatile="true" name="sendIaTime">
 *                 <property name="unit">
 *                     <property-key>
 *                         <literal type="spec" text="s" value="s"/>
 *                     </property-key>
 *                 </property>
 *             </param>
 *             <property name="display">
 *                 <property-key>
 *                     <literal type="string" text="&quot;i=block/source&quot;" value="i=block/source"/>
 *                 </property-key>
 *             </property>
 *         </parameters>
 *         <gates>
 *             <gate name="out" type="output"/>
 *         </gates>
 *     </simple-module>
 * </ned-file>
 * @endverbatim
 * </pre>
 */

/**
 * @defgroup DTD  DTD
 *
 * The DTD that describes NED XML files:
 *
 * @include ned2.dtd
 */

/**
 * @defgroup Data  Data classes
 *
 * @brief Classes in this group represent the parsed form of NED and MSG files.
 */

/**
 * @defgroup NedParser NED Parsing
 *
 * @brief Classes and functions for parsing NED and MSG files.
 */

/**
 * @defgroup XMLParser  XML Parsing
 *
 * @brief Classes and functions for deserializing a NED/MSG AST from XML.
 */

/**
 * @defgroup XMLGenerator Generating XML
 *
 * @brief Classes and functions for serializing a NED/MSG AST to XML.
 */

/**
 * @defgroup Validation  Validation
 *
 * @brief Classes and functions for validating a NED/MSG AST.
 */

/**
 * @defgroup NedResources  NED Resources
 *
 * @brief Classes for storing NED/MSG files in parsed form, and looking up
 * types in them.
 */

/**
 * @defgroup NedGenerator  Generating NED
 *
 * @brief Classes and functions for regenerating NED/MSG source from
 * their parsed form.
 */

/**
 * @defgroup CppGenerator Generating C++ code
 *
 * @brief Classes and functions for generating C++ source from a parsed MSG
 * file (opp_msgc functionality).
 */

}  // namespace nedxml

}  // namespace omnetpp

