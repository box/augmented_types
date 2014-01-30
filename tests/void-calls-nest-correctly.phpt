--TEST--
Void calls that nest under non-void calls correctly test void pointer 
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @return int
*/
function i_return_a_value()
{
	$x = i_also_return_a_value();
	i_dont_return_anything();
	return $x;
}

/**
* @return int
*/
function i_also_return_a_value()
{
	return 5;
}

/**
* @return void
*/
function i_dont_return_anything()
{
	//
}
echo i_return_a_value();
?>
--EXPECT--
5