Gem::Specification.new do |s|
  s.name     = "hwia"
  s.version  = "1.0.2"
  s.date     = "2009-08-23"
  s.summary  = "A faster HashWithIndifferentAccess (hwia) for MRI"
  s.email    = "lourens@methodmissing.com"
  s.homepage = "http://github.com/methodmissing/hwia"
  s.description = "A faster HashWithIndifferentAccess (hwia) for MRI (1.8.{6,7} and 1.9.2)"
  s.has_rdoc = true
  s.authors  = ["Lourens Naudé (methodmissing)"]
  s.platform = Gem::Platform::RUBY
  s.files    = %w[
    README
    Rakefile
    bench/bench.rb
    bench/as_hwia.rb
    ext/hwia/extconf.rb
    ext/hwia/hwia.c
    lib/hwia_rails.rb
    hwia.gemspec
  ] + Dir.glob('test/*')
  s.rdoc_options = ["--main", "README"]
  s.extra_rdoc_files = ["README"]
  s.extensions << "ext/hwia/extconf.rb"
end