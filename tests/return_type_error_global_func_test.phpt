--TEST--
Test to ensure that return type enforcing works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * @param int $x
 * @return float
 */
function inc_int($x)
{
	return $x + 42;
}

$b = inc_int(2);
?>
--EXPECTREGEX--

Fatal error: Wrong type returned by function inc_int, was expecting a float but got a \(int\) 44
.*
