--TEST--
Test to ensure that void enforcement works when the non-extant retval is actually used
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @return void
 */
function init()
{
/*
	$app_conf = CFG::instance()->application_conf();
	Feature_Instance::$always_cookieable = $app_conf['features']['always_cookieable'];
	static::$features_cache = array();
	static::$features_tested = array();
	*/
	$_GET['123'] = [];
}
init();
echo init();
?>
--EXPECTREGEX--
