--TEST--
Basic test to ensure that a return type hint with a varname works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * This adds 2 numbers, radical
 * @param int $x The coolest thing ever
 * @param int $y An awesome integer
 * @return int $super_awesome_retval is super awesome
 */
function addr($x, $y)
{
	return $x + $y;
}

/**
 * Returns the solution
 * @return integer the solution
 */
function theSolution()
{
	return 42;
}
echo addr(2, 3);
echo "\n";
echo addr(3, 1000);
echo "\n";
echo theSolution();
?>
--EXPECT--
5
1003
42
