--TEST--
Test to ensure that running a lot of phpcode with jump statements works
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
require_once 'stress/bench.php'
?>
--EXPECT--
starting stress test!
success!
