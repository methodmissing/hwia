require 'rake'
require 'rake/testtask'
require 'rake/clean'

HWIA_ROOT = 'ext/hwia'

desc 'Default: test'
task :default => :test

desc 'Run hwia tests.'
Rake::TestTask.new(:test) do |t|
  t.libs = [HWIA_ROOT]
  t.pattern = 'test/test_*.rb'
  t.verbose = true
end
task :test => :build

namespace :build do
  file "#{HWIA_ROOT}/hwia.c"
  file "#{HWIA_ROOT}/extconf.rb"
  file "#{HWIA_ROOT}/Makefile" => %W(#{HWIA_ROOT}/hwia.c #{HWIA_ROOT}/extconf.rb) do
    Dir.chdir(HWIA_ROOT) do
      ruby 'extconf.rb'
    end
  end

  desc "generate makefile"
  task :makefile => %W(#{HWIA_ROOT}/Makefile #{HWIA_ROOT}/hwia.c)

  dlext = Config::CONFIG['DLEXT']
  file "#{HWIA_ROOT}/hwia.#{dlext}" => %W(#{HWIA_ROOT}/Makefile #{HWIA_ROOT}/hwia.c) do
    Dir.chdir(HWIA_ROOT) do
      sh 'make' # TODO - is there a config for which make somewhere?
    end
  end

  desc "compile hwia extension"
  task :compile => "#{HWIA_ROOT}/hwia.#{dlext}"

  task :clean do
    Dir.chdir(HWIA_ROOT) do
      sh 'make clean'
    end if File.exists?("#{HWIA_ROOT}/Makefile")
  end

  CLEAN.include("#{HWIA_ROOT}/Makefile")
  CLEAN.include("#{HWIA_ROOT}/hwia.#{dlext}")
end

task :clean => %w(build:clean)

desc "compile"
task :build => %w(build:compile)

task :install do |t|
  Dir.chdir(HWIA_ROOT) do
    sh 'sudo make install'
  end
end

desc "clean build install"
task :setup => %w(clean build install)

desc "run benchmarks"
task :bench do
  ruby "bench/bench.rb"
end
task :bench => :build