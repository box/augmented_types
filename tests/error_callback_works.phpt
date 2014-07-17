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
	$arg_info_str = $arg_num === -1 ? "the return value" : "argument $arg_num";
	echo "Got a type error from file $file_path at line number $line_number in function $function_name. ";
	echo "The expected type of $arg_info_str was $expected_type and the actual value was:\n";
	if ($is_void) {
		echo "(void)\n";
	} else {
		var_dump($offending_value);
	}
	echo "\n";
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

/**
 * @param string $in
 * @return int
 */
function impossible($in) {
	return $in;
}
$a = impossible(1);
$a = impossible("1");
$a = impossible(true);

/**
 * @param string|int|Whatever $in
 * @return bool
 */
function mixer($in = false) {
	echo "mixer!\n";
	echo "$in\n";
	if (!$in) {
		return $in;
	}
}
$a = mixer();
$b = mixer(1); // NULL will be filled in for the return value

/**
 * @return void
 */
function hodor() {
	echo "hodor!\n";
}
hodor("hodor");
?>
--EXPECTREGEX--
Got a type error from file .* at line number .* in function negate. The expected type of argument 1 was uint and the actual value was:
int\(-1\)

Got a type error from file .* at line number .* in function impossible. The expected type of argument 1 was string and the actual value was:
int\(1\)

Got a type error from file .* at line number .* in function impossible. The expected type of the return value was integer and the actual value was:
string\(1\) "1"

Got a type error from file .* at line number .* in function impossible. The expected type of argument 1 was string and the actual value was:
bool\(true\)

Got a type error from file .* at line number .* in function impossible. The expected type of the return value was integer and the actual value was:
bool\(true\)

Got a type error from file .* at line number .* in function mixer. The expected type of argument 1 was string\|integer\|Whatever and the actual value was:
\(void\)

mixer!

mixer!
1
Got a type error from file .* at line number .* in function mixer. The expected type of the return value was boolean and the actual value was:
NULL

Got a type error from file .* at line number .* in function hodor. The expected type of argument 1 was void and the actual value was:
string\(5\) "hodor"

hodor!
