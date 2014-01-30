--TEST--
Ensure that unisgned ints work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param uint input
* @return integer output
*/
function negate($i) {
	return -$i;
}

echo negate(1) . "\n";
echo negate(0) . "\n";

negate(-1);

?>
--EXPECTREGEX--
-1
0

Fatal error: Wrong type encountered for argument 1 of function negate, was expecting a uint but got a \(int\) -1
.*
