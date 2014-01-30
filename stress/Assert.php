<?php

/**
* Prototype assertions
*/
class Assert
{
	/** @var Assert[] */
	private static $assert_arr = array();

	/** @var null|string */
	private $exception_class = null;

	/** @var array */
	private $exception_args;

	/**
	 * @static
	 * @return Assert
	 */
	public static function argument()
	{
		return self::with('InvalidArgumentException');
	}

	/**
	 * @static
	 * @return Assert
	 */
	public static function received_value()
	{
		return self::with('UnexpectedValueException');
	}

	/**
	 * @static
	 * @return Assert
	 */
	public static function logic()
	{
		return self::with('LogicException');
	}

	/**
	 * @static
	 * @return Assert
	 */
	public static function db_consistency()
	{
		return self::with('Database_Corruption_Exception');
	}

	/**
	 * @static
	 * @param DB_Model $rejected_object
	 * @param closure $whitelist_function
	 * @param bool $is_original
	 * @return Assert
	 */
	public static function whitelist($rejected_object, $whitelist_function, $is_original)
	{
		return self::with('Whitelist_Exception', array($rejected_object, $whitelist_function, $is_original));
	}

	/**
	 * @static
	 * @param string $exception_name
	 * @param array|void $args
	 * @return Assert
	 */
	public static function with($exception_name, $args = array())
	{
		if (!array_key_exists($exception_name, self::$assert_arr) || count($args) !== 0)
		{
			$Assert = new Assert($exception_name, $args);
			self::$assert_arr[$exception_name] = $Assert;
		}
		return self::$assert_arr[$exception_name];
	}

	/**
	 * @param string $exception_name the name of the exception class to use.
	 * @param array $args
	 * @throws InvalidArgumentException if the classname refers to an incorrect class.
	 * @return void
	 */
	private function __construct($exception_name, $args)
	{
		if (!is_subclass_of($exception_name, 'Exception'))
		{
			throw new InvalidArgumentException(sprintf("%s is not a valid subclass of Exception", $exception_name));
		}
		$this->exception_class = $exception_name;
		$this->exception_args = $args;
	}

	/**
	* @param string the message to use as input
	* @param *mixed|null arguments to the message
	* @return string
	*/
	private function format_message($message/*, $arg1, $arg2, ... $argN */)
	{
		$args = func_get_args();
		$formatted_args = array();
		array_shift($args);
		foreach ($args as $argument)
		{
			if ($argument === null)
			{
				$formatted_args[] = '<null>';
			}
			else if ($argument === false)
			{
				$formatted_args[] = '<false>';
			}
			else
			{
				$formatted_args[] = print_r($argument, true);
			}
		}
		array_unshift($formatted_args, $message);
		return call_user_func_array('sprintf', $formatted_args);
	}

	/**
	 * @param unknown $generated_message
	 * @param unknown $message
	 * @throws mixed exception type, specified in the constructor to the Assert instance.
	 * @return void
	 */
	private function throw_exception($generated_message, $message)
	{
		$cls = $this->exception_class;
		$full_message = "$message\n$generated_message";
		$arguments = array($full_message);
		$arguments = array_merge($arguments, $this->exception_args);
		$reflection = new ReflectionClass($cls);
		/** @var $exception Exception */
		$exception = $reflection->newInstanceArgs($arguments);
		throw $exception;
	}

	/**
	 * @param boolean $statement
	 * @param string|void $message
	 * @return Assert
	 */
	public function is_true($statement, $message="")
	{
		if ($statement !== true)
		{
			$this->throw_exception($this->format_message("The statement %s was not true", $statement), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $statement
	 * @param string $message
	 * @return Assert
	 */
	public function is_truthy($statement, $message="")
	{
		if (!$statement)
		{
			$this->throw_exception($this->format_message("The value %s was not truthy", $statement), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $statement
	 * @param string $message
	 * @return Assert
	 */
	public function is_false($statement, $message="")
	{
		if ($statement !== false)
		{
			$this->throw_exception($this->format_message("The value %s was not false (it is a %s)", $statement, gettype($statement)), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_numeric($thing, $message="")
	{
		if (!is_numeric($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not numeric but should have been", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_not_numeric($thing, $message="")
	{
		if (is_numeric($thing))
		{
			$this->throw_exception($this->format_message("The value %s is numeric but shouldn't have been", $thing), $message);
		}
		return $this;
	}

	/**
	* Raises an exception if the $thing's value is not numeric and is not equal to its value cast to an int. This
	* allows strings that represent integers but disallows strings that represent floats. As a consequence, it
	* disallows any representation of numbers in scientific notation where the exponent is != 0.
	*
	* @param unknown $thing mixed
	* @param unknown $message string message
	* @return Assert
	*/
	public function is_integery($thing, $message="")
	{
		if (!is_numeric($thing) || $thing != (int) $thing)
		{
			$this->throw_exception($this->format_message("The value %s does not represent an integer", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_integer($thing, $message="")
	{
		if (!is_integer($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not an integer (it is a %s)", $thing, gettype($thing)), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_float($thing, $message="")
	{
		if (!is_float($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not an float (it is a %s)", $thing, gettype($thing)), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_array($thing, $message="")
	{
		if (!is_array($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not an array", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_associative_array($thing, $message="")
	{
		if (!Box_Helper::is_associative_array($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not an associative array", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param mixed $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_scalar($thing, $message="")
	{
		if (!is_scalar($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not scalar", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_object($thing, $message="")
	{
		if (!is_object($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not an object", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_string($thing, $message="")
	{
		if (!is_string($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not a string (it is a %s)", $thing, gettype($thing)), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_boolean($thing, $message="")
	{
		if (!is_bool($thing))
		{
			$this->throw_exception($this->format_message("The value %s is not a boolean (it is a %s)", $thing, gettype($thing)), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_not_null($thing, $message="")
	{
		if (is_null($thing))
		{
			$this->throw_exception("The given value should not have been null", $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_not_empty($thing, $message="")
	{
		if (empty($thing))
		{
			$this->throw_exception("The given value should not have been empty", $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_empty($thing, $message="")
	{
		if (!empty($thing))
		{
			$this->throw_exception($this->format_message("The value %s should have been empty", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $thing
	 * @param string $message
	 * @return Assert
	 */
	public function is_callable($thing, $message="")
	{
		if (!is_callable($thing))
		{
			$this->throw_exception($this->format_message("The value %s should have been callable", $thing), $message);
		}
		return $this;
	}

	/**
	 * @param Array $allowed_values
	 * @param Mixed $actual - value to look for in the array
	 * @param string $message
	 * @return Assert
	 */
	public function is_in_array($allowed_values, $actual, $message="")
	{
		if (!in_array($actual, $allowed_values, true))
		{
			$this->throw_exception($this->format_message("The value %s is not one of the allowed values in %s", $actual, $allowed_values), $message);
		}
		return $this;
	}

	/**
	 * Asserts $array contains the key $key
	 *
	 * @param mixed $key Key to ensure exists
	 * @param array $array
	 * @param string $message
	 * @return Assert
	 */
	public function is_key_in_array($key, array $array, $message="")
	{
		if (!array_key_exists($key, $array))
		{
			$this->throw_exception($this->format_message("The key %s is not present in %s", $key, $array), $message);
		}
		return $this;
	}

	/**
	* @param unknown $expected - The expected value.
	* @param unknown $actual - The actual value. Note that if you are testing equality against an expected string type you
	* should ALWAYS use are_same() instead of are_equal() due to the bug-prone nature of comparison between
	* strings and integers.
	* @param string $message
	 * @return Assert
	*/
	public function are_equal($expected, $actual, $message="")
	{
		if ($expected != $actual)
		{
			$this->throw_exception($this->format_message("Expected %s, but it was %s", $expected, $actual), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $expected
	 * @param unknown $actual
	 * @param string $message
	 * @return Assert
	 */
	public function are_not_equal($expected, $actual, $message="")
	{
		if ($expected == $actual)
		{
			$this->throw_exception($this->format_message("both inputs were %s, but should have been different", $expected, $actual), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $expected
	 * @param unknown $actual
	 * @param string $message
	 * @return Assert
	 */
	public function are_same($expected, $actual, $message="")
	{
		if ($expected !== $actual)
		{
			$this->throw_exception($this->format_message("expected %s, but it was not identical to given %s", $expected, $actual), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $expected
	 * @param unknown $actual
	 * @param string $message
	 * @return Assert
	 */
	public function are_not_same($expected, $actual, $message="")
	{
		if ($expected === $actual)
		{
			$this->throw_exception($this->format_message("both inputs were %s, but should not have been identical", $expected, $actual), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $instance
	 * @param unknown $expected string name of class
	 * @param string $message
	 * @return Assert
	 */
	public function is_instance_of($instance, $expected, $message="")
	{
		if (is_null($instance) || (!is_a($instance, $expected) && !is_subclass_of($instance, $expected)))
		{
			$this->throw_exception($this->format_message("%s was supposed to be an instance of %s", $instance, $expected), $message);
		}
		return $this;
	}

	/**
	 * @param unknown $classname
	 * @param string $message
	 * @return Assert
	 */
	public function is_classname($classname, $message="")
	{
		$this->is_string($classname, $message);
		$this->is_true(class_exists($classname) || interface_exists($classname), $message);
		return $this;
	}

	/**
	 * Asserts that the current calling stack includes at least on of the specified classes.
	 *
	 * @param unknown $class_names
	 * @param string $message
	 */
	/*
	public function is_called_from($class_names, $message="")
	{
		if (!Box_Helper::is_called_from($class_names))
		{
			$this->throw_exception($this->format_message("%s was supposed to contain %s",
					Box_Helper::debug_backtrace_string(), $class_names), $message);
		}

		return $this;
	}
	*/

	/**
	 * @param unknown $expected
	 * @param unknown $actual
	 * @param string $message
	 * @return Assert
	 */
	public function uses_trait($object, $trait_name, $message="")
	{
		if (!Box_Helper::is_trait_used($object, $trait_name))
		{
			$this->throw_exception($this->format_message("The specified object does not use %s trait", $trait_name), $message);
		}

		return $this;
	}
}

class Box_Helper
{

	/**
	 * @static Returns whether $array is associative array or numeric array. This also means, for non-empty arrays, whether or not
	 * json_encode will encode using {} or [].
	 * http://stackoverflow.com/questions/173400/php-arrays-a-good-way-to-check-if-an-array-is-associative-or-numeric
	 * @param array $arr
	 * @return bool
	 */
	public static function is_associative_array(array $arr)
	{
		$keys = array_keys($arr);
		return ($keys !== array_keys($keys));
	}

	/**
	 * @param unknown $object to test
	 * @param string $trait_name
	 * @return bool
	 */
	public static function is_trait_used($object, $trait_name)
	{
		Assert::argument()	->is_object($object)
				->is_string($trait_name)
				->is_not_empty($trait_name);

		return array_key_exists($trait_name, class_uses($object));
	}
}
