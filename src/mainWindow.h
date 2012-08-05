#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtSql/QSqlDatabase>
#include <QFileInfo>
#include <QSettings>
#include <QTranslator>
#include <QMap>
#include <QLabel>

#include "categoriesForm.h"
#include "household.h"
#include "addIncomeExpenseDialog.h"
#include "configDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void								paintEvent(QPaintEvent* e);

public slots:
	//void								show();
	void								onAddIncomeCategoryRequested();
	void								onAddExpensesCategoryRequested();
	void								refreshCategoriesInDialogs();

private:
	// Members.
	Ui::MainWindow						*_ui;
	QTimer								*_timeRefresher;
	Household							*_household;
	AddIncomeExpenseDialog				*addIncomeDialog;
	AddIncomeExpenseDialog				*addExpenseDialog;
	QMultiMap<QString, QTranslator*>	_translations;
	QSettings							*_settings;
	QString								_language;
	QMap<QString, QString>				_languageTexts;
	QList<Data*>*						_dataList;

	bool								_isHouseholdLoaded;
	bool								_isHouseholdModified;
	bool								_dateFilterChanged;
	bool								_sortingChanged;
	bool								_showHelpTexts;
	bool								_debug;

	// Methods.
	void								fillTreeInfo(Data* d);
	void								fillTreeInfoAllData();
	void								fillDataTree();
	void								setupTimeRefresher();
	void								connectGlobalSignalsAndSlots();
	void								refreshUI();
	bool								createNewHousehold(const QFileInfo& fileInfo, const User& user, const Account& account);
	bool								openHousehold(const QFileInfo& fileInfo);
	void								refreshAccountsInDialogs();
	void								addData(const int type);
	void								addDataToTreeAndList(Data* newData);
	bool								filterData(const Data* const data);
	void								refreshSummaryTab();
	QList<QString>						getAvailableLanguagePacks();
	void								loadTranslations();
	void								changeLanguage();
	void								loadSettings();
	void								saveSettings();
	void								loadLanguageTexts();
	User								addUser();
	Account								addAccount();
	int									monthNumberFromEnglishShortName(const QString& monthName);
	void								scrollDataTabToId(qlonglong id);
	void								reloadData();
	void								refreshDataTree();
	void								changeDataTreeColumnVisibility();

private slots:
	void								closeHousehold();
	void								closeApplication();
	void								refreshTime();
	void								onNewHouseholdClicked();
	void								onOpenHouseholdClicked();
	void								onOpenCategoriesConfigClicked();
	void								onAddIncomeClicked();
	void								onAddExpenseClicked();
	void								onCurrentTabChanged(int index);
	void								showContextMenu(const QPoint& p);
	void								editData();
	void								removeData();
	void								onTreeWidgetDataHeaderClicked(int index);
	void								refreshDataTab();
	void								onApplyDataFilteringClicked();
	void								onOpenConfigurationClicked();
	void								onExportCategoriesClicked();
	void								onImportCategoriesClicked();
	void								onEditUsersClicked();
	void								onEditAccountsClicked();
	void								onTransferMoneyClicked();
	void								onToggleIncomeClicked(bool show);
	void								onToggleExpensesClicked(bool show);
	void								onDateFilterChanged();
	void								onAboutClicked();
	void								onDebugClicked();
	void								columnVisibilityContextMenu(const QPoint& p);
	void								onColumnVisibilityChanged(bool b);
	void								onAddCategoryFilterClicked();
	void								onRemoveCategoryFilterClicked();
	void								onClearCategoryFiltersClicked();
	void								onShowHideHelpTextsClicked();
	void								resizeAccountTreeColumns();
};

#endif // MAINWINDOW_H
