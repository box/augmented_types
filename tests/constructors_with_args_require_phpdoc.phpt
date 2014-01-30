--TEST--
Ensure that functions without phpdoc trigger errors
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

class A
{
	public function __construct($foo = "asdf")
	{
		echo "hello world!";
	}
}

$a = new A();

?>
--EXPECTREGEX--
.*No phpdoc specified - all constructors with arguments require phpdoc.*
