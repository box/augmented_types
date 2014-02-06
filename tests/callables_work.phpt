--TEST--
Test that callables are supported
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

$fn = function() {
	return "closure works\n";
};

class Invokable {

	/**
	* @return string
	*/
	public function __invoke() {
		return "invokable works\n";
	}
}

class Targetable {
	/**
	* @return string
	*/
	public function go() {
		return "array callable syntax works\n";
	}
}

/**
* @return string
*/
function global_fn() {
	return "global functions work\n";
}

/**
* @param string
* @param callable|void
* @return string
*/
function accept_optional_callable($string, $fn = null) {
	return $string;
}

/**
* @param callable
* @return string
*/
function accept_callables($fn) {
	return $fn();
}

echo "starting\n";
echo accept_callables($fn);
echo accept_callables(new Invokable());
echo accept_callables([new Targetable(), "go"]);
echo accept_callables("global_fn");
echo accept_optional_callable("optional callables work\n");
echo accept_optional_callable("optional callables work\n", function() {});

accept_callables(["NotAClass", "hey"]);

?>
--EXPECTREGEX--
starting
closure works
invokable works
array callable syntax works
global functions work
optional callables work
optional callables work

(.*)Wrong type encountered for argument 1 of function accept_callables, was expecting a callable but got a array
(.*)
