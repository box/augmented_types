--TEST--
Test to ensure that constructors with no args are enforced
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
class Foo
{
	private $x = 0;

	public function __construct()
	{
		$this->x = 0;
	}
}

$o = new Foo("HELLO WORLD LAWL");

?>
--EXPECTREGEX--
Fatal error: Too many arguments provided for function __construct, was expecting nothing but got a.*
.*
