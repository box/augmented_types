--TEST--
Basic test to ensure that annotation type hints with valid hints works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * This adds 2 numbers, horray
 * @param int $x The coolest thing ever
 * @param int $y An awesome integer
 * @return int
 */
function addr($x, $y)
{
	return $x + $y;
}
echo addr(2, 3);
echo "\n";
echo addr(3, 1000);
echo "\n";
?>
--EXPECT--
5
1003
