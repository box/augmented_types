--TEST--
Test that internal functions don't require valid phpdoc
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

$string = str_repeat("foo", 10);
if (strpos($string, "foof") !== false) {
	echo "oh hey, everything's cool\n";
}

$stack = new SplStack();

$stack->push("thereal");
$stack->push("thuper");

echo $stack->pop();
echo " ";
echo $stack->pop();

?>
--EXPECT--
oh hey, everything's cool
thuper thereal
