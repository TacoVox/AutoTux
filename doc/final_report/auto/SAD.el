(TeX-add-style-hook
 "SAD"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("report" "11pt" "a4paper")))
   (TeX-run-style-hooks
    "latex2e"
    "tex/title"
    "report"
    "rep11"
    "sadstyle")))

