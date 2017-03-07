/*
This file contains utility Prolog predicates which replace some that
swipl has so that the program can be run with other Prologs which don't
have them.
*/

/*			    SET PREDICATES.				*/

%Sets are represented as unordered lists.


%set_union(X, Y, Z): Z is the union of sets X & Y.
set_union([], Y, Y).
set_union([A|X], Y, Z):-
  ( is_member(A, Y) -> Z= Z1 ; Z= [A|Z1] ) ,
  set_union(X, Y, Z1).

%set_intersection(X, Y, Z): Z is the intersection of sets X & Y.
set_intersection([], _Y, []).
set_intersection([A|X], Y, Z):-
  ( is_member(A, Y) -> Z= [A|Z1] ; Z= Z1 ) ,
  set_intersection(X, Y, Z1).

%set_difference(X, Y, Z): Z is the set X - Y.
set_difference([], _Y, []).
set_difference([A|X], Y, Z):-
  ( is_member(A, Y) -> Z= Z1 ; Z= [A|Z1] ) ,
  set_difference(X, Y, Z1).

%conc(X, Y, Z): Standard append/3.  Different name to avoid clash with
%Prologs which provide it.
conc([], X, X).
conc([A|X], Y, [A|Z]):-
  conc(X, Y, Z).

%is_member(X, Xs): Standard member/2.  Different name to avoid clash with
%Prologs which provide it.
is_member(X, [X|_]).
is_member(X, [_|Xs]):-
  is_member(X, Xs).
