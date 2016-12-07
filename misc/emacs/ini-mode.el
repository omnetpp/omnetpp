;;; ini-mode.el --- major mode for editing ini files

;; Keywords: major mode ini

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; $Header: /home/omnest/cvs/omnetpp/contrib/emacs/ini-mode.el,v 1.1 2005/07/15 09:37:35 omnest Exp $
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;

;;; Commentary:
;;
;; This is a simple hack to allow syntax highlighting when editing
;; OMNeT++ .ini files with Emacs. It has been tested with Emacs 20.3.
;;
;; To make Emacs recognize .ini files put this in your .emacs:
;;
;; (autoload 'ini-mode "ini-mode" "Major Mode for editing ini files" t)
;; (setq auto-mode-alist (cons '("\\.ini\\'" . ini-mode) auto-mode-alist))
;;
;; If you put your ini-mode.el somewhere emacs doesn't find it, add this
;; to your .emacs before the two statements above:
;;
;; (setq load-path (cons "path_to_dirctory" load-path))
;;
;;

;;; Code:

(require 'derived)

(define-derived-mode ini-mode text-mode "Ini"
  "Major mode for editing ini files.
Special commands:
\\{ini-mode-map}"
(make-local-variable 'font-lock-defaults)
(setq font-lock-defaults '(ini-font-lock-keywords t))
)

(defvar ini-font-lock-keywords
  '(("#.*" . font-lock-comment-face)
    ("\".*\"" . font-lock-string-face)
    ("\\[.*\\]" . font-lock-keyword-face)
    )
  )
(provide 'ini-mode)

;; ---------------------------------------------------------------------------
;;; ini-mode.el ends here
;; ---------------------------------------------------------------------------
