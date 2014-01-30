--TEST--
Test to ensure that argument type enforcing works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * @param int $x
 * @return int
 */
function inc_int($x)
{
	return $x + 42;
}

inc_int('2');
?>
--EXPECTREGEX--
.*Wrong type encountered for argument 1 of function inc_int, was expecting a integer but got a \(string\) '2'.*
