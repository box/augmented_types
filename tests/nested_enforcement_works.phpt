--TEST--
All type-enforced op_ararys get cleaned up properly after an inner nested function fails typechecking
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * @param int $x
 * @param int $y
 * @return int
 */
function foo($x, $y)
{
	return bar($x) + $y;
}

/**
 * @param bool $z
 * @return int
 */
function bar($z)
{
	if ($z) {
		return 1;
	}
	return 0;
}

echo foo(1,2);
?>
--EXPECTREGEX--
Fatal error: Wrong type encountered for argument 1 of function bar, was expecting a boolean but got a \(int\) 1
 in .* on line \d*
