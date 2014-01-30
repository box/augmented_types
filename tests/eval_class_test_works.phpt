--TEST--
Basic test to ensure that annotation type hints with evaled-functions works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

$class_string = '
class Foo
{
	private $x = 0;

	/**
	 * @param int $y
	 * @return int
	 */
	public function inc_int($y)
	{
		return $this->x + 42 + $y;
	}

	/**
	 * I DGAF about types
	 * @return int
	 */
	public function inc_whatever()
	{
		return $this->x + 10;
	}
}
';

eval($class_string);
$o = new Foo();
echo $o->inc_whatever()."\n";
echo $o->inc_int(1)."\n";

$o->inc_int('2');
?>
--EXPECTREGEX--
10
43

Fatal error: Wrong type encountered for argument 1 of function inc_int, was expecting a integer but got a \(string\) '2'
.*
