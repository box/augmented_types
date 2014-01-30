--TEST--
Test to ensure that calling a function with optional args works as expected
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * @param int $x
 * @param float|void $y
 * @param boolean|void $z
 * @return float
 */
function opt_addr($x, $y = 0.1, $z = true)
{
	if ($z) {
		return $x + $y + 1;
	}
	return $x + $y;
}

echo opt_addr(2)."\n";
echo opt_addr(2, 1.5)."\n";
echo opt_addr(2, 1.5, false)."\n";

echo opt_addr(2.1);
?>
--EXPECTREGEX--
3.1
4.5
3.5

Fatal error: Wrong type encountered for argument 1 of function opt_addr, was expecting a integer but got a \(float\) 2.1.*
.*
