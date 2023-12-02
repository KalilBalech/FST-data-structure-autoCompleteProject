program Create Minimal Transduer for Given List ( input, output);
 var MinimalTransduerStates
 Ditionary : DICTIONARY;
 TempStates : array [0..MAX WORD SIZE℄ of STATE;
 InitialState : STATE;
 PreviousWord, CurrentWord, CurrentOutput,

WordSuÆx, CommonPrex : string;

 tempString : string;
 tempSet : set of string;
 i, j, PrexLengthPlus1 : integer;

10
:
har;
11 funtion FindMinimized ( s : STATE) : STATE;
12 freturns an equivalent state from the ditionary. If not present {

inserts a
opy of the parameter to the di

tionary and returns it.g

13 var r : STATE:
14 begin
15 r := MEMBER(MinimalTransdu

erStatesDi
tionary,s);

16 if r = NULL then begin
17 r := COPY STATE(s);
18 INSERT(r);
19 end;
20 return(r);
21 end; fFindMinimizedg
22 begin

23 MinimalTransdu

erStatesDi

tionary := NEW DICTIONARY;

24 for i := 0 to MAX WORD SIZE do
25 TempState[i℄ := NEW STATE;
26 PreviousWord := '';
27 CLEAR STATE(TempState[0℄);
28 while not eof(input) do begin
29 fLoop for the words in the input listg
30 read ln(input,CurrentWord,CurrentOutput);
31 f the fol lowing loop
al
ulates the length of the longest
ommon

pre
x of CurrentWord and PreviousWord g

32 i := 1;
33 while (i<length(CurrentWord)) and (i<length(PreviousWord))

and (PreviousWord[i℄ = CurrentWord[i℄) do

34 i := i+1;
35 Pre

xLengthPlus1 := i;

36 fwe minimize the states from the suÆx of the previous word g
37 for i := length(PreviousWord) downto Pre

xLengthPlus1 do
38 SET TRANSITION(TempStates[i-1℄,PreviousWord[i℄,

FindMinimized(TempStates[i℄));
39 fThis loop initializes the tail states for the

urrent wordg

40 for i := Pre

xLengthPlus1 to length(CurrentWord) do begin

41 CLEAR STATE(TempStates[i℄);
42 SET TRANSITION(TempStates[i-1℄,CurrentWord[i℄,

TempStates[i℄);

43 end;
44 if CurrentWords <> PreviousWord then begin
45 SET FINAL(TempStates[length(CurrentWord)℄, true);
46 SET OUTPUT(TempStates[length(CurrentWord)℄, f"g);
47 end;
48 for j := 1 to Pre

xLengthPlus1-1 do begin

49 CommonPre

x := OUTPUT(TempStates[j-1℄, CurrentWord[j℄)
^ CurrentOutput;
50 WordSuÆx := CommonPre
x
1
OUTPUT(TempStates[j-1℄,

CurrentWord[j℄);

51 SET OUTPUT(TempStates[j-1℄, CurrentWord[j℄,

CommonPre
x);

52 for

:= FIRST CHAR to LAST CHAR do begin

53 if TRANSITION(TempStates[j℄,

) <> NULL then

54 SET OUTPUT(TempStates[j℄,
,
on
at(WordSuÆx,

OUTPUT(TempStates[j℄,
)));

55 end;
56 if FINAL(TempStates[j℄) then begin
57 tempSet := ;;
58 for tempString in STATE OUTPUT(TempStates[j℄) do
59 tempSet := tempSet [
on
at(WordSuÆx,tempString);

60 SET STATE OUTPUT(TempStates[j℄, tempSet);
61 end;
62 CurrentOutput := CommonPre
x
1
CurrentOutput;

63 end;
64 if CurrentWord = PreviousWord then
65 SET STATE OUTPUT(TempStates[length(CurrentWord)℄,
STATE OUTPUT(TempStates[length(CurrentWord)℄)
[ CurrentOutput);
66 else SET OUTPUT(TempStates[Pre

xLengthPlus1-1℄,

CurrentWord[Pre

xLengthPlus1℄,CurrentOutput);

67 PreviousWord := CurrentWord;
68 end; fwhileg
69 f here we are minimizing the states of the last word g
70 for i := length(CurrentWord) downto 1 do
71 SET TRANSITION(TempStates[i-1℄,PreviousWord[i℄,

FindMinimized(TempStates[i℄));
72 InitialState := FindMinimized(TempStates[0℄);
73 PRINT TRANSDUCER(output,InitialState);
74 end.