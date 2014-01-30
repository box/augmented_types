--TEST--
Test to ensure constructors dont require return annotations
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
class A {

	/**
	 * @param int
	 */
	public function __construct($a)
	{
		echo "hello world! i got called with $a\n";
	}

	/**
	 * @return void
	 */
	public function foo()
	{
		echo "foobar\n";
	}
}

$a = new A(42);
$a->foo();
?>
--EXPECT--
hello world! i got called with 42
foobar

