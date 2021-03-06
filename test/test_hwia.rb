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
    assert_equal str.strhash, sym.strhash
  end  
end

class HashToStrHash < Test::Unit::TestCase
  def test_strhash
    hash = { 'a' => 1, 'b' => 2 }
    assert_instance_of StrHash, hash.strhash
    assert_equal %w(a b), hash.keys
    assert_equal [1,2], hash.values
  end  
end

class TestStrHash < Test::Unit::TestCase
  def setup
    @strings = { 'a' => 1, 'b' => 2 }.strhash
    @symbols = { :a  => 1, :b  => 2 }.strhash
    @mixed   = { :a  => 1, 'b' => 2 }.strhash
    @fixnums = {  0  => 1,  1  => 2 }.strhash
  end
  
  def test_inherits_hash
    assert_equal Hash, StrHash.superclass
  end  
    
  def test_strhash
    assert_equal @strings.object_id, @strings.strhash.object_id
    assert_instance_of StrHash, { 'a' => 1, 'b' => 2 }.strhash
  end
  
  def test_initialize
    strhash = StrHash.new({ 'a' => 1, 'b' => 2 })
    assert_equal 1, strhash[:a] 
    strhash = StrHash.new
    strhash[:a] = 'a'
    assert_equal 'a', strhash[:a]
  end
  
  def test_convert
    assert_equal 'a', @strings['a'] = 'a'
    hash = { 'a' => 1, 'b' => 2 }
    @strings[:str_hash] = hash
    assert_instance_of StrHash, @strings[:str_hash] 
    assert_equal %w(a b), @strings[:str_hash].keys
    assert_equal [:a,:b,:c], @strings[:array] = [:a,:b,:c]
    array_with_hash = [{ 'a' => 1, 'b' => 2 }, [:a,:b,:c]]
    @strings[:array_with_hash] = array_with_hash
    assert_instance_of StrHash, @strings[:array_with_hash].shift   
    assert_equal [:a,:b,:c], @strings[:array_with_hash].pop 
    @strings[:other_hash] = { 'a' => 1, 'b' => 2 }
    assert_instance_of StrHash, @strings[:other_hash]
    nested_hash = { "urls" => { "url" => [ { "address" => "1" }, { "address" => "2" } ] }}
    @strings[:nested_hash] = nested_hash
    assert_instance_of Array, @strings[:nested_hash][:urls][:url] 
  end
  
  def test_set
    array = [{ 'a' => 1, 'b' => 2 }, [:a,:b,:c]]
    @strings[:array] = array
    assert_instance_of StrHash, @strings[:array].shift
    assert_instance_of Array, @strings[:array] = array    
    assert_instance_of StrHash, @strings[:hash] = { 'a' => 1, 'b' => 2 }
  end
  
  def test_dup
    assert_equal @strings, @strings.dup
    assert_equal @mixed, @mixed.dup
    assert_not_equal @mixed.object_id, @mixed.dup.object_id
  end
    
  def test_keys
    assert_equal ["a", "b"], @strings.keys
    assert_equal [:a, :b], @symbols.keys
    assert_equal [:a, "b"], @mixed.keys
    assert_equal [0, 1], @fixnums.keys            
  end
  
  def test_values
    assert_equal [1, 2], @strings.values
    assert_equal [1, 2], @symbols.values
    assert_equal [1, 2], @mixed.values
    assert_equal [1, 2], @fixnums.values         
  end  
 
  def test_fetch
    assert_equal 1, @strings.fetch('a')
    assert_equal 1, @strings.fetch(:a.to_s)
    assert_equal 1, @strings.fetch(:a)
  end
  
  def test_key?
    assert @strings.key?(:a)
    assert @strings.include?('a')
    assert @mixed.has_key?('b')
  end
  
  def test_delete
    @strings.delete('a')
    assert !@strings.key?(:a)
  end
  
  def test_assorted
    hashes = { :@strings => @strings, :@symbols => @symbols, :@mixed => @mixed }
    method_map = { :'[]' => 1, :fetch => 1, :values_at => [1],
      :has_key? => true, :include? => true, :key? => true,
      :member? => true }

    hashes.each do |name, hash|
      method_map.sort_by { |m| m.to_s }.each do |meth, expected|
        assert_equal(expected, hash.__send__(meth, 'a'),
                     "Calling #{name}.#{meth} 'a'")
        assert_equal(expected, hash.__send__(meth, :a),
                     "Calling #{name}.#{meth} :a")
      end
    end

    assert_equal [1, 2], @strings.values_at('a', 'b')
    assert_equal [1, 2], @strings.values_at(:a, :b)
    assert_equal [1, 2], @symbols.values_at('a', 'b')
    assert_equal [1, 2], @symbols.values_at(:a, :b)
    assert_equal [1, 2], @mixed.values_at('a', 'b')
    assert_equal [1, 2], @mixed.values_at(:a, :b)
  end
  
  def test_reading
    hash = StrHash.new
    hash["a"] = 1
    hash["b"] = true
    hash["c"] = false
    hash["d"] = nil

    assert_equal 1, hash[:a]
    assert_equal true, hash[:b]
    assert_equal false, hash[:c]
    assert_equal nil, hash[:d]
    assert_equal nil, hash[:e]
  end  
  
  def test_reading_with_nonnil_default
    hash = StrHash.new(1)
    hash["a"] = 1
    hash["b"] = true
    hash["c"] = false
    hash["d"] = nil

    assert_equal 1, hash[:a]
    assert_equal true, hash[:b]
    assert_equal false, hash[:c]
    assert_equal nil, hash[:d]
    assert_equal 1, hash[:e]
  end  

  def test_writing
    hash = StrHash.new
    hash[:a] = 1
    hash['b'] = 2
    hash[3] = 3

    assert_equal hash['a'], 1
    assert_equal hash['b'], 2
    assert_equal hash[:a], 1
    assert_equal hash[:b], 2
    assert_equal hash[3], 3
  end  

  def test_update
    hash = StrHash.new
    hash[:a] = 'a'
    hash['b'] = 'b'

    updated_with_strings = hash.update(@strings)
    updated_with_symbols = hash.update(@symbols)
    updated_with_mixed = hash.update(@mixed)

    assert_equal updated_with_strings[:a], 1
    assert_equal updated_with_strings['a'], 1
    assert_equal updated_with_strings['b'], 2

    assert_equal updated_with_symbols[:a], 1
    assert_equal updated_with_symbols['b'], 2
    assert_equal updated_with_symbols[:b], 2

    assert_equal updated_with_mixed[:a], 1
    assert_equal updated_with_mixed['b'], 2

    assert [updated_with_strings, updated_with_symbols, updated_with_mixed].all? { |h| h.keys.size == 2 }
  end  
  
  def test_merging
    hash = StrHash.new
    hash[:a] = 'failure'
    hash['b'] = 'failure'

    other = { 'a' => 1, :b => 2 }

    merged = hash.merge(other)

    assert_equal StrHash, merged.class
    assert_equal 1, merged[:a]
    assert_equal 2, merged['b']

    hash.update(other)

    assert_equal 1, hash[:a]
    assert_equal 2, hash['b']
  end  
  
  def test_deleting
    get_hash = proc{ StrHash[ :a => 'foo' ] }
    hash = get_hash.call
    assert_equal hash.delete(:a), 'foo'
    assert_equal hash.delete(:a), nil
    hash = get_hash.call
    assert_equal hash.delete('a'), 'foo'
    assert_equal hash.delete('a'), nil
  end  

  def test_to_hash
    assert_instance_of Hash, @strings.to_hash
    assert_equal %w(a b), @strings.to_hash.keys
    # Should convert to a Hash with String keys.
    assert_equal @strings, @mixed.strhash.to_hash

    # Should preserve the default value.
    mixed_with_default = @mixed.dup
    mixed_with_default.default = '1234'
    roundtrip = mixed_with_default.strhash.to_hash
    assert_equal @strings, roundtrip
    assert_equal '1234', roundtrip.default    
  end

  def test_hash_with_array_of_hashes
    hash = { "urls" => { "url" => [ { "address" => "1" }, { "address" => "2" } ] }}
    hwia = StrHash[hash]
    assert_equal "1", hwia[:urls][:url].first[:address]
  end

  def test_indifferent_subhashes
    h = {'user' => {'id' => 5}}.strhash
    ['user', :user].each {|user| [:id, 'id'].each {|id| assert_equal 5, h[user][id], "h[#{user.inspect}][#{id.inspect}] should be 5"}}

    h = {:user => {:id => 5}}.strhash
    ['user', :user].each {|user| [:id, 'id'].each {|id| assert_equal 5, h[user][id], "h[#{user.inspect}][#{id.inspect}] should be 5"}}
  end  

  def test_assorted_keys_not_stringified
    original = {Object.new => 2, 1 => 2, [] => true}
    indiff = original.strhash
    assert(!indiff.keys.any? {|k| k.kind_of? String}, "A key was converted to a string!")
  end

  def test_should_use_default_value_for_unknown_key
    hash_wia = StrHash.new(3)
    assert_equal 3, hash_wia[:new_key]
  end

  def test_should_use_default_value_if_no_key_is_supplied
    hash_wia = StrHash.new(3)
    assert_equal 3, hash_wia.default
  end

  def test_should_nil_if_no_default_value_is_supplied
    hash_wia = StrHash.new
    assert_nil hash_wia.default
  end

  def test_should_copy_the_default_value_when_converting_to_hash_with_indifferent_access
    hash = Hash.new(3)
    hash_wia = hash.strhash
    assert_equal 3, hash_wia.default
  end      
end