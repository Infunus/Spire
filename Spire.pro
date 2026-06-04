QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += src src/core src/views src/map src/ui

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/views/MainWindow.cpp \
    src/views/MainMenuView.cpp \
    src/views/EventView.cpp \
    src/views/BattleView.cpp \
    src/views/ShopView.cpp \
    src/views/RewardView.cpp \
    src/views/MapView.cpp \
    src/views/BaseView.cpp \
    src/ui/StatusBarWidget.cpp \
    src/map/MapNode.cpp \
    src/map/MapGenerator.cpp \
    src/core/GameManager.cpp \
    src/core/EventBus.cpp \
    src/data/CardData.cpp \
    src/data/EnemyData.cpp \
    src/data/EventData.cpp \
    src/data/CharacterData.cpp \
    src/data/RelicData.cpp \
    src/data/PotionData.cpp

HEADERS += \
    src/views/MainWindow.h \
    src/views/MainMenuView.h \
    src/views/EventView.h \
    src/views/BattleView.h \
    src/views/ShopView.h \
    src/views/RewardView.h \
    src/views/MapView.h \
    src/views/BaseView.h \
    src/ui/StatusBarWidget.h \
    src/map/MapNode.h \
    src/map/MapGenerator.h \
    src/core/GameManager.h \
    src/core/RunData.h \
    src/core/EventBus.h \
    src/data/CardData.h \
    src/data/EnemyData.h \
    src/data/EventData.h \
    src/data/CharacterData.h \
    src/data/RelicData.h \
    src/data/PotionData.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
