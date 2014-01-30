--TEST--
Another test to ensure that a function with mixed values works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * This is cool
 * So cool it deserves a second line
 * @param float $x This is a super cool float
 * @param int $y
 * @param bool $z
 * @return float
 */
function mixed_type_adder($x, $y, $z)
{
	// thanks for letting us do this, php
	return $x + $y + $z;
}

echo mixed_type_adder(2.1, 3, true);
echo "\n";
echo mixed_type_adder(3.14159, 100, false);
echo "\n";
?>
--EXPECT--
6.1
103.14159
