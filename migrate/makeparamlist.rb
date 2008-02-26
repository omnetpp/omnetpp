#
# Extracts all parameters into a text file
#

require "rexml/document"

# open and parse the new document
doc = REXML::Document.new(File.new("inetframework.xml"))
puts "Document parsed"

out = File.open("defaultparams.txt.new", "w")
doc.elements.each("*/*/compound-module | */*/simple-module") { |mod|
    first = true
    mod.elements.each("parameters/param") { |param|
        if first
            out << "\n" << mod.attributes["name"] << "\n"
            first = false
        end
        out << "\t" << param.attributes["name"].ljust(15) << "\t" << param.attributes["type"] << "\t" << "TBD\n"
    }
}
out.close
