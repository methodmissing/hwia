#include "ruby.h"
#ifdef RUBY19
#include "ruby/st.h"
#else
#include "st.h"
#endif

VALUE rb_cStrHash;

static ID id_strhash, id_hash;
static VALUE hash_format;

#ifndef RSTRING_PTR
#define RSTRING_PTR(obj) RSTRING(obj)->ptr
#endif
 
#ifndef RSTRING_LEN
#define RSTRING_LEN(obj) RSTRING(obj)->len
#endif

#ifndef RARRAY_PTR
#define RARRAY_PTR(obj) RARRAY(obj)->heap.ptr
#endif
 
#ifndef RARRAY_LEN
#define RARRAY_LEN(obj) RARRAY(obj)->heap.len
#endif

#ifdef RUBY19
#define HASH_TBL(obj) RHASH(obj)->ntbl
#else
#define HASH_TBL(obj) RHASH(obj)->tbl
#endif

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
#ifdef RUBY18	
    VALUE args[2];
#endif
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
#ifdef RUBY18
    args[0] = a;
    args[1] = b;
    return !rb_with_disable_interrupt(eql, (VALUE)args);
#else
    return !rb_eql(a, b);
#endif
}

VALUE
rb_strhash(VALUE obj)
{
    VALUE hval = rb_funcall(obj, id_hash, 0);
#ifdef RUBY18
	return hval; 
#else	
 retry:
    switch (TYPE(hval)) {
      case T_FIXNUM:
	return hval;

      case T_BIGNUM:
	return LONG2FIX(((long*)(RBIGNUM_DIGITS(hval)))[0]);

      default:
	hval = rb_to_int(hval);
	goto retry;
    }
#endif
}

/* hash.c */
static int
rb_strhash_hash(VALUE a)
{
    VALUE hval, hnum;

    switch (TYPE(a)) {
      case T_FIXNUM:
#ifdef RUBY18
	return (int)a;  
#else
    hnum = a;
#endif	  
	break;
      case T_SYMBOL:
	hnum = rb_sym_strhash_m(a);
	break;
      case T_STRING:
	hnum = rb_str_strhash_m(a);
	break;

      default:
	hval = rb_hash(a);
#ifdef RUBY18
	if (!FIXNUM_P(hval)) {
	    hval = rb_funcall(hval, '%', 1, hash_format);
	}
#endif	
	hnum = FIX2LONG(hval);
    }
#ifdef RUBY18
    return (int)hnum;
#else
    hnum <<= 1;
    return (int)RSHIFT(hnum, 1);
#endif
}

static struct st_hash_type objstrhash = {
    rb_strhash_hash_cmp,
    rb_strhash_hash,
};

/* hash.c */
static void
rb_hash_modify(VALUE hash)
{
#ifdef RUBY18	
    if (!HASH_TBL(hash)) rb_raise(rb_eTypeError, "uninitialized Hash");
#endif
    if (OBJ_FROZEN(hash)) rb_error_frozen("hash");
    if (!OBJ_TAINTED(hash) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify hash");
#ifdef RUBY19
   if (!HASH_TBL(hash)) HASH_TBL(hash) = st_init_table(&objstrhash);
#endif
}

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

    HASH_TBL(hash) = st_init_table(&objstrhash);

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
    tbl = st_init_table_with_size(&objstrhash, HASH_TBL(hash)->num_entries);
    rb_hash_foreach(hash, rb_hash_rehash_i, (st_data_t)tbl);
    st_free_table(HASH_TBL(hash));
    HASH_TBL(hash) = tbl;

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
	HASH_TBL(hash) = st_copy(HASH_TBL(argv[0]));
	HASH_TBL(hash)->type = &objstrhash;
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
rb_strhash_strhash(VALUE hash)
{
	return hash;
}

static VALUE
rb_hash_strhash(VALUE hash)
{
	VALUE args[1];
	args[0] = hash;
	return rb_strhash_s_create(1, (VALUE *)args, rb_cStrHash );
}

/* temp. public API */
static VALUE
rb_strhash_convert(VALUE hash, VALUE val)
{
	int i;
	VALUE values;
    switch (TYPE(val)) {
      case T_HASH:
           return rb_hash_strhash(val);    
           break; 
      case T_ARRAY:
            values = rb_ary_new2(RARRAY_LEN(val));
            for (i = 0; i < RARRAY_LEN(val); i++) {
               VALUE el = RARRAY_PTR(val)[i];
               rb_ary_push(values, (TYPE(el) == T_HASH) ? rb_hash_strhash(el) : el);
            } 
            return values;
           break;
      default:
           return val;
    }
}

static VALUE
rb_strhash_aset(VALUE hash, VALUE key, VALUE val){
	VALUE converted = rb_strhash_convert(hash,val);
	rb_hash_aset(hash, key, converted);
	return converted;
}

/* hash.c */
static VALUE
to_strhash(hash)
    VALUE hash;
{
    return rb_convert_type(hash, T_HASH, "StrHash", "to_hash");
}

/* hash.c */
static int
rb_strhash_update_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_strhash_aset(hash, key, value);
    return ST_CONTINUE;
}

/* hash.c */
static int
rb_strhash_update_block_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    if (rb_hash_has_key(hash, key)) {
	value = rb_yield_values(3, key, rb_hash_aref(hash, key), value);
    }
    rb_strhash_aset(hash, key, value);
    return ST_CONTINUE;
}

/* hash.c */
static VALUE
rb_strhash_update(VALUE hash1, VALUE hash2)
{
    hash2 = to_strhash(hash2);
    if (rb_block_given_p()) {
	rb_hash_foreach(hash2, rb_strhash_update_block_i, hash1);
    }
    else {
	rb_hash_foreach(hash2, rb_strhash_update_i, hash1);
    }
    return hash1;
}

static VALUE
rb_strhash_initialize(int argc, VALUE *argv, VALUE hash){
	VALUE constructor;
	rb_scan_args(argc, argv, "01", &constructor);
	if(TYPE(constructor) == T_HASH){
		rb_strhash_update(hash,constructor);
		rb_call_super(argc,argv);
	}else{
		rb_call_super(argc,argv);
	}
	return hash;		
}

void
Init_hwia()
{
    id_hash = rb_intern("hash");
    id_strhash = rb_intern("strhash");
	hash_format = INT2FIX(536870923);

    rb_cStrHash = rb_define_class("StrHash", rb_cHash);

    rb_undef_alloc_func(rb_cStrHash);
    rb_define_alloc_func(rb_cStrHash, strhash_alloc);
    rb_define_singleton_method(rb_cStrHash, "[]", rb_strhash_s_create, -1);
  
    rb_define_method(rb_cString, "strhash", rb_str_strhash_m, 0);
    rb_define_method(rb_cSymbol, "strhash", rb_sym_strhash_m, 0);

    rb_define_method(rb_cStrHash,"initialize", rb_strhash_initialize, -1); 
    rb_define_method(rb_cStrHash, "rehash", rb_strhash_rehash, 0);
    rb_define_method(rb_cStrHash, "strhash", rb_strhash_strhash, 0);
    rb_define_method(rb_cStrHash, "convert", rb_strhash_convert, 1);
    rb_define_method(rb_cStrHash, "[]=", rb_strhash_aset, 2);
    rb_define_method(rb_cStrHash, "store", rb_strhash_aset, 2);
    rb_define_method(rb_cStrHash, "update", rb_strhash_update, 1);
    rb_define_method(rb_cStrHash, "merge!", rb_strhash_update, 1);
    rb_define_method(rb_cHash, "strhash", rb_hash_strhash, 0);
}	