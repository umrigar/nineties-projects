/*

Prolog utility routines for mix.  The top-level routines are called as
functions from the mix interpreter --- the return value of the function is
the last argument of the corresponding Prolog relation.

*/

%Construct a extended program-point with label A and static store B.
cons_xpp(A, B, xpp(A, B, _Label)).

%Return the extended label associated with an extended program-point.
xlabel(xpp(_, _, L), label(L)).


%Return initial label of program.
initial_pp([_Read, label(L)|_], L).

%Return first basic-block in program.
init_bb([_Read, _Label|BB], BB).

%get_basic_block(L, Program, Z): Z is the basic-block starting after label L
%in Program.
get_basic_block(L, [C|Rest], Z):-
  ( C = label(L1) , L1 == L ) ->
    Z= Rest
  ; get_basic_block(L, Rest, Z).

%pending_pps(Program, Division, Ls): Ls is the list of labels of targets of
%gotos within dynamic ifs in Program.  Division is a list of those Program
%vars which are static.
pending_pps([_Read, label(L)|RestPrg], Division, Ls):-
  get_dynamic_if_targets(RestPrg, Division, [L], LsX) ,
  reverse(LsX, Ls).

%get_dynamic_if_targets(Program, Division, Ls, LsZ): List LsZ is list Ls
%extended with the new targets of gotos within dynamic ifs in Program.  The
%dynamic nature of each if is decided using Division which is a list of the
%static variables in Program.
get_dynamic_if_targets([], _Division, Ls, Ls).
get_dynamic_if_targets([C|Cs], Division, Ls, LsZ):-
  ( ( C = (if Exp then goto Then else goto Else) , 
      \+is_static(Exp, Division) 
    ) ->
      ( is_member(Then, Ls)-> LsX= Ls ; LsX= [Then|Ls] ) ,
      ( is_member(Else, Ls)-> LsY= LsX ; LsY= [Else|LsX] )
    ; LsY= Ls
  ) ,
  get_dynamic_if_targets(Cs, Division, LsY, LsZ).

%next_pp(PP, Program, PPs, PPZ): PPZ is that member of list PPs which is the
%next program-point after PP in Program.
next_pp(PP, [C|Cs], PendingPPs, PPZ):-
  C = label(PP) ->
    next_pp(Cs, PendingPPs, PPZ)
  ; next_pp(PP, Cs, PendingPPs, PPZ).

%next_pp(Program, PPs, PPZ): PPZ is the next program-point in Program
%which is a member of list PPs.
next_pp([C|Cs], PendingPPs, PPZ):-
  ( C = label(L) , is_member(L, PendingPPs) ) ->
    PPZ= L
  ; next_pp(Cs, PendingPPs, PPZ).

%init_code(Read, CodeZ): CodeZ is a difference list containing Read.
init_code(Read, [Read|Code]-Code).

%extend_code(Code, Command, CodeZ):  CodeZ is Code with Command appended.
extend_code(Code-[Command|Rest], Command, Code-Rest).

%complete_code(Code, CodeZ): CodeZ is Code wrapped up.
complete_code(Code - [], prg(Code)).

%read_filter(Program, Division, ReadZ): ReadZ is the read statement which
%is the first statement in Program with the static variables specified by
%list Division removed.
read_filter([read ReadVars|_], Division, read ReadVarsZ):-
  set_difference(ReadVars, Division, ReadVarsZ).


/* 		ACCESSORS & CONSTRUCTORS FOR fc CONSTRUCTS.		*/

cons_assgn(X, Exp, X:= Exp).
assgn_lhs(X:= _Exp, X).   assgn_rhs(_X:= Exp, Exp).


goto_label(goto L, L).

cons_if(Exp, xpp(_, _, ThenLabel), xpp(_, _, ElseLabel), 
	if Exp then goto ThenLabel else goto ElseLabel).
if_test(if Exp then goto _ else goto _, Exp).
then_label(if _Exp then goto ThenLabel else goto _ElseLabel, ThenLabel).
else_label(if _Exp then goto _ThenLabel else goto ElseLabel, ElseLabel).

cons_return(Exp, return Exp).
return_exp(return Exp, Exp).


/* 			XPP LIST MAINTAINENCE.				*/

%add_new_xpp(XPP, Pending, Marked, LiveVarsInfo, Z): Add XPP to Pending
%if it is not in Pending or Marked.  The result is Z.  LiveVarsInfo gives
%the live-variables at each program point used in deciding whether XPP is
%indeed new.
add_new_xpp(XPP, Pending, Marked, LiveVarsInfo, Z):-
  XPP = xpp(L, _, _NewLab) , is_member(L-LiveVars, LiveVarsInfo) ,
  ( ( xpp_member(XPP, Marked, LiveVars) 
      ; xpp_member(XPP, Pending, LiveVars) ) -> 
      Z= Pending 
    ; Z= [XPP|Pending]
  ).

%xpp_member(XPP, XPPs, LiveVars): XPP is a member of XPPs according to 
%the live-variable information specified by LiveVars.
xpp_member(XPP, [xpp(L0, store(Names0, Vals0), NewL0)|XPPs], LiveVars):-
  XPP = xpp(L, store(Names, Vals), NewL) ,
  ( ( L == L0 , equal_vals(LiveVars, Names, Vals, Names0, Vals0) ,
      NewL = NewL0 ) ->
      true
    ; xpp_member(XPP, XPPs, LiveVars)
  ).

%equal_vals(VarNames, Names1, Values1, Names2, Values2): The values in
%Values1 of the variables in VarNames are equal to the values in Values2
%of the variables in VarNames.  Names1 and Names2 gives the variable names
%corresponging to the values in Values1 and Values2.
equal_vals([], _, _, _, _).
equal_vals([V|Vs], N1, V1, N2, V2):-
  get_val(V, N1, V1, VVal1) , get_val(V, N2, V2, VVal2) ,
  VVal1 = VVal2 ,
  equal_vals(Vs, N1, V1, N2, V2).

%get_val(Name, Names, Vals, V): V is the value of Names in the store
%specified by the parallel lists Names and Vals.
get_val(V, [V|_], [VZ|_], VZ).
get_val(V, [N|Ns], [_|Vs], VZ):-
  V \= N , get_val(V, Ns, Vs, VZ).
  
%is_static(X, Division, Z): Z is 1 if expression X is static according to
%Division (0 otherwise).
is_static(X, Division, Z):-
  is_static(X, Division) ->  Z= 1 ; Z= 0.

%is_static(X, Division): Expression X is static according to Division.
is_static(X, Division):-
  var(X) ->
    error(['Internal error: Prolog var in expression.'])
  ;
  atom(X) -> 
    is_member(X, Division)
  ; 
  ( number(X) ; X = (~ _) )->
    true
  ;
  functor(X, _F, N), is_static_args(1, N, X, Division).

is_static_args(I, N, X, Division):-
  I =< N ->
    arg(I, X, A) , is_static(A, Division) ,
    I1 is I + 1 , is_static_args(I1, N, X, Division)
  ; true.

%Reduce(Exp, Store, Z): Z is the result of reducing expression as much as
%possible using the values of the variables in Store.
reduce(Exp, Store, Z):-
  Store = store(Names, _) ,
  ( Exp = (~ _) ->
      Z= Exp
    ;
    is_static(Exp, Names) ->
      eval(Exp, Store, X) , Z= ~ X
    ; functor(Exp, F, N) , functor(Z, F, N) ,
      reduce_args(1, N, Exp, Store, Z)
  ).

reduce_args(I, N, Exp, Store, Z):-
  I =< N ->
    arg(I, Exp, A) , arg(I, Z, AZ) , reduce(A, Store, AZ) ,
    I1 is I + 1 , reduce_args(I1, N, Exp, Store, Z)
  ; true.


/*		    LIVE-VARIABLE ANALYSIS.			*/

%get_live_vars(Program, Division, PendingPPs, LiveVars): LiveVars is a 
%list of terms of the form L-Vs for each L in list PendingPPs, where Vs is
%the list of static variables in Division which are live in Program at L.
get_live_vars(Program, Division, PendingPPs, LiveVars):-
  live_vars(Program, LiveVarsX) ,
  project_live_vars(LiveVarsX, PendingPPs, Division, LiveVars).


%live_vars(Program, LiveVars): LiveVars is a list of terms of the form 
%bb_info/6 (see below) specifying which variables are live at each label
%in the program.
live_vars([read _|Program], LiveVars):-
  bb_info(Program, BBInfo) ,
  iterate_live_vars(BBInfo, LiveVars).

%bb_info(Program, BBInfoZ):  Collect basic-block information.
%For each basic-block in the program (identified by its 
%entering label), compute bb_info(Label, Succs, Defs, Uses, In, Out), 
%where Label is the label identifying the basic-block, Succs is the labels of
%successive basic-blocks, Defs is the sorted list of vars defined in
%the bb, & Uses are the vars used in the bb, In/Out are []. BBInfoZ is a list 
%of such bb_info structs.
bb_info([label(L)|Prg], [bb_info(L, Succs, Defs, Uses, [], [])|BBInfoZ]):-
  bb_info(Prg, [], [], PrgX, Succs, Defs, Uses) ,
  bb_info(PrgX, BBInfoZ).
bb_info([], []).

bb_info([C|Cs], Defs, Uses, PrgZ, SuccsZ, DefsZ, UsesZ):-
  bb_info(C, Cs, Defs, Uses, PrgZ, SuccsZ, DefsZ, UsesZ). 	

bb_info(label(L), Cs, Defs, Uses, [label(L)|Cs], [L], Defs, Uses):-
  !.
bb_info(if Cond then goto Then else goto Else, Cs, Defs, Uses, 
	Cs, [Then, Else], Defs, UsesZ):-
  ! ,
  update_sets(Cond, Defs, Uses, UsesZ).
bb_info(goto L, Cs, Defs, Uses, Cs, [L], Defs, Uses):-
  !.
bb_info(return Exp, Cs, Defs, Uses, Cs, [], Defs, UsesZ):-
  ! ,
  update_sets(Exp, Defs, Uses, UsesZ).
bb_info(Var:= Exp, Cs, Defs, Uses, CsZ, SuccsZ, DefsZ, UsesZ):-
  update_sets(Exp, Defs, Uses, UsesX) ,
  update_sets(Var, UsesX, Defs, DefsX) ,
  bb_info(Cs, DefsX, UsesX, CsZ, SuccsZ, DefsZ, UsesZ).

%update_sets(Exp, A, B, BZ): BZ is list of atoms B extended by those atoms
%which are in Exp but not in B or list of atoms A.
update_sets(Exp, A, B, BZ):-
  atom(Exp)->
    ( ( is_member(Exp, A) ; is_member(Exp, B) )->
        BZ= B
      ; BZ= [Exp|B]
    )
  ;
  ( Exp = (~ _) ; number(Exp) ) ->
    BZ= B
  ; functor(Exp, _F, N) ,
    update_sets(1, N, Exp, A, B, BZ).

update_sets(I, N, Exp, A, B, BZ):-
  I =< N ->
    arg(I, Exp, Arg) , update_sets(Arg, A, B, BX) ,
    I1 is I + 1 , update_sets(I1, N, Exp, A, BX, BZ)
  ; BZ= B.

%Iteratively compute the in/out sets of the bb_info structs in BBInfo until
%none of the in sets change.
iterate_live_vars(BBInfo, LiveVars):-
  iterate_live_vars(BBInfo, BBInfo, [], false, LiveVars).

%iterate_live_vars(BBInfo, OldBBInfo, AccBBInfo, Change, LiveVars):
%LiveVars is the list of bb_info structs which result by repeatedly
%iterating the in/out set computation of the bb_info structs in BBInfo
%until Change is still false after a complete iteration.
iterate_live_vars([], _AllInfo, AccLiveVars, Change, LiveVars):-
  Change = false ->
    LiveVars= AccLiveVars
  ; iterate_live_vars(AccLiveVars, AccLiveVars, [], false, LiveVars).
iterate_live_vars([LiveVar|LiveVars], AllInfo, AccLiveVars, Change, 
		  LiveVarsZ):-
  bb_live_vars(LiveVar, AllInfo, LiveVarZ, ChangeX) ,
  combine_change(Change, ChangeX, ChangeY) ,
  iterate_live_vars(LiveVars, AllInfo, [LiveVarZ|AccLiveVars], ChangeY,
		    LiveVarsZ).

%bb_live_vars(BB, BBInfos, BBZ, Change): BBZ is the result of performing
%a single update of the in/out sets of bb_info struct BB using the current
%live-variable information in list of bb_info structs BBInfos.  Change is
%true if BB is different from BBZ, false otherwise.
%The computation is out(BB)= U in(BB1) where BB1 is a successor of BB.
%in(BB)= uses(BB) U (out(BB) - defs(BB)).
bb_live_vars(bb_info(L, Succs, Defs, Uses, In, _Out), BBInfo, 
	     bb_info(L, Succs, Defs, Uses, InZ, OutZ), Change):-
  bb_outs(Succs, BBInfo, [], OutZ) ,
  set_difference(OutZ, Defs, X) , set_union(Uses, X, InX) , sort(InX, InZ) ,
  ( InZ = In -> Change = false ; Change = true ).

%bb_outs(Ls, BBInfo, Outs, OutsZ): OutsZ is Outs unioned with the in-sets
%of the basic-blocks corresponding to labels Ls, as specified by BBInfo.
bb_outs([], _BBInfo, Outs, Outs).
bb_outs([L|Ls], BBInfo, Outs, OutsZ):-
  is_member(bb_info(L, _Succs, _Defs, _Uses, In, _Out), BBInfo) ,
  set_union(In, Outs, OutsX) ,
  bb_outs(Ls, BBInfo, OutsX, OutsZ).

%combine_change(ChangeX, ChangeY, ChangeZ): ChangeZ= ChangeX || ChangeY.
combine_change(ChangeX, ChangeY, ChangeZ):-
  ( ChangeX = true ; ChangeY = true ) ->
    ChangeZ= true 
  ; ChangeZ= false.

%project_live_vars(BBInfos, PPs, Div, LiveVars): LiveVars is a list of terms
%of the form L-Vars where for each L in PPs, where Vars are those static
%variables in list Div which are live at L according to the information
%contains in list BBInfos of bb_info structs.
project_live_vars([], _PPs, _Div, []).
project_live_vars([bb_info(L, _Succs, _Defs, _Uses, In, _Out)|LiveInfo],
		  PPs, Div, LiveVars):-
  ( is_member(L, PPs)->
      set_intersection(Div, In, ProjectedIns) , 
      LiveVars= [L-ProjectedIns|LiveVarsX]
    ; LiveVars= LiveVarsX
  ) , 
  project_live_vars(LiveInfo, PPs, Div, LiveVarsX).
  
/*			NORMALIZE					*/

/* 

Normalize fc programs so that they can be compared via unification.
Unify variables followed by ':' with labnn terms.  Replace labxx atoms with
canonical labnn.

*/

normalize(Prog, ProgZ):-
  replace_lab_terms(Prog, 1, N, [], _, ProgX) ,
  replace_var_labels(ProgX, N, _, ProgZ).

%Replace all labnn atoms in the program with labmm terms where mm is
%generated in a systematic way.
replace_lab_terms(Prog, I, N, ReplacedLabels, ReplacedLabelsZ, ProgZ):-
  atom(Prog) ->
    ( name(Prog, [0'l, 0'a, 0'b|_])->
        ( is_member(Prog-ProgZ, ReplacedLabels)->
	    N= I , ReplacedLabelsZ= ReplacedLabels
          ; name(I, IName) , conc("lab", IName, ProgZName) ,
            name(ProgZ, ProgZName) ,
	    N is I + 1 , ReplacedLabelsZ= [Prog-ProgZ|ReplacedLabels]
        )
      ; ProgZ= Prog , N= I , ReplacedLabelsZ= ReplacedLabels
    )
  ;
  ( number(Prog) ; var(Prog) )->
    ProgZ= Prog , N= I , ReplacedLabelsZ= ReplacedLabels
  ;
    functor(Prog, F, M) , functor(ProgZ, F, M) ,
    replace_lab_terms(1, M, Prog, I, N, ReplacedLabels, ReplacedLabelsZ, ProgZ).

replace_lab_terms(J, M, Prog, I, N, ReplacedLabels, ReplacedLabelsZ, ProgZ):-
  J =< M ->
    arg(J, Prog, Arg) , arg(J, ProgZ, ArgZ) ,
    replace_lab_terms(Arg, I, NX, ReplacedLabels, ReplacedLabelsX, ArgZ) ,
    J1 is J + 1 ,
    replace_lab_terms(J1, M, Prog, NX, N, ReplacedLabelsX, 
		      ReplacedLabelsZ, ProgZ)
  ;
    N= I , ReplacedLabelsZ= ReplacedLabels.

%Replace all Prolog variable labels with labnn terms where nn is generated
%in a systematic manner.
replace_var_labels(Prog, I, N, ProgZ):-
  ( atomic(Prog) ; var(Prog) ) ->
     N= I , ProgZ= Prog
  ;
  ( Prog= (L:T) , var(L) )->
      name(I, IName) , conc("lab", IName, LName) , name(L, LName) ,
      ProgZ= (L:TZ) , I1 is I + 1 , replace_var_labels(T, I1, N, TZ)
  ;
    functor(Prog, F, M) , functor(ProgZ, F, M) ,
    replace_var_labels(1, M, Prog, I, N, ProgZ).

replace_var_labels(J, M, Prog, I, N, ProgZ):-
  J =< M ->
    arg(J, Prog, Arg) , arg(J, ProgZ, ArgZ) ,
    replace_var_labels(Arg, I, NX, ArgZ) ,
    J1 is J + 1 , replace_var_labels(J1, M, Prog, NX, N, ProgZ)
  ; N= I.

%fc_compare(FCFile1, FCFile2): Compare FC programs in files FCFile1 & FCFile2.
fc_compare(FCFile1, FCFile2):-
  see(FCFile1) , read(FCPrg1) , seen , normalize(FCPrg1, NormFCPrg1) ,
  see(FCFile2) , read(FCPrg2) , seen , normalize(FCPrg2, NormFCPrg2) ,
  NormFCPrg1 = NormFCPrg2.
