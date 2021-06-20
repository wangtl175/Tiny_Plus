# The Tiny Language Definition

Here is the definition for the Tiny language.

## The lexicon of the Tiny language is defined as follows:

- Keywords: **` WRITE READ IF ELSE RETURN BEGIN END MAIN STRING INT REAL WHILE REPEAT UNTIL AND OR`**

- Single-character separators: ` ; ,` `(`  `)`

- Single-character operators:   `+` `-` `*`  `/`

- Multi-character operators:  `:=` `==` `!=` `<=` `>=` `<` `>`

- Identifier: An identifier consists of a letter followed by any number of letters or digits. The following are examples
  of identifiers: x, x2, xx2, x2x, End, END2.Note that **End** is an identifier while **END** is a keyword. The
  following are not identifiers:

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

#### High-level run structures

```
Program -> MethodDecl MethodDecl* 
Type -> INT | REAL |STRING | BOOLEAN
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
            | (BoolExpression)

StringExpression -> string
```

![30892EA765B1A77BA624F3F25963D3D1](The%20Tiny%20Plus%20Language%20Definition.assets/30892EA765B1A77BA624F3F25963D3D1.png)

已经过时了，具体参考实验报告里

在BoolExpression的分析函数中，需要根据BoolFactor的分析函数判断这个是什么表达式，即不一定就是BoolExpression

实验报告是理论版本，这个是具体实施的版本

## Sample run

```
/** this is a comment line in the sample run **/
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

## 中间代码生成

### 控制流语句

```c++
if (BoolExp){
    A
}else{
    B
}
```

BoolExp有两个综合属性，分别是true和false出口，但是这两个出口的具体值要等BoolExp以及两个块A和B里的代码都生成之后才能确定。

所以控制流语在生成代码时，生成BoolExp的代码时，要记录true和false两个出口的代码，用一个独立的vector来保存BoolExp的生成代码，因为可能扩容导致迭代器失效

```c++
if (a<b){

}else{

}
```

翻译为

```
t=a<b
if t goto _  // 这是一个
goto _
```

所以，在控制流语句中，**无论多复杂的BoolExp，都只有最后两行代码还未生成**

在算术表达式中，也引入短路机制

```c++
a<b and c<d
```

翻译为

```
t1=a<b
if t goto l1
goto l2
l1: t2=c<d
t1=t2
l2:
```

结果是t1，当a<b换为TRUE或一个数字或字符串时，如何处理?

```
if TRUE goto l1
goto l2
l1: t2=c<d

l2:
```
结果时t2, 但是也不对

算术表达式引入短路机制难点在于

```
TRUE and TRUE
TRUE and a<b
FALSE and a<b
TRUE and FALSE
FALSE and TRUE
```

```
if TRUE goto l1
goto l2
l1: t=a<b
l2:
```

`a<b and c<d`

```
t1=a<b
if t1 goto l1
goto l2
l1: t2=c<d
res=t2
l2:
```

```c++
a<b or c<d
```

```
t1=a<b
if t1 goto l1
t2=c<d
if t2 goto 
goto 
```

没有全局变量，所以symtable都是局部的