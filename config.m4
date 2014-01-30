PHP_ARG_ENABLE(augmented_types, whether to enable augmented_types support,
[  --enable-augmented_types           Enable augmented_types support], yes)

if test "$PHP_AUGMENTED_TYPES" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_SUBST(AUGMENTED_TYPES_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, AUGMENTED_TYPES_SHARED_LIBADD)
  AC_DEFINE(HAVE_AUGMENTED_TYPES, 1, [Whether you have Augmented Types])
  PHP_NEW_EXTENSION(augmented_types,
	augmented_types.cc \
	augmented_types_module.cc \
	augmented_util.cc \
	PHPDoc_Bison_Compiler.cc \
	augmented_annotation_compiler.cc,
	shared,,,,yes)

	PHP_ADD_LIBRARY_WITH_PATH(phpdoc, $ext_srcdir/phpdoc/, AUGMENTED_TYPES_SHARED_LIBADD)
	PHP_ADD_MAKEFILE_FRAGMENT(Makefile-parser-targets.frag)

	if test `uname` == "Darwin"; then
		INCLUDES="$INCLUDES -ll -Wall"
	else
		INCLUDES="$INCLUDES -lfl -Wall"
	fi
fi

