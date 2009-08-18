require 'mkmf'

dir_config('hwia')

create_makefile('hwia')

$defs.push("-DRUBY18") if have_var('rb_trap_immediate', ['ruby.h', 'rubysig.h'])