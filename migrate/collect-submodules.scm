(require (lib "list.ss"))
(require (lib "xml.ss" "xml"))
(require (lib "match.ss"))
(require (lib "pregexp.ss"))
(require (planet "sxml.ss" ("lizorkin" "sxml.plt" 1 4)))
(require (planet "ssax.ss" ("lizorkin" "ssax.plt" 1 3)))
(require (planet "sxml-match.ss" ("jim" "sxml-match.plt" 1 0)))

; see
; http://modis.ispras.ru/Lizorkin/sxml-tutorial.html

(define input-file-name "allnedfiles.xml")
(define output-file-name "output.xml")

(define document
  (ssax:xml->sxml (open-input-file input-file-name) '()))

(define (write-document)
;  (delete-file output-file-name)
  (call-with-output-file output-file-name
    (lambda (output-file)
      (srl:sxml->xml document output-file))))

;<property name="display">
; <property-key>
;  <literal type="string" text="&quot;p=89,100;i=block/source&quot;" value="p=89,100;i=block/source"/>
; </property-key>
;</property>

#;(define (replace-values pattern replacement)
  ((sxpath (list "//property/property-key/literal/@value" 
                 (lambda (nodes ignore)
                   (map (lambda (node)
                          (set-first! node (regexp-replace* pattern (first node) replacement)))
                        (map rest nodes))
                   nodes)))
   document))

#;(define replacer
  (sxml:modify
   `("//property//property-key/literal"
     ,(lambda (node context base-node)
        '(a)))))

; interesting example
#;((sxml:modify '("purchaseOrder/recipient" delete))
 (sxml:document "http://modis.ispras.ru/Lizorkin/XML/po-short.xml"))

;(search-values "i=block/source")

;(replace-values "i=block/source" "i=block/alma")

;(replace-values "i=block" "i=blokk")

;(replacer document)


;(write-document)

(define (find-display-properties node pattern)
  ((sxpath (list "//property[@name='display']/property-key/literal/@value" 
                 (lambda (nodes ignore)
                   (filter
                    (lambda (node)
                      (regexp-match pattern (second node)))
                    nodes))))
   node))

(define (find-submodules-with-icon)
  ((sxpath (list "//submodule" 
                 (lambda (submodules ignore)
                   (filter
                    (lambda (submodule)
                      (not (empty? (find-display-properties submodule "i="))))
                    submodules))))
   document))

(define (get-attribute-value node path)
  (define att-name-value ((sxpath path) node))
  (if (empty? att-name-value)
      '()
      (second (first att-name-value))))

(define (collect-moduletype-icon-name-pairs)
  (map (lambda (submodule)
         (list 
          (get-attribute-value submodule "@type")
          (regexp-replace 
           ".*i=([a-z/A-Z0-9_]+).*"
           (get-attribute-value submodule "//property[@name='display']/property-key/literal/@value")
           "\\1")))
       (find-submodules-with-icon)))

(define ss (first (find-submodules-with-icon)))

(define s2 (collect-moduletype-icon-name-pairs))
