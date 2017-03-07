/* Scanner which illustrates debugging messages. */

%option debug

%%

[[:digit:]]+		|
[[:alpha:]]+		REJECT;
.|\n
