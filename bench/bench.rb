require "benchmark"
$:.unshift "." #ruby 1.9.2
require File.dirname(__FILE__) + "/../ext/hwia/hwia"
require File.dirname(__FILE__) + "/as_hwia"

STR_HASH = { :a  => 1, 'b' => 2 }.strhash
HWIA_HASH = HashWithIndifferentAccess.new({ :a  => 1, 'b' => 2 })
HASH = { :d => :d, 'e' => :e } 

TESTS = 10_000
Benchmark.bmbm do |results|
  results.report("StrHash#[:sym]") { TESTS.times { STR_HASH[:a] } }
  results.report("HashWithIndifferentAccess#[:sym]") { TESTS.times { HWIA_HASH[:a] } }  
  results.report("StrHash#['str']") { TESTS.times { STR_HASH['b'] } }
  results.report("HashWithIndifferentAccess#['str]") { TESTS.times { HWIA_HASH['b'] } }
  results.report("StrHash#key?(:sym)") { TESTS.times { STR_HASH.key?(:a) } }
  results.report("HashWithIndifferentAccess#key?(:sym)") { TESTS.times { HWIA_HASH.key?(:a) } }  
  results.report("StrHash#key?('str')") { TESTS.times { STR_HASH.key?('a') } }
  results.report("HashWithIndifferentAccess#key?('str')") { TESTS.times { HWIA_HASH.key?('a') } } 
  results.report("StrHash#fetch(:sym)") { TESTS.times { STR_HASH.fetch(:a) } }
  results.report("HashWithIndifferentAccess#fetch(:sym)") { TESTS.times { HWIA_HASH.fetch(:a) } }  
  results.report("StrHash#fetch('str')") { TESTS.times { STR_HASH.fetch('a') } }
  results.report("HashWithIndifferentAccess#fetch('str')") { TESTS.times { HWIA_HASH.fetch('a') } }  
  results.report("StrHash#values_at(:sym)") { TESTS.times { STR_HASH.values_at(:a) } }
  results.report("HashWithIndifferentAccess#values_at(:sym)") { TESTS.times { HWIA_HASH.values_at(:a) } }  
  results.report("StrHash#values_at('str')") { TESTS.times { STR_HASH.values_at('a') } }
  results.report("HashWithIndifferentAccess#values_at('str')") { TESTS.times { HWIA_HASH.values_at('a') } }   
  results.report("StrHash#['str']=") { TESTS.times { STR_HASH['c'] = :c } }
  results.report("HashWithIndifferentAccess#['str]=") { TESTS.times { HWIA_HASH['c'] = :c } }  
  results.report("StrHash#[:sym]=") { TESTS.times { STR_HASH[:c] = :c } }
  results.report("HashWithIndifferentAccess#[:sym]=") { TESTS.times { HWIA_HASH[:c] = :c } }
  results.report("StrHash#update") { TESTS.times { STR_HASH.update(HASH) } }
  results.report("HashWithIndifferentAccess#update") { TESTS.times { HWIA_HASH.update(HASH) } } 
  results.report("StrHash#dup") { TESTS.times { STR_HASH.dup } }
  results.report("HashWithIndifferentAccess#dup") { TESTS.times { HWIA_HASH.dup } }   
  results.report("StrHash#merge") { TESTS.times { STR_HASH.merge(HASH) } }
  results.report("HashWithIndifferentAccess#merge") { TESTS.times { HWIA_HASH.merge(HASH) } }
end  