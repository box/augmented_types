--TEST--
Test that 2d genericized arrays work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param mixed $thing
* @return int[][]
*/
function get_array($thing) {
	echo "success!\n";
	return $thing;
}

$a = get_array([[1,2,3]]);
$a = get_array([[1,2], [2]]);

$a = get_array([[1,2],2]);

?>
--EXPECTREGEX--
success!
success!
success!

Fatal error: Wrong type returned by function get_array, was expecting a \(\(integer\)\[\]\)\[\] but got a array
.*
