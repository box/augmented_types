--TEST--
Test that classes with magic __call and __callStatic are partly intercepted
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

class Magical {
	
	/**
	* @param string $name of the method
	* @param mixed[] $args to the function
	* @return string
	*/
	public function __call($name, $args) {
		return $name;
	}

	/**
	* @param string $name of the method
	* @param mixed[] 
	* @return int
	*/
	public static function __callStatic($name, $args) {
		
	}

	/**
	* @param string $name of the field to set
	* @param mixed $value the value to set
	* @return null magic setters will automatically
	* return null instead of a void pointer.
	*/
	public function __set($name, $value) {
		echo "setting $name with $value\n";
	}
}

$magical = new Magical();
echo $magical->foo() . " called\n";
echo $magical->bar(5) . " called\n";

$magical->x = 1;
$magical->y = 2;

?>
--EXPECT--
foo called
bar called
setting x with 1
setting y with 2
