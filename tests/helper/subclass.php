<?php

require_once "tests/helper/superclass.php";
class FooSub extends Foo {
	/**
	 * @param int
	 * @return int
	 */
	public function childMethod($a) {
		if (is_int($a)) {
			return $a + 42;
		}
		return 42;
	}
}
