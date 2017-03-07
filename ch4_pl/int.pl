/*

Interpreter for flow chart language of Ch. 3 & 4 of Jones, Gomard & Sestoft.


In the following grammar, non-terminals start with upper-case.  The only
meta-chars are ':', '|' and ';'.  Non-terminals starting with the string
"Prolog" are not defined further, but have the meaning intended by their
name.

We fake nice syntax, using the Prolog reader.  The most severe restriction
is that where normal languages allow single statements, we require blocks
enclosed within braces.  Semi-colons are used as statement separators --- in
particular there can never be a semi-colon before a closing brace.  The
then-part and else-part of an if-then-else must also be usually enclosed
within braces --- the only exception is when both the then and else parts
are simple goto statements.

Expressions use normal function/operator syntax.  The only strange operator
is the prefix tilde --- used similarly to the Lisp quote function to prevent
evaluation of its argument.

Labels must be Prolog vars. When programs are read in as terms, along with
other stuff, the same Prolog var should not be used for different purposes.

Pgm			: Block0 .
			;

Block0			: { read NameList ; CmdSeq }
			;
Block			: { CmdSeq }
			;
CmdSeq			: %empty
			| NonEmptyCmdSeq
			;
NonEmptyCmdSeq		: Cmd ';' NonEmptyCmdSeq
			| Cmd
			;
LabelledCmd		: Label ':' LabelledCmd
			| Cmd
			;
Cmd			: Name ':=' Exp
			| goto Label
			| if Exp then Block else Block
			| if Exp then Block
			| if Exp then goto Label 
			| if Exp then goto Label else goto Label
			| do Block while Exp
			| while Exp do Block
			| case Exp of { Cases }
			| return Exp
			;
Cases			: Case ';' Cases
			| Case
			;
Case			: PrologTerm ':' CaseBlock
			| default ':' Block
			;
CaseBlock		: PrologTerm ':' CaseBlock
			| Block
			;
Exp			: Exp RelOp Exp
			| PrologInteger
			| Name
			| ( Exp )
			| ~ PrologTerm	%Quoted (unevaluated) Prolog term.
			| PrologFunctor(ExpList) %Call Prolog pred.
			;
RelOp			: = | \= | < | =< | > | >=
			;
ExpList			: ExpList , Exp
			| Exp
			;
NameList		: NameList , Name
			| Name
			;
Name			: PrologAtom
			;
Label			: PrologVar
			;
	

*/

/*			    OPERATOR DECLARATIONS.			*/
:-op(999, xfy, ':').	%Labels, case values.
:-op(998, xfx, ':=').	%Assignment.
:-op(998, fx, if).
:-op(997, xfy, then).
:-op(997, xfx, else).
:-op(998, fx, do).	%do Block while Exp.
:-op(997, xfx, while).	%do Block while Exp.
:-op(998, fx, while).	%while Exp do Block.
:-op(997, xfx, do).	%while Exp do Block.
:-op(998, fx, case).
:-op(997, xfx, of).
:-op(998, fx, read).
:-op(998, fx, return).
:-op(996, fx, goto).
:-op(199, fx, ~).	%Quote.

/*			TOP-LEVEL INTERPRETER.				*/

%int(PrgFile): Read program from file PrgFile.  Prompt user for values
%for read-variables.  Run program and output result.  Repeat, until user
%enters an invalid list of values.
int(PrgFile):-
  int(PrgFile, user).
int(PrgFile, OutFile):-
  see(PrgFile) , read(RawPrg) , seen ,
  desugar(RawPrg, Prg) , 
  Prg= [read NameList|RestPrg] , length(NameList, L) ,
  store_init(InitStore) , 
  repeat ,
    clear_labels ,
    write('Enter input (file(F) for input from file F; [] to quit): ') ,
    read(Input) ,
    ( Input = []-> 
        true
      ;
      ( Input = file(F)-> 
          see(F) , read(ValList) , seen , length(ValList, L) 
        ; ValList= Input
      ) ,
      store_add(NameList, ValList, InitStore, Store) ,
      statistics(cputime, TBegin) ,
      int(RestPrg, Store, Z) , 
      statistics(cputime, TEnd) ,
      Time is TEnd - TBegin ,
      write('Time = ') , write(Time) , write(' seconds.') , nl ,
      tell(OutFile) , ( Z = prg(OutPrg)-> out_prg(OutPrg) ; write(Z) ) , told ,
      nl , 
      fail
    ).


%int(Prg, Store, Z): Z is the result of interpreting flow chart program Prg
%given initial store Store.
int(Pgm, Store, Z):-
  run(Pgm, Store, Pgm, Z).




/*				DESUGAR.				*/

%desugar(Prg, PrgZ): Translate program from sugared external form
%Prg to internal form PrgZ.  Translate structured constructs.  Translate
%do-while, while-do and case statements into equivalent if and gotos.
%Ensure that for all if statements, then and else parts consist of only
%goto statements.  Linearize outermost block by removing all nested blocks.
%Add an initial label after first read.

t(X):-
  see(X), read(T), seen, desugar(T, TZ) , out_prg(TZ) , fail.

desugar({Read; RestPrg}, [Read, label(init)|RestPrgZ]):-
  translate_cmd_seq(RestPrg, RestPrgX, []) ,
  goto_opt(RestPrgX, RestPrgZ) ,
  make_labels(RestPrgZ).

%translate_block(Prg, PrgZ, PrgZTail): Translate Prg into difference-list
%PrgZ-PrgZTail.
translate_block({}, Prg, Prg).
translate_block({Prg}, PrgZ, PrgZTail):-
  translate_cmd_seq(Prg, PrgZ, PrgZTail).

translate_cmd_seq(CmdSeq, PrgZ, PrgZTail):-
  CmdSeq= (Cmd ; CmdSeq1) ->
    translate_cmd(Cmd, PrgZ, PrgX) , translate_cmd_seq(CmdSeq1, PrgX, PrgZTail)
  ; translate_cmd(CmdSeq, PrgZ, PrgZTail).

%translate_cmd(Cmd, CmdZ, CmdZTail): Translate Cmd into difference-list
%CmdZ-CmdZTail.

translate_cmd(Label:Cmd, [label(Label)|CmdZ], CmdZTail):-
  ! ,
  ( var(Label)-> true ; error(['Label ', Label, ' not a Prolog variable.']) ) ,
  translate_cmd(Cmd, CmdZ, CmdZTail).

translate_cmd(while E do Block, 
	      [label(Loop), 
	       if E then goto Cont else goto Done, label(Cont)|CmdsZ], 
	      CmdsZTail):-
  ! ,
  translate_block(Block, CmdsZ, [goto Loop, label(Done)|CmdsZTail]).

translate_cmd(do Block while E,
	      [label(Loop) | CmdsZ], CmdsZTail):-
  ! ,
  translate_block(Block, CmdsZ, 
	          [if E then goto Loop else goto EndLoop, label(EndLoop)
	           |CmdsZTail]).

translate_cmd(if E then ThenBlock else ElseBlock, 
	      [if E then goto ThenLabel else goto ElseLabel,
	       label(ThenLabel) | ThenCode],
	      CmdsZTail):-
  functor(ThenBlock, {}, _) , functor(ElseBlock, {}, _) ,
  ! ,
  translate_block(ThenBlock, ThenCode, 
	          [goto EndIf, label(ElseLabel)|ElseCode]) ,
  translate_block(ElseBlock, ElseCode, [label(EndIf) | CmdsZTail]).

translate_cmd(if E then ThenBlock, 
	      [if E then goto ThenLabel else goto EndIf,
	       label(ThenLabel) | ThenCode],
	      CmdsZTail):-
  functor(ThenBlock, {}, _) ,
  ! ,
  translate_block(ThenBlock, ThenCode, 
	          [label(EndIf)|CmdsZTail]).


translate_cmd(if E then goto ThenLabel, 
	      [if E then goto ThenLabel else goto EndIf, 
	       label(EndIf)|CmdsZTail],
	      CmdsZTail):-
  !.


translate_cmd(case E of { Cases }, CmdsZ, CmdsZTail):-
  ! ,
  translate_cases(Cases, E, 
		  default(_DefaultCode, _DefaultCodeTail), EndCase,
		  CmdsZ, [label(EndCase)|CmdsZTail]).

translate_cmd(Cmd, [Cmd|Tail], Tail). %Not a structured command.

translate_cases(Cases, E, Default, EndCase, CmdsZ, CmdsZTail):-
  Cases = (Case ; Cases1) ->
    translate_case(Case, E, Default, EndCase, CmdsZ, CmdsX) ,
    translate_cases(Cases1, E, Default, EndCase, CmdsX, CmdsZTail)
  ; translate_case(Cases, E, Default, EndCase, CmdsZ, DefaultCode) ,
    Default = default(DefaultCode, CmdsZTail) ,
    ( var(DefaultCode)-> DefaultCode= CmdsZTail ; true ).


translate_case(default: Block, _E, default(DefaultCode, DefaultCodeTail),
	       _EndCase, CmdsZ, CmdsZ):-
  (functor(Block, {}, _)-> true ; error(['Invalid default case.']) ) ,
  ! ,
  translate_block(Block, DefaultCode, DefaultCodeTail).

translate_case(V : Case, E, _Default, EndCase, 
	       [if (E = V) then goto CaseLabel else goto NextLabel|CmdsZ], 
	       CmdsZTail):-
  translate_case_lo(Case, E, NextLabel, CaseLabel, EndCase, CmdsZ, CmdsZTail).

translate_case_lo(V : Case, E, NextLabel, CaseLabel, EndCase, 
		  [label(NextLabel), 
		   if (E = V) then goto CaseLabel else goto NextLabel1|CmdsZ],
  		  CmdsZTail):-
  ! ,
  translate_case_lo(Case, E, NextLabel1, CaseLabel, EndCase, CmdsZ, CmdsZTail).

translate_case_lo(Block, _E, NextLabel, CaseLabel, EndCase, 
		  [label(CaseLabel)|CmdsZ], CmdsZTail):-
  (functor(Block, {}, _)-> true ; error(['Invalid case block.']) ) ,
  translate_block(Block, CmdsZ, [goto EndCase, label(NextLabel)|CmdsZTail]).
		  


/*			    GOTO OPTIMIZE.				*/

%goto_opt(Prg, PrgZ): Optimize program, removing successive labels by 
%unification.  Also if labelled statement is a goto, then unify goto
%target with the statement label (thus chaining thru gotos).  If a
%goto is preceeded by a control statement (if or goto) and it does not 
%loop to itself then it can be removed, since it becomes redundant 
%after the preceeding optimization.
%We cash in here on using Prolog vars as labels, letting Prolog do all
%the work without explicitly needing some sort of symbol-table.


goto_opt(Prg, PrgZ):-
  goto_opt(Prg, [], [], PrgZ).

%goto_opt(Prg, LastOp, LastLabel, OptPrg):  OptPrg is the optimized version
%of Prg.  LastOp was the last non-label op, and LastLabel is either label(L)
%if labels occurred after the LastOp, [] otherwise.
goto_opt([], _LastOp, _Label, []).
goto_opt([Cmd|Cmds], LastOp, Label, CmdsZ):-
  goto_opt(Cmd, Cmds, LastOp, Label, CmdsZ).

goto_opt(Cmd, Cmds, LastOp, LastLabel, CmdsZ):-
  Cmd = label(L)->
    ( LastLabel = []-> LastLabelX= Cmd ; LastLabel= Cmd , LastLabelX= Cmd ) ,
    ( Cmds = []-> CmdsZ= [Cmd] ; goto_opt(Cmds, LastOp, LastLabelX, CmdsZ) )
  ;
  Cmd = (goto Target)->
    ( (LastLabel = label(L) , L == Target)-> Loop= true ; Loop= false ) ,
    ( LastLabel = label(L) -> Target= L ; true ) ,
    ((is_jmp_op(LastOp) , Loop = false )->
       goto_opt(Cmds, LastOp, [], CmdsZ)  %goto eliminated.
     ; ( Loop = true-> %Need to retain label.
	   CmdsZ= [LastLabel, Cmd|CmdsX] 
	 ; CmdsZ= [Cmd|CmdsX]
       )
    ) ,
    goto_opt(Cmds, Cmd, [], CmdsX)
  ; %Cmd is something besides a goto or a label.
    ( LastLabel = label(L)->
	CmdsZ= [LastLabel, Cmd|CmdsX] ; 
	CmdsZ= [Cmd|CmdsX]
    ) ,
    goto_opt(Cmds, Cmd, [], CmdsX).

is_jmp_op(goto _).
is_jmp_op(if _).
is_jmp_op(return _).


/*			OUTPUT INTERNAL FORM.				*/

out_prg(Prg):-
  write('{') , nl , write('  ') , out_prg_lo(Prg) , write('} .') , nl.

out_prg_lo([]).
out_prg_lo([Cmd|Cmds]):-
  Cmd = label(L) ->
    write(L) , write(': ') , 
    ( Cmds = [] -> write('goto ') , write(L) , nl ; out_prg_lo(Cmds) ) 
  ;
  Cmds = [] -> 
    write(Cmd) , nl 
  ;
    write(Cmd) , write(';') , nl , write('  ') , out_prg_lo(Cmds).


/*				RUN.					*/
%run(Cmds, Store, Prg, Z): Z is the result of interpreting tail Cmds of
%flow-chart program Prg given store Store.
run([Cmd|Cmds], Store, Prg, Z):-
 ( fc_trace(1)-> write(Cmd) , nl ; true ) ,
  run(Cmd, Cmds, Store, Prg, Z).


%run(Cmd, CmdsTail, Store, Prg, Z): Given store Store, Z is the
%result of interpreting command Cmd followed possibly (if Cmd is not a
%transfer) by tail CmdsTail of flow-chart program Prg.

run(goto(N), _CmdsTail, Store, Prg, Z):- 
  ! ,
  ( get_label(Prg, N, NCmds) ->
      run(NCmds, Store, Prg, Z)
    ; error(['Unknown label ', N, '.'])
  ).

run(label(_L), CmdsTail, Store, Prg, Z):-
  ! ,
  run(CmdsTail, Store, Prg, Z).

run(Var:= Exp, CmdsTail, Store, Prg, Z):-
  ! ,
  eval(Exp, Store, ExpVal) , store_update(Store, Var, ExpVal, Store1) ,
  run(CmdsTail, Store1, Prg, Z).

run(if E then goto M else goto N, _, Store, Prg, Z):-
  ! ,
  eval(E, Store, EVal) ,
  ( EVal = 0 ->
      get_label(Prg, N, ElseCmds) , run(ElseCmds, Store, Prg, Z)
    ; get_label(Prg, M, ThenCmds) , run(ThenCmds, Store, Prg, Z)
  ).
	
run(return(E), _CmdsTail, Store, _Prg, Z):-
  ! ,
  eval(E, Store, Z).

run(X, _CmdsTail, _Store, _Prg, _Z):-
  error(['Invalid statement ', X, '.']).


/*			EXPRESSION EVALUATION.				*/	

%eval(E, Store, Z): Z is the result of evaluating expression E given
%store Store.

eval(E, Store, Z):-
  integer(E)->
    Z= E
  ;
  atom(E)->
    store_lookup(E, Store, Z)
  ;
  ( functor(E, F, 2) , is_member(F, [<, =<, >, >=]) ) -> %Relational op.
      arg(1, E, A1) , eval(A1, Store, A1Z) , check_num(A1Z) ,
      arg(2, E, A2) , eval(A2, Store, A2Z) , check_num(A2Z) ,
      T =.. [F, A1Z, A2Z] , 
      (T-> Z= 1 ; Z= 0)
  ;
  ( functor(E, F, 2) , is_member(F, [\=, ==, \==]) ) -> 
      arg(1, E, A1) , eval(A1, Store, A1Z) ,
      arg(2, E, A2) , eval(A2, Store, A2Z) ,
      T =.. [F, A1Z, A2Z] , 
      (T-> Z= 1 ; Z= 0)
  ;
  E = (E1 = E2) ->
    eval(E1, Store, E1Z) , eval(E2, Store, E2Z) ,
    ( \+ \+ (E1Z = E2Z) -> Z= 1 ; Z= 0 ) %Do not bind any vars.
  ;
  E = ~T ->	%Quoted term.
    Z= T
  ;
  %Call Prolog relation with result in last argument.
  functor(E, F, N) , N1 is N + 1 , functor(E1, F, N1) ,	arg(N1, E1, Z) ,
  eval_args(1, N, E, Store, E1) ,
  ( E1-> true ; error(['Could not evaluate ', E, '.']) ).


eval_args(I, N, E, Store, EZ):-
  I =< N ->
    arg(I, E, A) , arg(I, EZ, AZ) , eval(A, Store, AZ) ,
    I1 is I + 1 , eval_args(I1, N, E, Store, EZ)
  ; true.


/*		   SOME PROLOG PREDICATES FOR EVALUATION. 		*/

+(X, Y, Z):-
  check_num(X) , check_num(Y) , Z is X + Y.

-(X, Y, Z):-
  check_num(X) , check_num(Y) , Z is X - Y.

hd([X|_], X).
tl([_|X], X).
tl([], []).
cons(X, Y, [X|Y]).

check_num(N):-
  number(N)->
    true
  ; error([N, ' is not a number.']).

error(MsgList):-
  error_lo(MsgList) ,
  trace , dummy(_).  %Turn on tracing to find out what went wrong.

error_lo([]):-
  nl.
error_lo([T|Ts]):-
  write(T) , error_lo(Ts).


dummy(_).


/*				STORE.					*/

/*
The store is maintained as two parallel lists of Names and Values, with
Names sorted using @<.
*/

%store_init(Store): Store is initialized to an empty store.
store_init(store([], [])).

%store_lookup(Name, Store, NameVal): NameVal is the value of Name in Store.
store_lookup(Name, store(Names, Vals), NameVal):-
  store_lookup(Name, Names, Vals, NameVal).

store_lookup(Name, [], _, _):-
  error(['No value for variable ', Name, ' found in store.']).

store_lookup(Name, [Name1|Names], [NameVal1|NameVals], NameVal):-
  Name = Name1 ->
    NameVal= NameVal1
  ;
  Name @> Name1 ->
    store_lookup(Name, Names, NameVals, NameVal)
  ; fail.

%store_update(Store, Var, Val, StoreZ): StoreZ is the result of updating
%the value of Var to Val in store Store.  The first component of store is
%maintained in a sorted (@<) order.
store_update(store(Names, Vals), Var, Val, store(NamesZ, ValsZ)):-
  store_update(Names, Vals, Var, Val, NamesZ, ValsZ).

store_update([], [], Var, Val, [Var], [Val]).
store_update(NameList, ValList, Var, VarVal, NamesZ, ValsZ):-
  NameList= [Name|Names] , ValList= [Val|Vals] ,
  ( Name = Var ->
      NamesZ= [Name|Names] , ValsZ= [VarVal|Vals]
    ;
    Var @< Name ->
      NamesZ= [Var|NameList] , ValsZ= [VarVal|ValList]
    ; NamesZ= [Name|NamesX] , ValsZ= [Val|ValsX] ,
      store_update(Names, Vals, Var, VarVal, NamesX, ValsX)
  ).

store_add([], [], Store, Store).
store_add([Name|Names], [Val|Vals], Store, StoreZ):-
  store_update(Store, Name, Val, StoreX) ,
  store_add(Names, Vals, StoreX, StoreZ).


/*				LABELS.					*/

%Bind variables used as labels to labnn terms.  This ensures that labels
%don't get accidentally bound during interpretation or program manipulation.
make_labels([]).
make_labels([C|Cs]):-
  ( C = label(L) -> gen_label(L) ; true ) ,
  make_labels(Cs).

%get_label(Prg, Label, LabelledSec): LabelledSec is the section of program
%Prg starting with label Label. 
get_label(Prg, Label, LabelledSec):-
  (Prg = [label(L)|RestPrg] , Label == L) ->
    LabelledSec= RestPrg
  ; Prg= [_|PrgTail] , get_label(PrgTail, Label, LabelledSec).

/* Could use swipl's gensym, but not all Prologs provide them. */
%SWIPL's database (recorded and friends) seems flaky.  Hence use assert
%and friends.

%Uncomment the next line if your Prolog requires dynamic 
%predicates to be declared.
%:-dynamic label_num/1.
label_num(0).

%clear_labels: Call when program starts to start generating labels at 0.
clear_labels:-
  retract(label_num(_)) , asserta(label_num(0)).

%gen_label(L): L is a new label of the form 'labN' where N is an integer.
gen_label(L):-
  retract(label_num(LNum)) ,
  LNum1 is LNum + 1 , asserta(label_num(LNum1)) ,
  name(LNum, LNumName) , conc("lab", LNumName, LName) , name(L, LName).

/*				TRACE.					*/

/* 

Print out each statement as it is executed.  Controlled by
set/reset_fc_trace.  Of limited utility.

*/

fc_trace(0).

set_fc_trace:-
  repeat , \+ retract(fc_trace(_)) , assert(fc_trace(1)).

reset_fc_trace:-
  repeat , \+ retract(fc_trace(_)) , assert(fc_trace(0)).

