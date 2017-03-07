 /* -*-c-*- Test for handling of 8-bit chars. */
%%
^\240.+		printf("{"); ECHO; printf("}");
.+		printf("["); ECHO; printf("]");
