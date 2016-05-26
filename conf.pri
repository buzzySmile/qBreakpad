CONFIG += static
CONFIG += release

macx {
    CONFIG += c++11
    LIBS += -lcrypto
}
