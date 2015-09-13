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
    tagmaster.h \
    aliasenv_simple.h \
    aliasenv_maker.h \
    aliasenv_generator.h \
    aliasenv_bobox.h \
    loader_test.h \
    errorhandling.h \
    cartesian_multiplier.h \
    unit_test4/output.h \
    templates/simple_send.h \
    templates/simple_env_out.h \
    templates/simple_env_in.h \
    templates/simple_decl_out.h \
    templates/simple_decl_in.h \
    templates/simple_box.h \
    templates/bobox_send.h \
    templates/bobox_env_out.h \
    templates/bobox_env_in.h \
    templates/bobox_decl_out.h \
    templates/bobox_decl_in.h \
    templates/bobox_box.h \
    templates/simple_body.h \
    aliasenv_simu.h \
    templates/random_eng.h \
    templates/macros.h \
    templates/simu_send.h \
    templates/simu_env_out.h \
    templates/simu_env_in.h \
    templates/simu_decl_out.h \
    templates/simu_decl_in.h \
    templates/simu_decl_cont.h \
    templates/simu_box.h \
    templates/simu_body.h

OTHER_FILES += \
    test \
    sse_set/C_table.csv

