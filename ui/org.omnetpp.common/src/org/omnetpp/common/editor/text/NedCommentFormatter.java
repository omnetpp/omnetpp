package org.omnetpp.common.editor.text;

import java.util.ArrayList;
import java.util.regex.Matcher;

import org.apache.commons.lang3.StringEscapeUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.StringUtils.IRegexpReplacementProvider;


/**
 * Formatting of NEDDOC comments.
 *
 * @author Levy, Andras
 */
public class NedCommentFormatter {
    private static final String ANY_RECOGNIZED_HTML_TAG = StringUtils.join(Keywords.DOC_TAGS, "|");

    public interface INeddocProcessor {
        String process(String comment);
    }

    /**
     * Renders a NEDDOC comment into HTML. The intended use of INeddocProcessor is
     * to add hyperlinks on NED types mentioned in the text.
     *
     * @param comment  The comment to be processed
     * @param brief    If true, only the first paragraph of the comment will be included
     * @param tildeMode If true, then ~ is special before an identifier, otherwise \ is special
     * @param processor When not null, it will be invoked in the middle of the creation process.
     */
    public static String makeHtmlDocu(String comment, boolean brief, boolean tildeMode, INeddocProcessor processor) {
        // kill CRs, and add sentries to facilitate processing
        comment = comment.replace("\r", "");
        comment = "\n\n" + comment + "\n\n";

        // remove '//#' lines (those are comments to be ignored by documentation generation)
        comment = comment.replaceAll("(?s)(?<=\n)[ \t]*//#.*?\n", "\n");

        // remove '// ', '/// ' and '//////...' from beginning of lines
        comment = comment.replaceAll("(?s)\n[ \t]*//+ ?", "\n");

        // hyperlink URLs in the text
        if (comment.contains("http://") || comment.contains("https://")) {
            // protect <nohtml> sections
            ArrayList<String> nohtmlList1 = new ArrayList<String>();
            comment = extract(comment, "(?s)<nohtml>(.*?)</nohtml>", nohtmlList1, "nohtml");

            // protect <a> tags which may contain URLs in both attribute and body
            ArrayList<String> aTagList = new ArrayList<String>();
            comment = extract(comment, "(?is)(<a\\s.*?>.*?</a>)", aTagList, "a");

            // also protect all opening tags, because they might contain URLs in attributes (e.g. <img src="...">)
            ArrayList<String> tagOpenersList = new ArrayList<String>();
            comment = extract(comment, "(?s)(<.*?>)", tagOpenersList, "tag");

            // hyperlink URLs in the text. URL extends to next whitespace, but we exclude trailing space, comma, etc.
            comment = comment.replaceAll("(?i)(http(s?)://[^\\s]+[^\\s.,;\"'])", "<a href=\"$1\" target=\"_blank\">$1</a>");

            // restore all the previous
            comment = restore(comment, "tag", tagOpenersList);
            comment = restore(comment, "a", aTagList);
            comment = restore(comment, "nohtml", nohtmlList1);
        }

        // extract existing <pre> sections to prevent tampering inside them
        final ArrayList<String> preList = new ArrayList<String>();
        comment = StringUtils.replaceMatches(comment, "(?s)<pre>(.*?)</pre>", new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                preList.add(matcher.group(0));

                return "<pre" + (preList.size() - 1) + "/>";
            }
        });

        // a plain '-------' line outside <pre> is replaced by a divider (<hr> tag)
        comment = comment.replaceAll("(?s)\n[ \t]*------+[ \t]*\n", "\n<hr/>\n");

        // lines outside <pre> containing whitespace only count as blank
        comment = comment.replaceAll("(?s)\n[ \t]+\n", "\n\n");

        // insert blank line (for later processing) in front of lines beginning with '- ' or '-# '
        comment = comment.replaceAll("(?s)\n( *-#? )", "\n\n$1");

        // if brief comment, keep only the 1st paragraph
        if (brief)
           comment = comment.replaceAll("(?s)(.*?[^ \t\n].*?)\n\n.*", "$1\n\n");

        // format @author, @date, @todo, @bug, @see, @since, @warning, @version
        comment = comment.replaceAll("@author\\b", "\n\n<b>Author:</b>");
        comment = comment.replaceAll("@date\\b", "\n\n<b>Date:</b>");
        comment = comment.replaceAll("@todo\\b", "\n\n<b>TODO:</b>");
        comment = comment.replaceAll("@bug\\b", "\n\n<b>BUG:</b>");
        comment = comment.replaceAll("@see\\b", "\n\n<b>See also:</b>");
        comment = comment.replaceAll("@since\\b", "\n\n<b>Since:</b>");
        comment = comment.replaceAll("@warning\\b", "\n\n<b>WARNING:</b>");
        comment = comment.replaceAll("@version\\b", "\n\n<b>Version:</b>");
        comment = comment.replaceAll("\n\n\n+", "\n\n");

        // wrap paragraphs NOT beginning with '-' into <p></p>.
        // well, we should write "paragraphs not beginning with '- ' or '-# '", but
        // how do you say that in a Perl regex?
        // (note: (?=...) and (?<=...) constructs are lookahead and lookbehind assertions,
        // see e.g. http://tlc.perlarchive.com/articles/perl/pm0001_perlretut.shtml).
        comment = comment.replaceAll("(?s)(?<=\n\n)[ \t]*([^- \t\n].*?)(?=\n\n)", "<p>$1</p>");

        // wrap paragraphs beginning with '-' into <li></li> and <ul></ul>
        // every 3 spaces increase indent level by one.
        comment = comment.replaceAll("(?s)(?<=\n\n)          *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><ul><ul><li>$1</li></ul></ul></ul></ul>");
        comment = comment.replaceAll("(?s)(?<=\n\n)       *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><ul><li>$1</li></ul></ul></ul>");
        comment = comment.replaceAll("(?s)(?<=\n\n)    *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><li>$1</li></ul></ul>");
        comment = comment.replaceAll("(?s)(?<=\n\n) *-[ \t]+(.*?)(?=\n\n)", "  <ul><li>$1</li></ul>");
        for (int i = 0; i < 4; i++) {
            comment = comment.replaceAll("(?s)</ul>[ \t\n]*<ul>", "\n\n  ");
        }

        // wrap paragraphs beginning with '-#' into <li></li> and <ol></ol>.
        // every 3 spaces increase indent level by one.
        comment = comment.replaceAll("(?s)(?<=\n\n)          *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><ol><ol><li>$1</li></ol></ol></ol></ol>");
        comment = comment.replaceAll("(?s)(?<=\n\n)       *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><ol><li>$1</li></ol></ol></ol>");
        comment = comment.replaceAll("(?s)(?<=\n\n)    *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><li>$1</li></ol></ol>");
        comment = comment.replaceAll("(?s)(?<=\n\n) *-#[ \t]+(.*?)(?=\n\n)", "  <ol><li>$1</li></ol>");
        for (int i = 0; i < 4; i++) {
            comment = comment.replaceAll("(?s)</ol>[ \t\n]*<ol>", "\n\n  ");
        }

        // now we can trim excess blank lines
        comment = comment.replaceAll("\n\n+", "\n");

        // now we can put back <pre> regions
        comment = StringUtils.replaceMatches(comment, "<pre(\\d+)/>", new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                return preList.get(Integer.parseInt(matcher.group(1)));
            }
        });

        // escape html content, elements will be restored later, after processor has been called
        comment = StringEscapeUtils.escapeHtml4(comment);

        // restore unicode chars, otherwise Chinese identifiers won't be hyperlinked
        // (they can remain as they are anyway, because we generate html in utf-8 encoding)
        comment = StringUtils.replaceMatches(comment, "&#([0-9]+);", new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                char code = (char)Integer.parseInt(matcher.group(1));
                return code < 128 ? null : new String(new char[] {code});
            }
        });

        // extract <nohtml> sections to prevent substituting inside them;
        // also backslashed words to prevent putting hyperlinks on them
        final ArrayList<String> nohtmlList = new ArrayList<String>();
        comment = StringUtils.replaceMatches(comment, "(?s)&lt;nohtml&gt;(.*?)&lt;/nohtml&gt;", new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                nohtmlList.add(matcher.group(1));
                return "<nohtml" + (nohtmlList.size() - 1) + "/>";
            }
        });

        // put hyperlinks on type names
        if (processor != null)
            comment = processor.process(comment);

        // restore accented characters e.g. "&ouml;" from their "&amp;ouml;" forms
        comment = comment.replaceAll("&amp;([a-z]+);", "&$1;");

        // restore " from &quot; (important for attributes of html tags, see below)
        comment = comment.replaceAll("&quot;","\"");

        // restore html elements (pre elements must be unescaped here) and leave other content untouched
        comment = comment.replaceAll("&lt;((" + ANY_RECOGNIZED_HTML_TAG + ")( [^\n]*?)?/?)&gt;", "<$1>");
        comment = comment.replaceAll("&lt;(/(" + ANY_RECOGNIZED_HTML_TAG + "))&gt;", "<$1>");

        // remove backslashes and tildes before identifiers; double backslashes and tildes become single ones
        // (note: this will only process tildes and backslashes not hyperlinked by INeddocProcessor already)
        //  !tildeMode                          tildeMode
        // \TCP    ->  TCP                   ~TCP   -> TCP
        // \Hello  ->  Hello                 ~Hello -> Hello
        // \\TCP   ->  \TCP                  ~~TCP  -> ~TCP
        // \100    ->  \100                  ~100   -> 100
        // \\100   ->  \\100                 ~~100  -> ~~100
        String pattern = tildeMode ? "(?i)(~+)([a-z_])" : "(?i)(\\\\+)([a-z_])";
        comment = StringUtils.replaceMatches(comment, pattern, new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                String prefix = matcher.group(1);
                return prefix.substring(0, prefix.length() / 2) + matcher.group(2);
            }
        });

        // put back <nohtml> sections
        comment = StringUtils.replaceMatches(comment, "<nohtml(\\d+)/>", new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                return nohtmlList.get(Integer.parseInt(matcher.group(1)));
            }
        });

        return comment;
    }

    private static String extract(String comment, String regex, final ArrayList<String> extractions, String token) {
        return StringUtils.replaceMatches(comment, regex, new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                extractions.add(matcher.group(1));
                return "<" + token + "!" + (extractions.size() - 1) + "/>";
            }
        });
    }

    private static String restore(String comment, String token, final ArrayList<String> extractions) {
        return StringUtils.replaceMatches(comment, "<" + token + "!(\\d+)/>", new IRegexpReplacementProvider() {
            public String getReplacement(Matcher matcher) {
                return extractions.get(Integer.parseInt(matcher.group(1)));
            }
        });
    }

    /**
     * Formats a NED comment as a one-line doc string. If it is longer than the
     * given max length, it gets truncated. Only the first sentence or the first paragraph
     * is returned.
     */
    public static String makeBriefDocu(String comment, int maxlen) {
        if (comment == null)
            return null;

        // BEWARE: Java's multiline regexp mode "(?m)" seems to be broken:
        // negative sets "[^...]", "\s" etc may consume newlines, so substitutions
        // may change the number of lines in the string! This corresponds to the
        // documentation, but is inconsistent with Perl!

        comment = comment.replaceAll("(?m)^\\s*//#.*$", "");      // remove '//#' lines
        comment = comment.replaceAll("(?m)^\\s*//", "");          // remove "//"'s
        comment = comment.replaceFirst("(?s)\n[ \t]*\n.*", "");   // keep only first paragraph, or:
        comment = comment.replaceFirst("(?s)\\.\\s.*", ".");      // extract the first sentence only (up to the first period)

        // throw out known HTML tags (and leave alone any XML fragments that the docu might contain)
        comment = comment.replaceAll("<(" + ANY_RECOGNIZED_HTML_TAG + ")( [^\n]*?)?>", "");
        comment = comment.replaceAll("</(" + ANY_RECOGNIZED_HTML_TAG + ")>", "");

        comment = comment.replaceAll("(?s)\\s+", " ");  // make it one line, and normalize whitespace
        if (comment.length() > maxlen)
            comment = comment.substring(0, maxlen)+"...";  // abbreviate
        return comment.trim();
    }

    /**
     * Formats a NED comment as plain text. Basically, strips "//" from the lines,
     * and removes explicit HTML formatting from the string.
     */
    public static String makeTextDocu(String comment) {
        if (comment == null)
            return null;

        // BEWARE: Java's multiline mode "(?m)" seems to be broken, see in method above!

        comment = comment.replaceAll("(?m)^[ \t]*//#.*$", ""); // remove '//#' lines
        comment = comment.replaceAll("(?m)^[ \t]*//+ ?", "");  // remove "//"'s

        // throw out known HTML tags (and leave alone any XML fragments that the docu might contain)
        comment = comment.replaceAll("<(" + ANY_RECOGNIZED_HTML_TAG + ")( [^\n]*?)?>", "");
        comment = comment.replaceAll("</(" + ANY_RECOGNIZED_HTML_TAG + ")>", "");

        comment = comment.replaceAll("(?s)[ \t]+\n","\n");     // remove whitespace from end of lines
        comment = comment.replaceAll("(?s)\n\n\n+","\n\n");    // remove multiple blank lines
        return comment.trim();
    }

}
