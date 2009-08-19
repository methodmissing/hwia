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

class TestHashWithIndifferentAccess < Test::Unit::TestCase
  def setup
    @strings = HashWithIndifferentAccess[ 'a' => 1, 'b' => 2 ]
    @symbols = HashWithIndifferentAccess[ :a  => 1, :b  => 2 ]
    @mixed   = HashWithIndifferentAccess[ :a  => 1, 'b' => 2 ]
    @fixnums = HashWithIndifferentAccess[  0  => 1,  1  => 2 ]
  end
  
  def test_inherits_hash
    assert_equal Hash, HashWithIndifferentAccess.superclass
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
    hash = HashWithIndifferentAccess.new
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
    hash = HashWithIndifferentAccess.new(1)
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
    hash = HashWithIndifferentAccess.new
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
    hash = HashWithIndifferentAccess.new
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
    hash = HashWithIndifferentAccess.new
    hash[:a] = 'failure'
    hash['b'] = 'failure'

    other = { 'a' => 1, :b => 2 }

    merged = hash.merge(other)

    assert_equal HashWithIndifferentAccess, merged.class
    assert_equal 1, merged[:a]
    assert_equal 2, merged['b']

    hash.update(other)

    assert_equal 1, hash[:a]
    assert_equal 2, hash['b']
  end  
  
  def test_deleting
    get_hash = proc{ HashWithIndifferentAccess[ :a => 'foo' ] }
    hash = get_hash.call
    assert_equal hash.delete(:a), 'foo'
    assert_equal hash.delete(:a), nil
    hash = get_hash.call
    assert_equal hash.delete('a'), 'foo'
    assert_equal hash.delete('a'), nil
  end  

=begin  
  def test_hash_with_array_of_hashes
    hash = { "urls" => { "url" => [ { "address" => "1" }, { "address" => "2" } ] }}
    hwia = HashWithIndifferentAccess[hash]
    assert_equal "1", hwia[:urls][:url].first[:address]
  end
  
  def test_indifferent_subhashes
    h = {'user' => {'id' => 5}}.with_indifferent_access
    ['user', :user].each {|user| [:id, 'id'].each {|id| assert_equal 5, h[user][id], "h[#{user.inspect}][#{id.inspect}] should be 5"}}

    h = {:user => {:id => 5}}.with_indifferent_access
    ['user', :user].each {|user| [:id, 'id'].each {|id| assert_equal 5, h[user][id], "h[#{user.inspect}][#{id.inspect}] should be 5"}}
  end
  
  def test_indifferent_slice
    original = { :a => 'x', :b => 'y', :c => 10 }.with_indifferent_access
    expected = { :a => 'x', :b => 'y' }.with_indifferent_access

    [['a', 'b'], [:a, :b]].each do |keys|
      # Should return a new hash with only the given keys.
      assert_equal expected, original.slice(*keys), keys.inspect
      assert_not_equal expected, original
    end
  end

  def test_indifferent_slice_inplace
    original = { :a => 'x', :b => 'y', :c => 10 }.with_indifferent_access
    expected = { :c => 10 }.with_indifferent_access

    [['a', 'b'], [:a, :b]].each do |keys|
      # Should replace the hash with only the given keys.
      copy = original.dup
      assert_equal expected, copy.slice!(*keys)
    end
  end

  def test_indifferent_slice_access_with_symbols
    original = {'login' => 'bender', 'password' => 'shiny', 'stuff' => 'foo'}
    original = original.with_indifferent_access

    slice = original.slice(:login, :password)

    assert_equal 'bender', slice[:login]
    assert_equal 'bender', slice['login']
  end
  
  def test_should_copy_the_default_value_when_converting_to_hash_with_indifferent_access
    hash = Hash.new(3)
    hash_wia = hash.with_indifferent_access
    assert_equal 3, hash_wia.default
  end    
=end    
end