--TEST--
Test to ensure that whitelisting eval-ed code works
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
// this isn't whitelisted or blacklisted yet, so it shouldn't be compiled
eval($function_string);
echo addr(2, '3')."\n";

echo augmented_types_whitelist(["tests/helper/"])."\n";
require_once "tests/helper/eval_func.php"; // whitelist is longer

echo subtractor(5, 2)."\n";
echo subtractor(5, '2')."\n";
// TODO - what should be the desired behavior if the eval is in this file??????
// should we still enforce it???? eval sucks...
?>
--EXPECTREGEX--
5
1
3

Fatal error.*
.*
