--TEST--
Basic test to ensure that annotation type hints with evaled-functions works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

$function_string = '
/**
 * This adds 2 numbers, radical
 * @param int $x The coolest thing ever
 * @param int $y An awesome integer
 * @return int
 */
function addr($x, $y)
{
	return $x + $y;
}
';
eval($function_string);
echo addr(2, 3);
echo "\n";
echo addr(3, 1000);
echo "\n";
echo addr('3', 1000);
echo "\n";
?>
--EXPECTREGEX--
5
1003

Fatal error: Wrong type encountered for argument 1 of function addr, was expecting a integer but got a \(string\) '3'
.*
