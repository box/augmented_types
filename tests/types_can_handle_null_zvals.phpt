--TEST--
Test to ensure that basic types can handle enforcement of an optional param when no param is passed
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
/**
 * @param int|void
 * @return void
 */
function foo1($x = null)
{
	echo $x;
	echo "1\n";
}

/**
 * @param numeric|void
 * @return void
 */
function foo2($x = null)
{
	echo $x;
	echo "2\n";
}

/**
 * @param uint|void
 * @return void
 */
function foo3($x = null)
{
	echo $x;
	echo "3\n";
}

/**
 * @param mixed|void
 * @return void
 */
function foo4($x = null)
{
	echo $x;
	echo "4\n";
}

/**
 * @param stdObject|void
 * @return void
 */
function foo5($x = null)
{
	echo $x;
	echo "5\n";
}

foo1();
foo2();
foo3();
foo4();
foo5();

?>
--EXPECT--
1
2
3
4
5
