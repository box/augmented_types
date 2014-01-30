--TEST--
Test to ensure that skipping compilation of a file then compiling another one enforces only things defined
in the second one thats compiled
--FILE--
<?php
echo augmented_types_whitelist(["tests/helper/"])."\n";
echo augmented_types_blacklist(["tests/helper/invalid2.php"])."\n";
require_once "tests/helper/invalid2.php"; // blacklisted
require_once "tests/helper/valid.php"; // whitelisted

echo valid(42)."\n";
echo invalid2('42')."\n";
echo valid('42')."\n";
?>
--EXPECTREGEX--
1
1
84
42

Fatal error.*
.*
