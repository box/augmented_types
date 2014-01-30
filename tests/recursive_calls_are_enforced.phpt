--TEST--
Test to ensure that recursive calls to functions are enforced
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * @param int
 * @return int
 */
function fake_fib($i) {
	if (!$i) {
		return 0;
	}
	return fake_fib($i-1) + fake_fib(false);
}

echo fake_fib(2);
?>
--EXPECTREGEX--
Fatal error: Wrong type encountered for argument 1 of function fake_fib, was expecting a integer but got a \(bool\) false
.*in.*$
