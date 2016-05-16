(TeX-add-style-hook
 "final_report"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("report" "12pt" "a4paper")))
   (TeX-run-style-hooks
    "latex2e"
    "tex/title"
    "tex/ch1"
    "report"
    "rep12"
    "tex/style")))

