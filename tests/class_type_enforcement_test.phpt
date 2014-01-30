--TEST--
Test to ensure class type annotations work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
class A {}
class B extends A {}

/**
 * @param A $x
 * @param A $y
 * @param B $z
 * @return void
 */
function doStuff($x, $y, $z)
{
	echo "Hello\n";
}

$a = new A();
$b = new B();

doStuff($a, $a, $b);
doStuff($b, $b, $b);

doStuff($a, $a, $a);

?>
--EXPECTREGEX--
Hello
Hello

Fatal error: Wrong type encountered for argument 3 of function doStuff, was expecting a B but got a A
.*

