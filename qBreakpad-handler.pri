# test config
# TODO actually, I shoud check it better
LIST = thread exceptions rtti stl
for(f, LIST) {
    !CONFIG($$f) {
        warning("Add '$$f' to CONFIG, or you will find yourself in 'funny' problems.")
    }
}

INCLUDEPATH += $$PWD/handler/ $$PWD/handler/singletone

HEADERS += \
    $$PWD/handler/singletone/call_once.h \
    $$PWD/handler/singletone/singleton.h \
    $$PWD/handler/QBreakpadHandler.h
