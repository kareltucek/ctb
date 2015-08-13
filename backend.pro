TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    xmlloader.cpp \
    writer.cpp \
    test.cpp \
    instructions.cpp \
    graph.cpp \
    datatypes.cpp \
    backend.cpp \
    model.cpp \
    model_bobox.cpp \
    ctb.cpp \
    model_simple.cpp \
    main.cpp

HEADERS += \
    xmlloader.h \
    writer.h \
    instructions.h \
    graph.h \
    datatypes.h \
    backend.h \
    model.h \
    model_bobox.h \
    model_simple.h \
    output/test_simple.h \
    ctb.h \
    generator.h \
    model_generator.h \
    languages.h \
    csvloader.h \
    loader_csv.h \
    loader_xml.h \
    model_maker.h \
    proxy.h \
    tagmaster.h

OTHER_FILES += \
    test

