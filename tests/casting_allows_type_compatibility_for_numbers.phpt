--TEST--
Test to show that userland casting fixes type incompatibilities
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param int
* @return void
*/
function i_accept_ints($i) {
	echo $i . "\n";
}

i_accept_ints((int) 1.0);

i_accept_ints(1.0);


?>
--EXPECTREGEX--
1

Fatal error: Wrong type encountered for argument 1 of function i_accept_ints, was expecting a integer but got a \(float\) 1.0.*
(.*)
