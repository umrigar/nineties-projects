#sed script to extract nodes for each long-option in the form of a sed
#script, which when run will substitute xref-references to the long
#option name, with references to the node in which that option is defined.
#Assumes each long option is listed in @item or @itemx entry.  
#Necessitated because @node reference granularity for texinfo is too coarse.

/^@node/{
  s/^@node[ ]*\([^,]*\).*/@xref{\1}/
  h
}
/^@item.* --/{
  s&^@item.* --\([-a-zA-Z0-9]*\).*&s/^@xref{\1}/&
  G
  s/\n//
  s&$&/&p
}
$a\
  /^@xref/{t\
   d\
  }
