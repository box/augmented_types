--TEST--
Test ensures that @return disjunctions with void don't work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @return integer|void
*/
function sometimes_returns() {
	if (rand() < .5) {
		return 1;
	}
}


?>
--EXPECTREGEX--
.*@return declarations may not mix void and values.*
