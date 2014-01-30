--TEST--
Abstract classes defined in the same file work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

abstract class TheAbstractThing
{
	/**
	* @param string
	* @return bool
	*/
	public abstract function foo($thing);
}

class TheRealThing extends TheAbstractThing
{
	/**
	* @param string $thing
	* @return bool
	*/
	public function foo($thing)
	{
		return true;
	}
}

$thing = new TheRealThing();
$thing->foo("go");
echo 'hi';

?>
--EXPECT--
hi
