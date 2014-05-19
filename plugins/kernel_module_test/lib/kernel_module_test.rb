# -*- coding: utf-8 -*-
require 'ceedling/plugin'
require 'ceedling/constants'

# TODO ファイルコピーをしない方法検討
class KernelModuleTest < Plugin

  attr_reader :config

  def setup
    @module_name = ""
    @module_path = ""
    @ceedling_root=`pwd`.chomp
    @plugin_root = File.expand_path(File.join(File.dirname(__FILE__), '..'))

    configError unless defined?(KERNEL_MODULE_TEST_ARCH)
    case KERNEL_MODULE_TEST_ARCH
    when "x86" then
      arch_dir = "#{@plugin_root}/src/arch/x86/"
      arch_cflags="-DTEST -D\"UNITY_INT_WIDTH=32\" -D\"UNITY_LONG_WIDTH=32\""
    when "x86_64" then
      arch_dir = "#{@plugin_root}/src/arch/x86_64/"
      arch_cflags="-DTEST -D\"UNITY_INT_WIDTH=32\" -D\"UNITY_LONG_WIDTH=64\""
    when "arm" then
      arch_dir = "#{@plugin_root}/src/arch/arm/"
      arch_cflags="-DTEST -D\"UNITY_INT_WIDTH=32\" -D\"UNITY_LONG_WIDTH=32\""
    else
      configError
    end

    # kernel no support floating point
    @unity_cflags="#{arch_cflags} -DUNITY_EXCLUDE_DOUBLE -DUNITY_EXCLUDE_FLOAT -D\"UNITY_OUTPUT_CHAR(a)=kernel_module_putchar(a)\""
    @header_dirs = [arch_dir, "#{@plugin_root}/src/"]

    @sources = ["#{@plugin_root}/src/kernel_module.c", "#{arch_dir}/setjmp.S"]
    @objects = ["#{PROJECT_TEST_BUILD_OUTPUT_PATH}/kernel_module.o", "#{PROJECT_TEST_BUILD_OUTPUT_PATH}/setjmp.o"]
  end

  def pre_compile_execute(arg_hash)
    @sources.push(arg_hash[:source])
  end

  def pre_link_execute(arg_hash)
    @module_name = "unittest_"+File.basename(arg_hash[:executable], '.*')
    @module_path = "#{PROJECT_TEST_BUILD_OUTPUT_PATH}/#{@module_name}.ko"
    @objects += arg_hash[:objects]

    copy_souces_to_make_dir
    create_dummy_libc_header

    makefile_path = File.join(PROJECT_TEST_BUILD_OUTPUT_PATH, "Makefile")
    create_makefile(arg_hash, makefile_path)
    do_make(makefile_path)
  end

  def pre_test_fixture_execute(arg_hash)
    `sudo rmmod  #{@module_path} > /dev/null 2>&1`
    `sudo insmod #{@module_path}`
  end

  def post_test_fixture_execute(arg_hash)
    `sudo rmmod #{@module_path}`
  end


  private #############################

  def configError
    raise "You must have ':kernel_module_test:arch:{x86,x86_64,arm}' defined in your plugin configuration file"
  end

  def write_extra_cflags(stream)
    cflags = "-I#{@ceedling_root}/#{PROJECT_TEST_BUILD_OUTPUT_PATH} " # for unity_internals.h, unity.h, libc dummy headers

    (COLLECTION_PATHS_TEST_SUPPORT_SOURCE_INCLUDE_VENDOR+COLLECTION_PATHS_TEST_TOOLCHAIN_INCLUDE+@header_dirs).each do |i|
      if i[0].chr == '/'
        cflags += "-I#{i} "
      else
        cflags += "-I#{@ceedling_root}/#{i} " # change to absolute path
      end
    end
    stream.puts("EXTRA_CFLAGS += #{cflags} #{@unity_cflags}")
  end

  def write_all_target(stream)
    stream.puts("all:")
    stream.puts("\tmake -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules")
  end

  def write_clean_target(stream)
    stream.puts("clean:")
    stream.puts("\tmake -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean")
  end

  def write_module_objects(stream)
    stream.puts("obj-m += #{@module_name}.o")
    objs_str = ""
    @objects.each {|o| objs_str+="#{File.basename(o)} "}
    stream.puts("#{@module_name}-objs := #{objs_str}")
  end

  def create_makefile(arg_hash, makefile_path)
    @ceedling[:file_wrapper].open( makefile_path, 'w' ) do |f|
      write_module_objects(f)
      write_extra_cflags(f)
      write_all_target(f)
      write_clean_target(f)
    end
  end

  def do_make(makefile_path)
    `cd #{PROJECT_TEST_BUILD_OUTPUT_PATH}; make; cd -`
  end

  def copy_souces_to_make_dir
    (@sources).each do |f|
      `cp #{f} #{PROJECT_TEST_BUILD_OUTPUT_PATH}`
    end
  end

  # <stdio.h>, <stdint.h>, <setjmp.h>, <string.h>, <stdlib.h>
  def create_dummy_libc_header
    `echo "#include <linux/module.h>" > #{PROJECT_TEST_BUILD_OUTPUT_PATH}/stdio.h`
    `echo > #{PROJECT_TEST_BUILD_OUTPUT_PATH}/stdint.h`
    `echo > #{PROJECT_TEST_BUILD_OUTPUT_PATH}/stdlib.h`
    `echo > #{PROJECT_TEST_BUILD_OUTPUT_PATH}/string.h`
    `echo > #{PROJECT_TEST_BUILD_OUTPUT_PATH}/limits.h`
    `echo "#include <longjmp.h>" > #{PROJECT_TEST_BUILD_OUTPUT_PATH}/setjmp.h`
  end

end
