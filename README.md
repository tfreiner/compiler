# compiler

Simple c compiler that translates source code for a language (defined below) into an executable.  

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

The compiler is invoked as:  
`> testSem [file]` where 'file' is entered without the extension.  The extension must be 4280E02.  
testSem is the executable produced by the makefile.  
