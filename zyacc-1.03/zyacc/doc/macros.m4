m4_divert(-1)
m4_changequote({[, ]})
m4_define({[m4_c_include]}, 
          {[m4_esyscmd(expand $1 | ../c2tex/c2tex -l c $2)]})
m4_define({[m4_l_include]}, 
	  {[m4_esyscmd(expand $1 | ../c2tex/c2tex -l lex $2)]})
m4_define({[m4_y_include]}, 
	   {[m4_esyscmd(expand $1 | ../c2tex/c2tex -l yacc $2)]})
m4_divert(0)