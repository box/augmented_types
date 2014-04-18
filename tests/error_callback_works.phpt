--TEST--
Ensure that the augmented types error callback functionality works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

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
function error_cb($offending_value, $is_void, $expected_type, $function_name, $file_path, $line_number, $arg_num)
{
	$arg_info_str = $arg_num === -1 ? "return value" : "argument $arg_num";
	echo "Got a type error from file $file_path at line number $line_number in function $function_name. ";
	echo "The expected type of $arg_info_str was $expected_type and the actual value was:\n";
	if ($is_void) {
		echo "(void)";
	} else {
		var_dump($offending_value);
	}
}

augmented_types_register_type_error_callback("error_cb");

/**
* @param uint input
* @return integer output
*/
function negate($i) {
	return -$i;
}
negate(-1);

?>
--EXPECTREGEX--
Got a type error from file .* at line number .* in function negate. The expected type of argument 1 was uint and the actual value was:
int\(-1\)
