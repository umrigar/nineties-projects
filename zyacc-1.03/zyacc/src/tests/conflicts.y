/* Tests for conflicts. */
%expect 4
%%
S 
  : A 'z'
  | B 'z'
  | C 'z'
  ;
/* 2 RR and 2 SR conflicts, tho' not all may be reported. */
A 
  : 'a'
  | 'a'
  | 'a' 'z'
  ;
/* Should report 1 SR conflict (First production masked by %test. */
B
  : 'b' %test(0)
  | 'b'
  | 'b' 'z'
  ;
/* 2 RR conflicts: both should be reported. */
C
  : 'c'
  | 'c'
  ;
