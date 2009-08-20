#include "ruby.h"
#ifndef RUBY18
#include "ruby/st.h"
#elif
#include "st.h"
#endif

VALUE rb_cStrHash;

static ID id_strhash_hash, id_hash;

#ifndef RSTRING_PTR
#define RSTRING_PTR(obj) RSTRING(obj)->ptr
#endif
 
#ifndef RSTRING_LEN
#define RSTRING_LEN(obj) RSTRING(obj)->len
#endif

/* hash.c */
static void
rb_hash_modify(VALUE hash)
{
    if (!RHASH(hash)->tbl) rb_raise(rb_eTypeError, "uninitialized Hash");
    if (OBJ_FROZEN(hash)) rb_error_frozen("hash");
    if (!OBJ_TAINTED(hash) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify hash");
}

static int 
strhash(register const char *string)
{
    register int c;
    register int val = 0;
    while ((c = *string++) != '\0') {
	  val = val*997 + c;
    }
    return val + (val>>5);
}

int
rb_sym_strhash(VALUE sym)
{
	ID id = SYM2ID(sym);
    return strhash((char*)rb_id2name(id));
}

static VALUE
rb_sym_strhash_m(VALUE sym)
{
    return INT2FIX(rb_sym_strhash(sym));
}

int
rb_str_strhash(VALUE str)
{
    return strhash((char*)RSTRING_PTR(str));
}

static VALUE
rb_str_strhash_m(VALUE str)
{
    return INT2FIX(rb_str_strhash(str));
}

int
rb_strhash_cmp(VALUE s1,VALUE s2)
{
	int s1_hash = SYMBOL_P(s1) ? rb_sym_strhash(s1) : rb_str_strhash(s1);
	int s2_hash = SYMBOL_P(s2) ? rb_sym_strhash(s2) : rb_str_strhash(s2);
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

/* hash.c */
static int
rb_strhash_hash_cmp(VALUE a, VALUE b)
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
	return rb_strhash_cmp(a, b);
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
rb_strhash(VALUE obj)
{
    return rb_funcall(obj, id_hash, 0);
}

/* hash.c */
static int
rb_strhash_hash(VALUE a)
{
    VALUE hval;

    switch (TYPE(a)) {
      case T_FIXNUM:
	return (int)a;    
      case T_SYMBOL:
	return rb_sym_strhash(a);
	break;

      case T_STRING:
	return rb_str_strhash(a);
	break;

      default:
	hval = rb_funcall(a, id_hash, 0);
	if (!FIXNUM_P(hval)) {
	    hval = rb_funcall(hval, '%', 1, INT2FIX(536870923));
	}
	return (int)FIX2LONG(hval);
    }
}

static struct st_hash_type objstrhash = {
    rb_strhash_hash_cmp,
    rb_strhash_hash,
};

static VALUE strhash_alloc0 _((VALUE));
static VALUE strhash_alloc _((VALUE));
/* hash.c */
static VALUE
strhash_alloc0(VALUE klass)
{
    NEWOBJ(hash, struct RHash);
    OBJSETUP(hash, klass, T_HASH);

    hash->ifnone = Qnil;

    return (VALUE)hash;
}

static VALUE
strhash_alloc(VALUE klass)
{
    VALUE hash = strhash_alloc0(klass);

    RHASH(hash)->tbl = st_init_table(&objstrhash);

    return hash;
}

VALUE
rb_strhash_new()
{
    return strhash_alloc(rb_cStrHash);
}

/* hash.c */
static int
rb_hash_rehash_i(VALUE key, VALUE value, st_table *tbl)
{
    if (key != Qundef) st_insert(tbl, key, value);
    return ST_CONTINUE;
}

/* hash.c */
static VALUE
rb_strhash_rehash(VALUE hash)
{
    st_table *tbl;

    rb_hash_modify(hash);
    tbl = st_init_table_with_size(&objstrhash, RHASH(hash)->tbl->num_entries);
    rb_hash_foreach(hash, rb_hash_rehash_i, (st_data_t)tbl);
    st_free_table(RHASH(hash)->tbl);
    RHASH(hash)->tbl = tbl;

    return hash;
}

/* hash.c */
static VALUE
rb_strhash_s_create(int argc, VALUE *argv, VALUE klass)
{
    VALUE hash;
    int i;

    if (argc == 1 && TYPE(argv[0]) == T_HASH) {
	hash = strhash_alloc0(klass);
	RHASH(hash)->tbl = st_copy(RHASH(argv[0])->tbl);
	RHASH(hash)->tbl->type = &objstrhash;
	return rb_strhash_rehash(hash);
    }

    if (argc % 2 != 0) {
	rb_raise(rb_eArgError, "odd number of arguments for Hash");
    }

    hash = strhash_alloc(klass);
    for (i=0; i<argc; i+=2) {
        rb_hash_aset(hash, argv[i], argv[i + 1]);
    }

    return hash;
}

static VALUE
rb_hash_strhash(VALUE hash)
{
	VALUE args[1];
	args[0] = hash;
	return rb_strhash_s_create(1, (VALUE *)args, rb_cStrHash );
}

void
Init_hwia()
{
	id_hash = rb_intern("hash");
	id_strhash_hash = rb_intern("strhash_hash");
	
	rb_cStrHash = rb_define_class("StrHash", rb_cHash);

	rb_undef_alloc_func(rb_cStrHash);
	rb_define_alloc_func(rb_cStrHash, strhash_alloc);
	rb_define_singleton_method(rb_cStrHash, "[]", rb_strhash_s_create, -1);
	
	rb_define_method(rb_cString, "strhash", rb_str_strhash_m, 0);
	rb_define_method(rb_cSymbol, "strhash", rb_sym_strhash_m, 0);
	rb_define_method(rb_cStrHash, "rehash", rb_strhash_rehash, 0);
	rb_define_method(rb_cHash, "strhash", rb_hash_strhash, 0);
}	