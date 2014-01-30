--TEST--
Ensure that functions without phpdoc trigger errors
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

class A
{
	public function __construct()
	{
		echo "hello world!";
	}
}

$a = new A();

?>
--EXPECT--
hello world!
