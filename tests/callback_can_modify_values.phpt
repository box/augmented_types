--TEST--
Ensure that the augmented types error callback can modify values safely
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

class Helper
{
	/**
	 * @param mixed|null $offending_value
	 * @param bool $is_void
	 * @param string $expected_type
	 * @param string $function_name
	 * @param string $file_path
	 * @param int $line_number
	 * @param int $arg_num
	 * @return void
	 */
	public static function error_cb($offending_value, $is_void, $expected_type, $function_name, $file_path, $line_number, $arg_num)
	{
		$offending_value = null;
		$is_void = "foo";
		$expected_type = "asdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf";
		$function_name = "foobarbazwhatever_longfunctionname";

		echo $expected_type.$function_name.$is_void;
	}
}
augmented_types_register_type_error_callback("Helper::error_cb");

/**
* @param uint input
* @return integer output
*/
function negate($i) {
	return -$i;
}
negate(-1);

?>
--EXPECT--
asdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdffoobarbazwhatever_longfunctionnamefoo
