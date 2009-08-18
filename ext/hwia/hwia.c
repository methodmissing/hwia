#include "ruby.h"

#ifndef RSTRING_PTR
#define RSTRING_PTR(obj) RSTRING(obj)->ptr
#endif
 
#ifndef RSTRING_LEN
#define RSTRING_LEN(obj) RSTRING(obj)->len
#endif

static int strhash(register const char *string)
{
    register int c;
    register int val = 0;
    while ((c = *string++) != '\0') {
	  val = val*997 + c;
    }
    return val + (val>>5);
}

static VALUE
rb_sym_hwia_hash(sym)
    VALUE sym;
{
	char *name;
	ID id = SYM2ID(sym);
	name = rb_id2name(id);
    return INT2FIX(strhash(name));
}

static VALUE
rb_str_hwia_hash(str)
    VALUE str;
{
	char *name;
	name = RSTRING_PTR(str);
    return INT2FIX(strhash(name));
}

void
Init_hwia()
{
	rb_define_method(rb_cString, "hwia_hash", rb_str_hwia_hash, 0);
	rb_define_method(rb_cSymbol, "hwia_hash", rb_sym_hwia_hash, 0);
}	