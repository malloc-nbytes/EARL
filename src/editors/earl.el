;; Earl mode
(defconst earl-mode-syntax-table
  (with-syntax-table (copy-syntax-table)
    (modify-syntax-entry ?- ". 124b")
    (modify-syntax-entry ?* ". 23")
    (modify-syntax-entry ?\n "> b")
    (modify-syntax-entry ?' "\"")
    (modify-syntax-entry ?' ".")
    (syntax-table))
  "Syntax table for `earl-mode'.")

;; Function taken from:
;;  https://www.omarpolo.com/post/writing-a-major-mode.html
(defun earl-indent-line ()
  "Indent current line."
  (let (indent
        boi-p
        move-eol-p
        (point (point)))
    (save-excursion
      (back-to-indentation)
      (setq indent (car (syntax-ppss))
            boi-p (= point (point)))
      (when (and (eq (char-after) ?\n)
                 (not boi-p))
        (setq indent 0))
      (when boi-p
        (setq move-eol-p t))
      (when (or (eq (char-after) ?\))
                (eq (char-after) ?\}))
        (setq indent (1- indent)))
      (delete-region (line-beginning-position)
                     (point))
      (indent-to (* tab-width indent)))
    (when move-eol-p
      (move-end-of-line nil))))

(eval-and-compile
  (defconst earl-keywords
    '("if" "else" "while" "let" "void" "int"
      "str" "for" "fn" "return" "break"
      "struct" "char" "import" "in" "mod"
      "class" "true" "false" "none" "match"
      "when" "constructor" "this"
      ;; intrinsic functions
      "print" "println" "assert" "len" "open"
      "type" "unimplemented" "input"
      "some" "unwrap" "argv" "exit"
      "panic"
      ;; private intrinsic functions
      "__internal_move__"
      ;; list member intrinsics
      "rev" "nth" "append" "pop"
      "filter"
      ;; str member intrinsics
      "split" "remove_lines" "substr"
      ;; char member intrinsics
      "ascii"
      ;; file member intrinsics
      "read" "close" "dump" "write"
      "writelines"
      ;; option member intrinsics
      "is_none" "is_some"
      ;; function attributes
      "world" "pub" "ref"
      ;; Stdlib Modules
      "IO" "List" "Math" "Stack" "Utils"
      "Str" "Tree" "Set" "Queue"
      )))

(defconst earl-highlights
  `((,(rx (group "#" (zero-or-more nonl))) . font-lock-comment-face)
    (,(regexp-opt earl-keywords 'symbols) . font-lock-keyword-face)))

;;;###autoload
(define-derived-mode earl-mode prog-mode "earl"
  "Major Mode for editing Earl source code."
  :syntax-table earl-mode-syntax-table
  (setq font-lock-defaults '(earl-highlights))
  (setq-local comment-start "#")
  (setq-local indent-tabs-mode nil)
  (setq-local tab-width 4)
  (setq-local indent-line-function #'earl-indent-line)
  (setq-local standard-indent 2))

;;;###autoload
(add-to-list 'auto-mode-alist '("\\.earl\\'" . earl-mode))

(provide 'earl-mode)
;; End Earl mode
