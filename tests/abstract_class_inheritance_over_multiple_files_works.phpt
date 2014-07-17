--TEST--
Test to ensure that compilation works with class hierarchy inheritance
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
require_once "tests/helper/base.php";
require_once "tests/helper/sub1.php";
require_once "tests/helper/valid.php";

$o = new Bar();
var_dump( $o->inc_int(2) );
$o->barbar();
$o->inc_int("foo");

?>
--EXPECTREGEX--
hello
int\(44\)
barbarbar

Fatal error:.*
.*
