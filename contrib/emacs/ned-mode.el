;;; ned-mode.el --- major mode for editing OMNeT++ ned files

;; Keywords: major mode omnetpp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; $Header: /tools/lib/CVSROOT/site-lisp/ned-mode.el,v 1.4 1999/07/05 07:41:53
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
;; OMNeT++ .ned files with Emacs. It has been tested with Emacs 20.3.
;;
;; To make Emacs recognize .ned files put this in your .emacs:
;;
;; (autoload 'ned-mode "ned-mode" "Major Mode for editing Ned files" t)
;; (setq auto-mode-alist (cons '("\\.ned\\'" . ned-mode) auto-mode-alist))
;;
;; If you put your ned-mode.el somewhere emacs doesn't find it, add this
;; to your .emacs before the two statements above:
;;
;; (setq load-path (cons "path_to_dirctory" load-path))
;;
;;

;;; Code:

(require 'derived)

(define-derived-mode ned-mode text-mode "Ned"
  "Major mode for editing OMNeT++ Ned files.
Special commands:
\\{ned-mode-map}"
(make-local-variable 'font-lock-defaults)
(setq font-lock-defaults '(ned-font-lock-keywords t))
)

(defvar ned-font-lock-keywords
  '(("//.*" . font-lock-comment-face)
    ("\".*\"" . font-lock-string-face)
    ("\\<\\(allowunconnected\\|if\\)\\>" . font-lock-keyword-face)
    ("\\<\\(types\\|submodules\\|gates\\|input\\|output\\|inout\\|parameters\\|connections\\)\\>.*:" 1 font-lock-keyword-face)
    ("\\<\\(package\\|property\\|channel\\|channelinterface\\|simple\\|module\\|moduleinterface\\|network\\|const\\|volatile\\|extends\\|index\\|like\\|default\\|sizeof\\|this\\|true\\false\\||input\\|int\\|double\\|string\\|bool\\|char\\|xml\\|xmldoc\\)\\>" 1 font-lock-keyword-face)
    ("\\(package\\|import\\|channel\\|channelinterface\\|simple\\|moduleinterface\\|module\\|network\\)[ \t]*\\([A-Za-z0-9_-]*\\)" 2 font-lock-function-name-face)
;;    ("\\(.*\\)[ \t]*:" 1 font-lock-function-name-face)
    ("[ \t]*\\(for\\).*\\(\\.\\.\\|to\\).*\\(do\\)"
     (1 font-lock-keyword-face)
     (2 font-lock-keyword-face)
     (3 font-lock-keyword-face))
    ("[ \t]*endfor" . font-lock-keyword-face)
    ("<?-->?". font-lock-keyword-face)
    ("=[ \t]*\\(input\\)[ \t]*(" 1 font-lock-keyword-face)
    )
  )
(provide 'ned-mode)

;; ---------------------------------------------------------------------------
;;; ned-mode.el ends here
;; ---------------------------------------------------------------------------

