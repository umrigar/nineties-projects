%s S1 S2 S3 S4 S5
%%
^"#1".*		{ printf("Entering S1"); yy_push_state(S1); }
^"#2".*	   	{ printf("Entering S2"); yy_push_state(S2); }
^"#3".*	   	{ printf("Entering S3"); yy_push_state(S3); }
^"#4".*   	{ printf("Entering S4"); yy_push_state(S4); }
^"#5".*   	{ printf("Entering S5"); yy_push_state(S5); }
^"#END".* 	{ int s= yy_top_state();
                  if (s == INITIAL) { 
		    printf("Returning to INITIAL"); 
		  }
		  else {
		    printf("Returning to S%d", 
			   s == S1 ? 1 : s == S2 ? 2 
			     : s == S3 ? 3 : s == S4 ? 4 : 5); 
		  }
                  yy_pop_state(); 
                }
