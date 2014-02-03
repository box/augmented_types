--TEST--
Ensure that arrayOf types that contain void are forbidden at compile-time
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param mixed $thing
* @param (int|string|void)[]|void
* @return int[][]
*/
function foo($bar, $baz) {
	echo "success!\n";
	return $baz;
}

?>
--EXPECTREGEX--
.*Error in type for argument 2: ArrayOf types cannot contain void!.*
