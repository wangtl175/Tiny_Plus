# The Tiny Language Definition

Here is the definition for the Tiny language.

## The lexicon of the Tiny language is defined as follows:

- Keywords: **` WRITE READ IF ELSE RETURN BEGIN END MAIN STRING INT REAL WHILE REPEAT UNTIL AND OR`**

- Single-character separators: ` ; ,` `(`  `)`

- Single-character operators:   `+` `-` `*`  `/`

- Multi-character operators:  `:=` `==` `!=` `<=` `>=` `<` `>`

- Identifier: An identifier consists of a letter followed by any number of letters or digits. The following are examples of identifiers: x, x2, xx2, x2x, End, END2.Note that **End** is an identifier while **END** is a keyword. The following are not identifiers:

    - `IF, WRITE, READ, ...`. (keywords are not counted as identifiers)
    - `2x `(identifier can not start with a digit)
    - Strings in comments are not identifiers.

- Number is a sequence of digits, or a sequence of digits followed by a dot, and followed by digits.  

    ```
    Number -> Digits | Digits '.' Digits
    Digits -> Digit | Digit Digits
    Digit  -> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    ```

- Comments: string between /\*\* and \*\*/. Comments can be longer than one line. 

    > 扩展了关键词：WHILE REPEAT UNTIL AND OR
    >
    > 扩展了多元运算符：<= >= < >
    >
    > 扩展了运算符：%
    >
    > 字符串位于 ""中

## The EBNF Grammar

#### High-level program structures

```
Program -> MethodDecl MethodDecl* 
Type -> INT | REAL |STRING 
MethodDecl -> Type Id '(' FormalParams ')' Block | Type MAIN '(' FormalParams ')' Block
FormalParams -> FormalParam ( ',' FormalParam )* | e
FormalParam -> Type Id
```

**e代表空串**

要确保只有一个MAIN

#### Statements

```
Block -> BEGIN Statement* END

Statement -> Block
           | LocalVarDecl  
           | AssignStmt   
           | ReturnStmt
           | IfStmt
     	   | WriteStmt
      	   | ReadStmt
      	   | WhileStmt
      	   | RepeatStmt
                
LocalVarDecl -> Type Id ';' | Type AssignStmt  
        
AssignStmt -> Id := Expression ';'

ReturnStmt -> RETURN Expression ';'
IfStmt    -> IF '(' BoolExpression ')' Statement
           | IF '(' BoolExpression ')' Statement ELSE Statement
WriteStmt -> WRITE '(' Expression ',' String ')' ';'
ReadStmt  -> READ '(' Id ',' String ')' ';'


WhileStmt -> WHILE '(' BoolExpression ')' Statement
RepeatStmt -> REPEAT Statement UNTIL '(' BoolExpression ')'
```

#### Expressions

```
Expression -> ArithmeticExpression | BoolExpression | StringExpression


ArithmeticExpression -> MultiplicativeExpr  (( '+' | '-' ) MultiplicativeExpr)*
MultiplicativeExpr -> (PrimaryExpr | Num | Id) (( '*' | '/' ) (PrimaryExpr | Num | Id) )*

PrimaryExpr -> '(' ArthmeticExpression ')'
             | Id '(' ActualParams ')'
             | Id '(' ')'
ActualParams -> Expression ( ',' Expression)*

BoolExpression -> BoolTerm (OR BoolTerm)*
BoolTerm -> BoolFactor (AND BoolFactor)*
BoolFactor -> ArithmeticExpression ( ('<' | '>' | '<=' | '>=' | '==')
              ArithmeticExpression )
            | StringExpression ( ('<' | '>' | '<=' | '>=' | '==') StringExpression )

StringExpression -> string
```

![30892EA765B1A77BA624F3F25963D3D1](The%20Tiny%20Plus%20Language%20Definition.assets/30892EA765B1A77BA624F3F25963D3D1.png)

在BoolExpression的分析函数中，需要根据BoolFactor的分析函数判断这个是什么表达式，即不一定就是BoolExpression

## Sample program

```
/** this is a comment line in the sample program **/
INT f2(INT x, INT y ) 
BEGIN 
	INT z;
	z := x*x - y*y;
	RETURN z; 
END 
INT MAIN f1() 
BEGIN
	INT x;
	READ(x, "A41.input");
	INT y;
	READ(y, "A42.input");
	INT z;
	z := f2(x,y) + f2(y,x);
	WRITE (z, "A4.output"); 
END
```

