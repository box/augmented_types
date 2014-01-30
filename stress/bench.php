<?php


// begin benchmark
$iterations = 5;
//$iterations = 100000;
echo "starting stress test!\n";

require_once "Assert.php";
/**
 * Hello world
 * @param int $iterations a cool bench function with lots of function calls
 * @return array
 */
function is_true_bench_two_calls($iterations)
{
	$start = microtime(true);
	$a = true;

	for ($i = 0; $i < $iterations; $i++)
	{
		if(!(true === $a))
		{
			throw new InvalidArgumentException("unacceptable!");
		}
		if(!(true === true))
		{
			throw new UnexpectedValueException("unacceptable!");
		}
	}

	$end = microtime(true);
	$base_time = $end - $start;

	$start = microtime(true);

	for ($i = 0; $i < $iterations; $i++)
	{
		Assert::argument()->is_true(true);
		Assert::received_value()->is_true(true);
	}

	$end = microtime(true);
	$assert_time = $end - $start;

	$start = microtime(true);

	for ($i = 0; $i < $iterations; $i++)
	{
		Assert::argument()->is_true(true);
		Assert::received_value()->is_true(true);
	}

	$end = microtime(true);
	$assert_time = $end - $start;
	return [$base_time, $assert_time, $assert_time];
}

/**
 * Do absolutely nothing!
 * @param array $res_array something to do nothing with
 * @return void
 */
function do_nothing($res_array)
{
}

/**
 * @param int $iterations
 * @return array
 */
function run_is_true_with_four_chains($iterations)
{
	$start = microtime(true);

	for ($i = 0; $i < $iterations; $i++)
	{
		Assert::argument()->is_true(true)->is_true(true)->is_true(true)->is_true(true);
	}

	$end = microtime(true);
	$assert_time = $end - $start;

	$start = microtime(true);

	for ($i = 0; $i < $iterations; $i++)
	{
		Assert::argument()->is_true(true)->is_true(true)->is_true(true)->is_true(true);
	}

	$end = microtime(true);
	$assert_time = $end - $start;

	$start = microtime(true);

	for ($i = 0; $i < $iterations; $i++)
	{
		if(!(true === true))
		{
			throw new InvalidArgumentException("unacceptable!");
		}
		if(!(true === true))
		{
			throw new InvalidArgumentException("unacceptable!");
		}
		if(!(true === true))
		{
			throw new InvalidArgumentException("unacceptable!");
		}
		if(!(true === true))
		{
			throw new InvalidArgumentException("unacceptable!");
		}
	}

	$end = microtime(true);
	$base_time = $end - $start;
	return [$base_time, $assert_time, $assert_time];
}

$is_true_results = is_true_bench_two_calls($iterations);
do_nothing($is_true_results);

$is_true_four_results = run_is_true_with_four_chains($iterations);
do_nothing($is_true_four_results);

echo "success!\n";
