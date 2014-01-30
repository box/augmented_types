--TEST--
Ensure that functions with javascript style docs will have syntax errors on parse
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @param {int} 
 * @return {int}
 */
function doubleIt($value) {
	return $value * 2;
}

?>
--EXPECTREGEX--
.*unexpected OBRACE.*