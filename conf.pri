CONFIG += static
CONFIG += release

mac {
    CONFIG += c++11
    LIBS += -lcrypto
}
