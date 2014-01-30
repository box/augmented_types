--TEST--
Test that variadic arguments cannot be followed by other arguments
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param *int
* @param *int
* @return void
*/
function go() {
	
}

go();

?>
--EXPECTREGEX--
.*variadic arguments cannot be followed by regular arguments.*

