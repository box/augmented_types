--TEST--
Static functions get void return value
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

class Someclass
{
	private static $foo = null;

	/**
	* @return void
	*/
	public static function init()
	{
		// nada
		static::$foo = self::returns_a_value();
	}

	/**
	 * @return array
	 */
	private static function returns_a_value()
	{
		return [];
	}
}

Someclass::init();
echo "hi!";

?>
--EXPECT--
hi!