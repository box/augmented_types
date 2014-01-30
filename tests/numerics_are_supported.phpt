--TEST--
Test numeric types allow strings that have numeric value
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param numeric $param
* @return int|float
*/
function this_accepts_numerics($number) {
	return $number * 3;
}

echo this_accepts_numerics(5) . "\n";
echo this_accepts_numerics("3.3") . "\n";
echo this_accepts_numerics("0") . "\n";

this_accepts_numerics("foo");

?>
--EXPECTREGEX--
15
9.9
0

Fatal error: Wrong type encountered for argument 1 of function this_accepts_numerics, was expecting a numeric but got a \(string\) 'foo'
.*
