# compiler

Simple c compiler that translates source code for a language (defined below) into a .asm file.  

_note: project is still in progress_

## Language (in BNF)

`<program>`  ->  `<vars> <block>`  
`<block>`    ->  `BEGIN <vars> : <stats> END`  
`<vars>`     ->  `empty | VOID Identifier <mvars>`  
`<mvars>`    ->  `empty | Identifier <mvars>`  
`<expr>`     ->  `<M> + <expr> | <M>`  
`<M>`        ->  `<T> - <M> | <T>`  
`<T>`        ->  `<F> * <T> | <F> / <T> | <F>`  
`<F>`        ->  `- <F> | <R>`  
`<R>`        ->  `( <expr> ) | Identifier | Number`  
`<stats>`    ->  `<stat> :  <mStat>`  
`<mStat>`    ->  `empty | <stat> :  <mStat>`  
`<stat>`     ->  `<in> | <out> | <block> | <if> | <loop> | <assign>`  
`<in>`       ->  `INPUT Identifier`  
`<out>`      ->  `OUTPUT <expr>`  
`<if>`       ->  `IF ( <expr> <RO> <expr> ) <block>`  
`<loop>`     ->  `FOR ( <expr> <RO> <expr> ) <block>`  
`<assign>`   ->  `Identifier = <expr>` 
`<RO>`       ->  `>=> | <=< | >=>  = |  <=<  = | ! !  |  = =`

## Invocation

The compiler can be invoked in 3 ways:
  1) `> comp [file]` where 'file' is entered WITHOUT the extension.  
  2) `> comp < [file]` where 'file' is the entered WITH the extension.  
  3) `> comp` which will read input from the keyboard until EOF is simulated.  
     To simulate EOF, press enter and then Ctrl-D (on Mac/Linux) or Ctrl-Z (on Windows).  
     
Notes:
* testSem is the executable produced by the makefile.  
* The file extensions must be .4280E02.
