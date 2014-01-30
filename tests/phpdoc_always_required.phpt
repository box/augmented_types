--TEST--
Ensure that functions without phpdoc trigger errors
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

function something() {

};

?>
--EXPECTREGEX--
.*No phpdoc specified - all named functions require phpdoc.*
