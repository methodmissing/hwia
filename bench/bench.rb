require "benchmark"
$:.unshift "." #ruby 1.9.2
require File.dirname(__FILE__) + "/../ext/hwia/hwia"
require File.dirname(__FILE__) + "/as_hwia"

STR_HASH = { :a  => 1, 'b' => 2 }.strhash
HWIA_HASH = HashWithIndifferentAccess.new({ :a  => 1, 'b' => 2 })

TESTS = 10_000
Benchmark.bmbm do |results|
  results.report("StrHash#[:sym]") { TESTS.times { STR_HASH[:a] } }
  results.report("HashWithIndifferentAccess#[:sym]") { TESTS.times { HWIA_HASH[:a] } }  
end  