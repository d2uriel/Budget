#-------------------------------------------------
#
# Project created by QtCreator 2012-06-15T09:55:20
#
#-------------------------------------------------

QT       += core gui network sql

TARGET = Budget
TEMPLATE = app

Debug:DEFINES+=DEBUG

SOURCES += main.cpp\
        mainWindow.cpp \
    categoriesForm.cpp \
    addCategoryDialog.cpp \
    household.cpp \
    addIncomeExpenseDialog.cpp \
    table.cpp \
    configDialog.cpp \
    addEditUserDialog.cpp \
    usersForm.cpp \
    loginDialog.cpp \
    addEditAccountsDialog.cpp \
    accountsForm.cpp \
    changePasswordDialog.cpp \
    moneyTransferDialog.cpp \
    chart.cpp \
    newChartDialog.cpp

HEADERS  += mainWindow.h \
    constants.h \
    categoriesForm.h \
    addCategoryDialog.h \
    household.h \
    addIncomeExpenseDialog.h \
    table.h \
    configDialog.h \
    addEditUserDialog.h \
    usersForm.h \
    loginDialog.h \
    addEditAccountsDialog.h \
    accountsForm.h \
    changePasswordDialog.h \
    moneyTransferDialog.h \
    chart.h \
    newChartDialog.h

FORMS    += mainWindow.ui \
    categoriesForm.ui \
    addCategoryDialog.ui \
    addIncomeExpense.ui \
    configDialog.ui \
    addEditUserDialog.ui \
    usersForm.ui \
    loginDialog.ui \
    addEditAccountsDialog.ui \
    accountsForm.ui \
    changePasswordDialog.ui \
    moneyTransferDialog.ui \
    newChartDialog.ui

TRANSLATIONS = budget_pl.ts

OTHER_FILES += \
    budget_pl.ts
