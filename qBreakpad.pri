message("BREAKPAD_crash_handler_attached")

# test config
# TODO actually, I shoud check it better
LIST = thread exceptions rtti stl
for(f, LIST) {
    !CONFIG($$f) {
        warning("Add '$$f' to CONFIG, or you will find yourself in 'funny' problems.")
    }
}

INCLUDEPATH += $$PWD/handler/

HEADERS += \
    $$PWD/handler/QBreakpadHandler.h \
    $$PWD/handler/QBreakpadHttpUploader.h
