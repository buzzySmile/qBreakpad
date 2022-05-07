CONFIG += c++11

# internal file, used but breakpad-qt
BREAKPAD_PATH = $$PWD/breakpad  #for breakpad's third_patry(such as lss)
BREAKPAD_PATH_SRC = $$PWD/breakpad/src
INCLUDEPATH += $$BREAKPAD_PATH $$BREAKPAD_PATH_SRC

# every *nix
unix {
    SOURCES += \
        $$BREAKPAD_PATH_SRC/client/minidump_file_writer.cc \
        $$BREAKPAD_PATH_SRC/common/convert_UTF.cc \
        $$BREAKPAD_PATH_SRC/common/md5.cc \
        $$BREAKPAD_PATH_SRC/common/string_conversion.cc
}

# mac os x
mac {
    OBJECTIVE_SOURCES += \
        $$BREAKPAD_PATH_SRC/client/mac/crash_generation/crash_generation_client.cc \
        $$BREAKPAD_PATH_SRC/client/mac/handler/breakpad_nlist_64.cc \
        $$BREAKPAD_PATH_SRC/client/mac/handler/dynamic_images.cc \
        $$BREAKPAD_PATH_SRC/client/mac/handler/exception_handler.cc \
        $$BREAKPAD_PATH_SRC/client/mac/handler/minidump_generator.cc \
        $$BREAKPAD_PATH_SRC/common/mac/MachIPC.mm \
        $$BREAKPAD_PATH_SRC/common/mac/bootstrap_compat.cc \
        $$BREAKPAD_PATH_SRC/common/mac/file_id.cc \
        $$BREAKPAD_PATH_SRC/common/mac/macho_id.cc \
        $$BREAKPAD_PATH_SRC/common/mac/macho_utilities.cc \
        $$BREAKPAD_PATH_SRC/common/mac/macho_walker.cc \
        $$BREAKPAD_PATH_SRC/common/mac/string_utilities.cc
}

# other *nix
unix:!mac {
        SOURCES += \
        $$BREAKPAD_PATH_SRC/client/linux/crash_generation/crash_generation_client.cc \
        $$BREAKPAD_PATH_SRC/client/linux/dump_writer_common/thread_info.cc \
        $$BREAKPAD_PATH_SRC/client/linux/dump_writer_common/ucontext_reader.cc \
        $$BREAKPAD_PATH_SRC/client/linux/handler/exception_handler.cc \
        $$BREAKPAD_PATH_SRC/client/linux/handler/minidump_descriptor.cc \
        $$BREAKPAD_PATH_SRC/client/linux/log/log.cc \
        $$BREAKPAD_PATH_SRC/client/linux/microdump_writer/microdump_writer.cc \
        $$BREAKPAD_PATH_SRC/client/linux/minidump_writer/linux_core_dumper.cc \
        $$BREAKPAD_PATH_SRC/client/linux/minidump_writer/linux_dumper.cc \
        $$BREAKPAD_PATH_SRC/client/linux/minidump_writer/linux_ptrace_dumper.cc \
        $$BREAKPAD_PATH_SRC/client/linux/minidump_writer/minidump_writer.cc \
        $$BREAKPAD_PATH_SRC/common/linux/breakpad_getcontext.S \
        $$BREAKPAD_PATH_SRC/common/linux/elfutils.cc \
        $$BREAKPAD_PATH_SRC/common/linux/file_id.cc \
        $$BREAKPAD_PATH_SRC/common/linux/guid_creator.cc \
        $$BREAKPAD_PATH_SRC/common/linux/linux_libc_support.cc \
        $$BREAKPAD_PATH_SRC/common/linux/memory_mapped_file.cc \
        $$BREAKPAD_PATH_SRC/common/linux/safe_readlink.cc
}

win32 {
    SOURCES += \
        $$BREAKPAD_PATH_SRC/client/windows/crash_generation/crash_generation_client.cc \
        $$BREAKPAD_PATH_SRC/client/windows/handler/exception_handler.cc \
        $$BREAKPAD_PATH_SRC/common/windows/guid_string.cc
}
