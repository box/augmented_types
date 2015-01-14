--TEST--
Test to ensure that blacklisting works
--INI--
augmented_types.whitelist=./tests/helper/invalid2.php:.
augmented_types.blacklist=./tests/helper
--FILE--
<?php
require_once "tests/helper/invalid1.php"; // shouldn't be compiled
echo "HERE\n";

require_once "tests/helper/invalid2.php"; // should be compiled!
?>
--EXPECTREGEX--
HERE

Fatal error.*
.*
