--TEST--
Test ensures that functions defined with disjunctive param and retval syntax work properly.
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param integer|string
* @return void
*/
function take_string_or_int($thing) {
	if (is_string($thing)) {
		echo "thing is string\n";
	}
	else {
		echo "thing is int\n";
	}
}

take_string_or_int("foo");
take_string_or_int(1);

/**
* @param integer|void
* @return void
*/
function ive_got_some_default_argz($a = 0){
	echo "ive got " . ($a + 1) . "\n";
}

ive_got_some_default_argz(1);
ive_got_some_default_argz();

/**
* @param int|string|bool $coolargbro
* @return void
*/
function cool($coolargbro) {
	echo 'cool '.$coolargbro."\n";
}

cool(123);
try {
	cool(123.123);
} catch (InvalidArgumentException $e) {
	echo $e->getMessage()."\n";
}

?>
--EXPECTREGEX--
thing is string
thing is int
ive got 2
ive got 1
cool 123

Fatal error: Wrong type encountered for argument 1 of function cool, was expecting a integer|string|boolean but got a float
.*

