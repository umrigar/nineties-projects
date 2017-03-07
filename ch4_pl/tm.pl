/*

Prolog routines for Turing machine interpreter tm.fc.

*/

%first_instruction(Prog, I): I is the first instruction in TM program Prog.
first_instruction([_:I|_], I).

%rest(Prog, Rest): Rest is the rest (barring the first statement) of TM 
%program Prog.
rest([_|Rest], Rest).

%instruction_operator(I, Op): Op is the operator for TM instruction I.
instruction_operator(I, Op):-
  functor(I, Op, _).

%first_sym(Tape, Z): Z is the current symbol on the TM tape.
first_sym([], b).
first_sym([S|_], S).

%new_tail(Label, Prog, Z): Z is the tail of TM program Prog starting with
%label Label.
new_tail(Label, Tail, Z):-
  Tail = [L:_|T] ,
  ( Label = L -> Z= Tail ; new_tail(Label, T, Z) ).

