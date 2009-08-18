$:.unshift "." # 1.9.2
require File.dirname(__FILE__) + '/helper'

class TestSymbolAndStringHash < Test::Unit::TestCase  
  def test_hwia_hash
    assert_hash_keys 'key', :key
    assert_hash_keys 'under_scored_key', :under_scored_key
    assert_hash_keys '@ivar', :@ivar 
  end  
  
  private
  def assert_hash_keys(str,sym)
    assert_equal str.hwia_hash, sym.hwia_hash
  end  
end