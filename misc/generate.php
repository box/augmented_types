<?php

// Generate a bunch of files for stress-testing the compiler.
// This will generate files of the form stress<number>.php containing
// a unique class and global function, numbered by <number>

$file_begin_to_func = <<<'EOD'
<?php

/**
 * This is a generated file for stress testing AT. See misc/generate.php for how to generate this.
 */

/**
 * @param int
 * @return int
 */
function globalFunc
EOD;

$func_body = <<<'EOD'
($int) {
	return 2*$int + 
EOD;

$end_func_body = <<<'EOD'
;
}
EOD;

for ($i = 1; $i < 3000; $i++) {
	$file_body = $file_begin_to_func.$i.$func_body.$i.$end_func_body;
	file_put_contents("tests/generated/function$i.php", $file_body);
}
