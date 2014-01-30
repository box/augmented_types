--TEST--
Test that phpdoc cannot be declared multiple times for the same function.
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param int
* @return void
* 
* @param int
* @return void
*/
function go($i) {
	
}

?>
--EXPECTREGEX--
.*@param declarations may not appear after the @return declaration.*

