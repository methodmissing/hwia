require 'hwia'
raise LoadError.new("Rails environment required!") unless Object.const_defined?(:ActiveSupport)

class Hash
  alias hash_with_indifferent_access strhash
end

class StrHash
  def stringify_keys!; self end
  def symbolize_keys!; self end
  def to_options!; self end
  
  protected
  # AS test suite compat only
  def convert_key(key)
    key.kind_of?(Symbol) ? key.to_s : key
  end  
end

begin
  old, $VERBOSE = $VERBOSE, nil
  ActiveSupport::HashWithIndifferentAccess = StrHash
  HashWithIndifferentAccess = ActiveSupport::HashWithIndifferentAccess
ensure
  $VERBOSE = old
end