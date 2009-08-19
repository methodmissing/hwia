#include "ruby.h"
#ifndef RUBY18
#include "ruby/st.h"
#elif
#include "st.h"
#endif

VALUE rb_cHashWithIndifferentAccess;

static ID id_hwia_hash, id_hash;

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

int
rb_sym_hwia_hash(VALUE sym)
{
	ID id = SYM2ID(sym);
    return strhash((char*)rb_id2name(id));
}

static VALUE
rb_sym_hwia_hash_m(VALUE sym)
{
    return INT2FIX(rb_sym_hwia_hash(sym));
}

int
rb_str_hwia_hash(VALUE str)
{
    return strhash((char*)RSTRING_PTR(str));
}

static VALUE
rb_str_hwia_hash_m(VALUE str)
{
    return INT2FIX(rb_str_hwia_hash(str));
}

int
rb_hwia_cmp(VALUE s1,VALUE s2)
{
	int s1_hash = SYMBOL_P(s1) ? rb_sym_hwia_hash(s1) : rb_str_hwia_hash(s1);
	int s2_hash = SYMBOL_P(s2) ? rb_sym_hwia_hash(s2) : rb_str_hwia_hash(s2);
	if (s1_hash == s2_hash) return 0;
	if (s1_hash > s2_hash) return 1;
	return -1;	
}

static VALUE
eql(args)
    VALUE *args;
{
    return (VALUE)rb_eql(args[0], args[1]);
}

static int
rb_hwia_hash_cmp(a, b)
    VALUE a, b;
{
    VALUE args[2];

    if (a == b) return 0;
    if (FIXNUM_P(a) && FIXNUM_P(b)) {
	return a != b;
    }
    if (TYPE(a) == T_STRING && RBASIC(a)->klass == rb_cString &&
	TYPE(b) == T_STRING && RBASIC(b)->klass == rb_cString) {
	return rb_str_cmp(a, b);
    }
    if ((TYPE(a) == T_STRING && RBASIC(a)->klass == rb_cString && SYMBOL_P(b)) || (TYPE(b) == T_STRING && RBASIC(b)->klass == rb_cString && SYMBOL_P(a))) {
	return rb_hwia_cmp(a, b);
    }
    if (a == Qundef || b == Qundef) return -1;
    if (SYMBOL_P(a) && SYMBOL_P(b)) {
	return a != b;
    }

    args[0] = a;
    args[1] = b;
    return !rb_with_disable_interrupt(eql, (VALUE)args);
}

VALUE
rb_hwia(obj)
    VALUE obj;
{
    return rb_funcall(obj, id_hash, 0);
}

static int
rb_hwia_hash(a)
    VALUE a;
{
    VALUE hval;

    switch (TYPE(a)) {
      case T_FIXNUM:
	return (int)a;    
      case T_SYMBOL:
	return rb_sym_hwia_hash(a);
	break;

      case T_STRING:
	return rb_str_hwia_hash(a);
	break;

      default:
	hval = rb_funcall(a, id_hash, 0);
	if (!FIXNUM_P(hval)) {
	    hval = rb_funcall(hval, '%', 1, INT2FIX(536870923));
	}
	return (int)FIX2LONG(hval);
    }
}

static struct st_hash_type objhwia = {
    rb_hwia_hash_cmp,
    rb_hwia_hash,
};

static VALUE hwia_alloc0 _((VALUE));
static VALUE hwia_alloc _((VALUE));
static VALUE
hwia_alloc0(klass)
    VALUE klass;
{
    NEWOBJ(hash, struct RHash);
    OBJSETUP(hash, klass, T_HASH);

    hash->ifnone = Qnil;

    return (VALUE)hash;
}

static VALUE
hwia_alloc(klass)
    VALUE klass;
{
    VALUE hash = hwia_alloc0(klass);

    RHASH(hash)->tbl = st_init_table(&objhwia);

    return hash;
}

VALUE
rb_hwia_new()
{
    return hwia_alloc(rb_cHashWithIndifferentAccess);
}

static VALUE
rb_hwia_s_create(argc, argv, klass)
    int argc;
    VALUE *argv;
    VALUE klass;
{
    VALUE hash;
    int i;

    if (argc == 1 && TYPE(argv[0]) == T_HASH) {
	hash = hwia_alloc0(klass);
	RHASH(hash)->tbl = st_copy(RHASH(argv[0])->tbl);
	RHASH(hash)->tbl->type = &objhwia;
	return hash;
    }

    if (argc % 2 != 0) {
	rb_raise(rb_eArgError, "odd number of arguments for Hash");
    }

    hash = hwia_alloc(klass);
    for (i=0; i<argc; i+=2) {
        rb_hash_aset(hash, argv[i], argv[i + 1]);
    }

    return hash;
}

static int
rb_hash_rehash_i(key, value, tbl)
    VALUE key, value;
    st_table *tbl;
{
    if (key != Qundef) st_insert(tbl, key, value);
    return ST_CONTINUE;
}

static VALUE
rb_hwia_rehash(hash)
    VALUE hash;
{
    st_table *tbl;

    rb_hash_modify(hash);
    tbl = st_init_table_with_size(&objhwia, RHASH(hash)->tbl->num_entries);
    rb_hash_foreach(hash, rb_hash_rehash_i, (st_data_t)tbl);
    st_free_table(RHASH(hash)->tbl);
    RHASH(hash)->tbl = tbl;

    return hash;
}

void
Init_hwia()
{
	id_hash = rb_intern("hash");
	id_hwia_hash = rb_intern("hwia_hash");	
	
	rb_cHashWithIndifferentAccess = rb_define_class("HashWithIndifferentAccess", rb_cHash);

	rb_undef_alloc_func(rb_cHashWithIndifferentAccess);
    rb_define_alloc_func(rb_cHashWithIndifferentAccess, hwia_alloc);
    rb_define_singleton_method(rb_cHashWithIndifferentAccess, "[]", rb_hwia_s_create, -1);
	
	rb_define_method(rb_cString, "hwia_hash", rb_str_hwia_hash_m, 0);
	rb_define_method(rb_cSymbol, "hwia_hash", rb_sym_hwia_hash_m, 0);
    rb_define_method(rb_cHashWithIndifferentAccess,"rehash", rb_hwia_rehash, 0);	
}	