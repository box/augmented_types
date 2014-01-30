--TEST--
Test that closures do not expect valid phpdoc.
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

$fn = function() {
	echo "hi, I'm a closure!\n";
	return 10;
};
$fn();

/**
* @param string
* @param callable
* @return void
*/
function foo($s, $c) {
	echo $s."\n";
	$c();
}

$bar = "i will say to you ";

foo("hello world",
	/**
	 * IM A CLOSURE LOL @param @var @lolz
	 */
	function () use ($bar){
		echo $bar."goodnight moon\n";
	});

?>
--EXPECT--
hi, I'm a closure!
hello world
i will say to you goodnight moon
