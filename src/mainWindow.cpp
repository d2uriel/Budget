#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "constants.h"
#include "addCategoryDialog.h"
#include "addEditUserDialog.h"
#include "usersForm.h"
#include "accountsForm.h"
#include "addEditAccountsDialog.h"
#include "moneyTransferDialog.h"
#include "chart.h"

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QChar>
#include <QTranslator>
#include <QScrollBar>

void MessageHandler(QtMsgType type, const char* msg) {
	QFile debugLog(QApplication::applicationDirPath() + "/debug.log");
	debugLog.open(QFile::WriteOnly | QIODevice::Text | QFile::Append);
	QTextStream debugOut(&debugLog);
	switch(type) {
	case QtDebugMsg:
		debugOut << "Debug: " << msg << endl;
		break;
	case QtWarningMsg:
		debugOut << "Warning: " << msg << endl;
		break;
	case QtCriticalMsg:
		debugOut << "Critical: " << msg << endl;
		break;
	case QtFatalMsg:
		debugOut << "Fatal: " << msg << endl;
		abort();
	}
	debugOut.flush();
	debugLog.close();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
	qDebug() << "MainWindow::MainWindow()";
	_ui->setupUi(this);
	_settings = 0;
	loadLanguageTexts();
	loadSettings();
	loadTranslations();

	_ui->treeWidget_data->setColumnHidden(DATATREE_ID, true);
	_ui->treeWidget_data->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeWidget_data->header()->setClickable(true);
	_ui->treeWidget_data->header()->setContextMenuPolicy(Qt::CustomContextMenu);

	_household = 0;
	addIncomeDialog = new AddIncomeExpenseDialog(TYPE_INCOME, this);
	addExpenseDialog = new AddIncomeExpenseDialog(TYPE_EXPENSE, this);
	_dateFilterChanged = false;
	_sortingChanged = false;
	_debug = false;
	_dataList = 0;

	setupTimeRefresher();
	connectGlobalSignalsAndSlots();

#ifdef DEBUG
	QFileInfo fileInfo("D:/test.hhb");
	if(!openHousehold(fileInfo)) {
		QMessageBox::critical(this, tr("Error"), tr("Failed to open household."));
	} else {
		_dataList = _household->getDataList();
		_sortingChanged = true;
		fillTreeInfoAllData();
		fillDataTree();
	}
#else
	_ui->tabWidget->widget(2)->deleteLater();
	_ui->tabWidget->removeTab(2);
	_ui->menu_charts->menuAction()->setVisible(false);
	// Check for command line params.
	if(QApplication::arguments().size() > 1) {
		QString fileToOpen = QApplication::arguments().at(1);
		if(QFile::exists(fileToOpen)) {
			QFileInfo fileInfo(fileToOpen);
			if(!openHousehold(fileInfo)) {
				QMessageBox::critical(this, tr("Error"), tr("Failed to open household."));
			} else {
				_dataList = _household->getDataList();
				_sortingChanged = true;
				fillTreeInfoAllData();
				fillDataTree();
			}
		}
	}
#endif
	refreshUI();
}
/*
void MainWindow::show() {
	QMainWindow::show();
	refreshUI();
}
*/

void MainWindow::resizeAccountTreeColumns() {

}

void MainWindow::paintEvent(QPaintEvent* e) {
	QMainWindow::paintEvent(e);
	if(_ui->tab_summary->isVisible()) {
		refreshSummaryTab();
	}
}

void MainWindow::loadSettings() {
	qDebug() << "MainWindow::loadSettings()";
	if(!QFile::exists(QApplication::applicationDirPath() + "/" + SETTINGS)) {
		_settings = new QSettings(QApplication::applicationDirPath() + "/" + SETTINGS, QSettings::IniFormat, this);
		_settings->setValue(SETTINGS_LANGUAGE, "English");
	}
	if(_settings == 0) {
		_settings = new QSettings(QApplication::applicationDirPath() + "/" + SETTINGS, QSettings::IniFormat, this);
	}
	_language = _settings->value(SETTINGS_LANGUAGE).toString();
}

void MainWindow::saveSettings() {
	qDebug() << "MainWindow::saveSettings()";
}

MainWindow::~MainWindow() {
	qDebug() << "MainWindow::~MainWindow()";
	if(_household != 0) {
		delete _household;
	}
	_timeRefresher->stop();
	delete _timeRefresher;
	delete addIncomeDialog;
	delete addExpenseDialog;
	delete _ui;
}

void MainWindow::closeApplication() {
	qDebug() << "MainWindow::closeApplication()";
	closeHousehold();
	close();
}

void MainWindow::closeHousehold() {
	qDebug() << "MainWindow::closeHousehold()";
	if(_household != 0) {
		delete _household;
		_household = 0;
	}
	refreshUI();
}

void MainWindow::setupTimeRefresher() {
	qDebug() << "MainWindow::setupTimeRefresher()";
	_timeRefresher = new QTimer(this);
	_timeRefresher->start(1000);
	_ui->label_currentDateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss"));
}

void MainWindow::onDebugClicked() {
	qDebug() << "MainWindow::onDebugClicked()";
	switch(_debug) {
	case true:
		_debug = false;
		qInstallMsgHandler(0);
		_ui->statusBar->clearMessage();
		break;
	case false:
		_debug = true;
		qInstallMsgHandler(MessageHandler);
		_ui->statusBar->showMessage(tr("Debug mode enabled."));
		qDebug() << "###VERSION" + VERSION + ", DB VERSION" + DB_VERSION;
		break;
	}
}

void MainWindow::connectGlobalSignalsAndSlots() {
	qDebug() << "MainWindow::connectSignalsAndSlots()";
	connect(_ui->action_about, SIGNAL(triggered()), this, SLOT(onAboutClicked()));
	connect(_ui->action_exit, SIGNAL(triggered()), this, SLOT(closeApplication()));
	connect(_timeRefresher, SIGNAL(timeout()), this, SLOT(refreshTime()));
	connect(_ui->action_newHousehold, SIGNAL(triggered()), this, SLOT(onNewHouseholdClicked()));
	connect(_ui->action_editCategories, SIGNAL(triggered()), this, SLOT(onOpenCategoriesConfigClicked()));
	connect(_ui->action_addIncome, SIGNAL(triggered()), this, SLOT(onAddIncomeClicked()));
	connect(_ui->action_addExpense, SIGNAL(triggered()), this, SLOT(onAddExpenseClicked()));
	connect(_ui->action_openHousehold, SIGNAL(triggered()), this, SLOT(onOpenHouseholdClicked()));
	connect(_ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabChanged(int)));
	connect(_ui->treeWidget_data->header(), SIGNAL(sectionClicked(int)), this, SLOT(onTreeWidgetDataHeaderClicked(int)));
	connect(_ui->treeWidget_data->header(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(columnVisibilityContextMenu(QPoint)));
	connect(_ui->treeWidget_data, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
	connect(addIncomeDialog, SIGNAL(addCategoryRequested()), this, SLOT(onAddIncomeCategoryRequested()));
	connect(addExpenseDialog, SIGNAL(addCategoryRequested()), this, SLOT(onAddExpensesCategoryRequested()));
	connect(_ui->action_closeHousehold, SIGNAL(triggered()), this, SLOT(closeHousehold()));
	connect(_ui->action_configuration, SIGNAL(triggered()), this, SLOT(onOpenConfigurationClicked()));
	connect(_ui->action_exportCategories, SIGNAL(triggered()), this, SLOT(onExportCategoriesClicked()));
	connect(_ui->action_importCategories, SIGNAL(triggered()), this, SLOT(onImportCategoriesClicked()));
	connect(_ui->action_editUsers, SIGNAL(triggered()), this, SLOT(onEditUsersClicked()));
	connect(_ui->action_editAccounts, SIGNAL(triggered()), this, SLOT(onEditAccountsClicked()));
	connect(_ui->action_transferMoney, SIGNAL(triggered()), this, SLOT(onTransferMoneyClicked()));
	connect(_ui->pushButton_toggleIncome, SIGNAL(toggled(bool)), this, SLOT(onToggleIncomeClicked(bool)));
	connect(_ui->pushButton_toggleExpenses, SIGNAL(toggled(bool)), this, SLOT(onToggleExpensesClicked(bool)));
	connect(_ui->pushButton_addCategoryFilter, SIGNAL(clicked()), this, SLOT(onAddCategoryFilterClicked()));
	connect(_ui->pushButton_removeCategoryFilter, SIGNAL(clicked()), this, SLOT(onRemoveCategoryFilterClicked()));
	connect(_ui->pushButton_clearCategoryFilters, SIGNAL(clicked()), this, SLOT(onClearCategoryFiltersClicked()));
	connect(_ui->pushButton_applyFiltering, SIGNAL(clicked()), this, SLOT(onApplyDataFilteringClicked()));
	connect(_ui->radio_filterDatesLastMonth, SIGNAL(clicked()), this, SLOT(onDateFilterChanged()));
	connect(_ui->radio_filterDatesSpecific, SIGNAL(clicked()), this, SLOT(onDateFilterChanged()));
	connect(_ui->radio_filterDatesThisAndLastMonth, SIGNAL(clicked()), this, SLOT(onDateFilterChanged()));
	connect(_ui->radio_filterDatesThisMonth, SIGNAL(clicked()), this, SLOT(onDateFilterChanged()));
	connect(_ui->dateEdit_from, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onDateFilterChanged()));
	connect(_ui->dateEdit_to, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onDateFilterChanged()));
	connect(_ui->actionDebug_mode, SIGNAL(triggered()), this, SLOT(onDebugClicked()));
}

void MainWindow::fillTreeInfoAllData() {
	qDebug() << "MainWindow::fillTreeInfoAllData()";
	for(int i = 0; i < _dataList->count(); ++i) {
		Data* d = _dataList->at(i);
		fillTreeInfo(d);
	}
}

void MainWindow::fillTreeInfo(Data* d) {
	qDebug() << "MainWindow::fillTreeInfo()";
	d->setText(DATATREE_DATE, d->dateTime.date().toString("yyyy-MM-dd"));
	d->setText(DATATREE_CATEGORY, d->categoryName);
	d->setText(DATATREE_VALUE, Household::stringFromValueInt(d->value));
	d->setText(DATATREE_ID, QString::number(d->id));
	d->setText(DATATREE_QTY, Household::stringFromQtyInt(d->qty));
	d->setText(DATATREE_SUM, Household::stringFromValueInt(d->sum));
	d->setText(DATATREE_USERNAME, _household->getUserName(d->uid));
	d->setText(DATATREE_DETAILS, d->details);
	d->setTextAlignment(DATATREE_DATE, Qt::AlignHCenter);
	d->setTextAlignment(DATATREE_CATEGORY, Qt::AlignLeft);
	d->setTextAlignment(DATATREE_VALUE, Qt::AlignRight);
	d->setTextAlignment(DATATREE_QTY, Qt::AlignRight);
	d->setTextAlignment(DATATREE_SUM, Qt::AlignRight);
	QBrush brush = d->background(0);
	brush.setStyle(Qt::SolidPattern);
	if(d->type == TYPE_INCOME) {
		brush.setColor(QColor::fromRgb(230, 255, 230));
	} else if(d->type == TYPE_EXPENSE) {
		brush.setColor(QColor::fromRgb(255, 230, 230));
	}
	for(int j = 0; j < _ui->treeWidget_data->columnCount(); ++j) {
		d->setBackground(j, brush);
	}
}

void MainWindow::fillDataTree() {
	qDebug() << "MainWindow::fillDataTree()";
	for(int i = 0; i < _dataList->count(); ++i) {
		_ui->treeWidget_data->addTopLevelItem(_dataList->at(i));
	}
}

void MainWindow::refreshTime() {
	//qDebug() << "MainWindow::refreshTime()";
	_ui->label_currentDateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss"));
}

void MainWindow::refreshUI() {
	qDebug() << "MainWindow::refreshUI()";
	refreshTime();
	if(_household == 0) {
		_ui->tabWidget->hide();
		_ui->action_closeHousehold->setDisabled(true);
		_ui->action_addExpense->setDisabled(true);
		_ui->action_addIncome->setDisabled(true);
		_ui->action_editCategories->setDisabled(true);
		_ui->action_importCategories->setDisabled(true);
		_ui->action_exportCategories->setDisabled(true);
		_ui->action_editUsers->setDisabled(true);
		_ui->action_editAccounts->setDisabled(true);
		_ui->action_transferMoney->setDisabled(true);
		_ui->label_householdName->setText(tr("No household loaded."));
	}
	else {
		_ui->tabWidget->show();
		_ui->action_closeHousehold->setEnabled(true);
		_ui->action_addExpense->setEnabled(true);
		_ui->action_addIncome->setEnabled(true);
		_ui->action_editCategories->setEnabled(true);
		_ui->action_importCategories->setEnabled(true);
		_ui->action_exportCategories->setEnabled(true);
		_ui->label_householdName->setText(_household->getName());
		if(_household->currentUserAccessLevel() == ACCESS_ADMIN) {
			_ui->action_editUsers->setEnabled(true);
			_ui->action_editAccounts->setEnabled(true);
		}
		if(_household->getAccountCount() > 1) {
			_ui->action_transferMoney->setEnabled(true);
		}
		if(_ui->pushButton_toggleIncome->isChecked()) {
			_ui->pushButton_toggleIncome->setText(tr("Hide income"));
		} else {
			_ui->pushButton_toggleIncome->setText(tr("Show income"));
		}
		if(_ui->pushButton_toggleExpenses->isChecked()) {
			_ui->pushButton_toggleExpenses->setText(tr("Hide expenses"));
		} else {
			_ui->pushButton_toggleExpenses->setText(tr("Show expenses"));
		}
		_ui->dateEdit_from->setDate(_household->getDateFilterFrom().date());
		_ui->dateEdit_to->setDate(_household->getDateFilterTo().date());
		refreshSummaryTab();
		refreshDataTab();
		//refreshCategoriesInDialogs();
		refreshAccountsInDialogs();
	}
}

void MainWindow::refreshSummaryTab() {
	qDebug() << "MainWindow::refreshSummaryTab()";
	qlonglong incomeSumMonth = 0;
	qlonglong expensesSumMonth = 0;

	if(_household->getMonthStartsAt() == MONTH_STARTS_LAST_INCOME) {
		incomeSumMonth = _household->getIncomeSum(_household->getLastIncomeDate(), QDateTime::currentDateTime());
		expensesSumMonth = _household->getExpensesSum(_household->getLastIncomeDate(), QDateTime::currentDateTime());
	} else if(_household->getMonthStartsAt() == MONTH_STARTS_FIRST_DAY) {
		QDate currentDate = QDateTime::currentDateTime().date();
		incomeSumMonth = _household->getIncomeSum(QDateTime(QDate(currentDate.year(), currentDate.month(), 1)), QDateTime::currentDateTime());
		expensesSumMonth = _household->getExpensesSum(QDateTime(QDate(currentDate.year(), currentDate.month(), 1)), QDateTime::currentDateTime());
	}
	qlonglong moneyLeftMonth = incomeSumMonth - expensesSumMonth;
	_ui->label_incomeMonth->setText(Household::stringFromValueInt(incomeSumMonth));
	_ui->label_incomeMonth->setStyleSheet("QLabel { color: green; }");
	_ui->label_expensesMonth->setText(Household::stringFromValueInt(expensesSumMonth));
	_ui->label_expensesMonth->setStyleSheet("QLabel { color: red; }");
	_ui->label_moneyLeftMonth->setText(Household::stringFromValueInt(moneyLeftMonth));
	if(moneyLeftMonth < 0) {
		_ui->label_moneyLeftMonth->setStyleSheet("QLabel { color: red; }");
	} else if(moneyLeftMonth == 0){
		_ui->label_moneyLeftMonth->setStyleSheet("QLabel { color: black; }");
	} else {
		_ui->label_moneyLeftMonth->setStyleSheet("QLabel { color: green; }");
	}

	// Account summary.
	QList<Account*> accounts = _household->getAccountList();
	while(_ui->treeWidget_accountSummary->topLevelItemCount() > 0) {
		delete _ui->treeWidget_accountSummary->takeTopLevelItem(0);
	}
	QString longestValueString;
	for(int i = 0; i < accounts.count(); ++i) {
		longestValueString = "";
		Account* a = accounts[i];
		QTreeWidgetItem* i = new QTreeWidgetItem;
		i->setText(0, a->name);
		if(longestValueString.length() < Household::stringFromValueInt(a->money).length()) {
			longestValueString = Household::stringFromValueInt(a->money);
		}
		i->setText(1, Household::stringFromValueInt(a->money));
		i->setTextAlignment(1, Qt::AlignRight);
		_ui->treeWidget_accountSummary->addTopLevelItem(i);
	}
	QFontMetrics fm(_ui->treeWidget_accountSummary->fontMetrics());
	int treeWidgetWidth = _ui->treeWidget_accountSummary->width();
	int scrollBarWidth = 0;
	if(_ui->treeWidget_accountSummary->verticalScrollBar()->isVisible()) {
		scrollBarWidth = _ui->treeWidget_accountSummary->verticalScrollBar()->width();
	}
	_ui->treeWidget_accountSummary->setColumnWidth(0, treeWidgetWidth - fm.boundingRect(longestValueString).width() - 100);
	_ui->treeWidget_accountSummary->setColumnWidth(1, treeWidgetWidth - _ui->treeWidget_accountSummary->columnWidth(0) - scrollBarWidth);
}

void MainWindow::onNewHouseholdClicked() {
	qDebug() << "MainWindow::onNewHouseholdClicked()";
	if(_household != 0) {
		closeHousehold();
	}
	QString filter = tr("Households");
	filter.append(" (*" + DB_EXT + ")");
	QString filePath = QFileDialog::getSaveFileName(this, tr("Create new household"), QApplication::applicationDirPath(), filter);
	if(!filePath.isEmpty()) {
		if(QFile::exists(filePath)) {
			QFile file(filePath);
			file.remove();
		}
		QFileInfo fileInfo(filePath);
		User user = addUser();
		if(user.name.isEmpty()) {
			QMessageBox::information(this, tr("Information"), tr("You need to have at least one user in your household.\nNew household not created."));
			return;
		}
		Account account = addAccount();
		if(account.name.isEmpty()) {
			QMessageBox::information(this, tr("Information"), tr("You need to have at least one account in your household.\nNew household not created."));
			return;
		}
		account.isDefault = ACCOUNT_DEFAULT;
		if(!createNewHousehold(fileInfo, user, account)) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to create new household."));
			return;
		}
		refreshUI();
	}
}

bool MainWindow::createNewHousehold(const QFileInfo& fileInfo, const User& user, const Account& account) {
	qDebug() << "MainWindow::createNewHousehold()";
	bool error = false;
	if(_household != 0) {
		closeHousehold();
	}
	_household = new Household;
	_household->setName(fileInfo.baseName());
	_household->setPath(fileInfo.path());
	if(_household->createDB()) {
		if(!_household->addUser(user)) {
			error = true;
		} else {
			if(!_household->addAccount(account)) {
				error = true;
			} else {
				if(!_household->loadDB()) {
					error = true;
				}
			}
		}
	} else {
		error = true;
	}
	if(error) {
		delete _household;
		_household = 0;
		return false;
	}
	_household->setCurrentUser(user.name);
	return true;
}

bool MainWindow::openHousehold(const QFileInfo &fileInfo) {
	qDebug() << "MainWindow::openHousehold()";
	if(_household != 0) {
		closeHousehold();
	}
	_household = new Household;
	connect(_household, SIGNAL(categoriesChanged()), this, SLOT(refreshCategoriesInDialogs()));
	_household->setName(fileInfo.baseName());
	_household->setPath(fileInfo.path());
	bool success = true;
	if(_household->openDB()) {
		if(_household->loadDB()) {
#ifndef DEBUG
			if(!_household->login()) {
				success = false;
			}
#else
			_household->setCurrentUser("Daniel");
#endif
		} else {
			success = false;
		}
	} else {
		success = false;
	}
	if(!success) {
		delete _household;
		_household = 0;
		return false;
	}

	return true;
}

void MainWindow::onOpenHouseholdClicked() {
	qDebug() << "MainWindow::onOpenHouseholdClicked()";
	QString filter = tr("Households");
	filter.append(" (*" + DB_EXT + ")");
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open existing household"), QApplication::applicationDirPath(), filter);
	if(!filePath.isEmpty()) {
		QFileInfo fileInfo(filePath);
		if(!openHousehold(fileInfo)) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to open household."));
		} else {
			_dataList = _household->getDataList();
			_sortingChanged = true;
			fillTreeInfoAllData();
			fillDataTree();
			refreshUI();
		}
	}
}

void MainWindow::onOpenCategoriesConfigClicked() {
	qDebug() << "MainWindow::onOpenCategoriesConfigClicked()";
	CategoriesForm _categoriesForm(_household, this);
	_categoriesForm.exec();
	refreshUI();
}

void MainWindow::refreshCategoriesInDialogs() {
	qDebug() << "MainWindow::refreshCategoriesInDialogs()";
	addIncomeDialog->fillCategoriesComboBox(_household->getCategoryNames(TYPE_INCOME));
	addIncomeDialog->fillAccountsComboBox(_household->getAccountNames());
	addExpenseDialog->fillCategoriesComboBox(_household->getCategoryNames(TYPE_EXPENSE));
	addExpenseDialog->fillAccountsComboBox(_household->getAccountNames());
	_ui->listWidget_categories->clear();
	_ui->listWidget_categories->addItems(_household->getCategoryNames(TYPE_ALL));
	_ui->listWidget_activeFilters->clear();
}

void MainWindow::refreshAccountsInDialogs() {
	qDebug() << "MainWindow::refreshAccountsInDialogs()";
	addIncomeDialog->fillAccountsComboBox(_household->getAccountNames());
	addExpenseDialog->fillAccountsComboBox(_household->getAccountNames());
}

void MainWindow::onAddIncomeClicked() {
	qDebug() << "MainWindow::onAddIncomeClicked()";
	addData(TYPE_INCOME);
}

void MainWindow::onAddExpenseClicked() {
	qDebug() << "MainWindow::onAddExpenseClicked()";
	addData(TYPE_EXPENSE);
}

void MainWindow::addDataToTreeAndList(Data* newData) {
	qDebug() << "MainWindow::addDataToTreeAndList();";
	fillTreeInfo(newData);
	int index;
	int lastSortType = _household->getLastSortType();
	for(index = 0; index < _dataList->count(); ++index) {
		if(lastSortType & SORT_DATE) {
			if(lastSortType & SORT_DESCENDING) {
				if(newData->dateTime > _dataList->at(index)->dateTime) {
					break;
				}
			} else if(lastSortType & SORT_ASCENDING) {
				if(newData->dateTime < _dataList->at(index)->dateTime) {
					break;
				}
			}
		} else if(lastSortType & SORT_QTY) {
			if(lastSortType & SORT_DESCENDING) {
				if(newData->qty > _dataList->at(index)->qty) {
					break;
				}
			} else if(lastSortType & SORT_ASCENDING) {
				if(newData->qty < _dataList->at(index)->qty) {
					break;
				}
			}
		} else if(lastSortType & SORT_VALUE) {
			if(lastSortType & SORT_DESCENDING) {
				if(newData->value > _dataList->at(index)->value) {
					break;
				}
			} else if(lastSortType & SORT_ASCENDING) {
				if(newData->value < _dataList->at(index)->value) {
					break;
				}
			}
		} else if(lastSortType & SORT_SUM) {
			if(lastSortType & SORT_DESCENDING) {
				if(newData->sum > _dataList->at(index)->sum) {
					break;
				}
			} else if(lastSortType & SORT_ASCENDING) {
				if(newData->sum < _dataList->at(index)->sum) {
					break;
				}
			}
		} else if(lastSortType & SORT_CATEGORY) {
			if(lastSortType & SORT_DESCENDING) {
				if(newData->categoryName > _dataList->at(index)->categoryName) {
					break;
				}
			} else if(lastSortType & SORT_ASCENDING) {
				if(newData->categoryName < _dataList->at(index)->categoryName) {
					break;
				}
			}
		}
	}

	if(_ui->treeWidget_data->indexOfTopLevelItem(newData) != -1) {
		_ui->treeWidget_data->takeTopLevelItem(_ui->treeWidget_data->indexOfTopLevelItem(newData));
		_dataList->removeAt(_household->getDataIndexInList(*newData));
	}
	_ui->treeWidget_data->insertTopLevelItem(index, newData);
	_dataList->insert(index, newData);
	// Add data to list
}

void MainWindow::refreshDataTree() {
	qDebug() << "MainWindow::refreshDataTree()";
	while(_ui->treeWidget_data->topLevelItemCount() != 0) {
		_ui->treeWidget_data->takeTopLevelItem(0);
	}
	fillDataTree();
}

void MainWindow::reloadData() {
	qDebug() << "MainWindow::reloadData()";
	while(_ui->treeWidget_data->topLevelItemCount() != 0) {
		_ui->treeWidget_data->takeTopLevelItem(0);
	}
	while(!_dataList->isEmpty()) {
		delete _dataList->takeAt(0);
	}
	if(_household->getMonthStartsAt() == MONTH_STARTS_LAST_INCOME) {
		_household->loadDataList(_household->getLastIncomeDate(), QDateTime::currentDateTime());
	} else {
		// TBD
	}
	_dataList = _household->getDataList();
	fillTreeInfoAllData();
	fillDataTree();
}

void MainWindow::addData(const int type) {
	qDebug() << "MainWindow::addData()";
	AddIncomeExpenseDialog *dlg = 0;
	if(type == TYPE_INCOME) {
		dlg = addIncomeDialog;
	} else if(type == TYPE_EXPENSE) {
		dlg = addExpenseDialog;
	}
	if(_household->getAccountCount() == 1) {
		dlg->hideAccounts();
	} else {
		dlg->showAccounts();
	}
	if(dlg->add() == QDialog::Accepted) {
		Data* data = new Data;
		data->value = Household::valueIntFromMoneyString(dlg->getValue());
		data->categoryName = dlg->getCategoryName();
		data->cid = _household->getCategoryId(data->categoryName);
		data->dateTime = dlg->getDateTime();
		data->qty = Household::qtyIntFromMoneyString(dlg->getQty());
		data->sum = Household::multiplyToRoundedInt(data->qty, data->value);
		data->details = dlg->getDetails();
		data->aid = _household->getAccountId(dlg->getAccount());
		data->type = type;
		data->isMonthStart = dlg->getIsMonthStart();
		data->uid = _household->currentUserId();
		int retVal = _household->addData(*data);
		switch(retVal) {
		case DATA_RELOAD_NEEDED:
			reloadData();
			break;
		case DATA_ADD_EDIT_NOT_SUCCESSFULL:
			break;
		default:
			if(data->dateTime < _household->getDateFilterFrom() || data->dateTime > _household->getDateFilterTo()) {
				delete data;
			} else {
				addDataToTreeAndList(data);
			}
			break;
		}
		refreshUI();
	}
}


void MainWindow::editData() {
	qDebug() << "MainWindow::editData()";
	QTreeWidget *tree = _ui->treeWidget_data;
	QTreeWidgetItem *item = tree->currentItem();
	AddIncomeExpenseDialog *dlg = 0;
	if(item != 0) {
		Data* oldDataPtr = _household->getData(item->text(DATATREE_ID).toLongLong());
		Data oldData(*oldDataPtr);
		if(oldData.type == TYPE_INCOME) {
			dlg = addIncomeDialog;
		} else if(oldData.type == TYPE_EXPENSE) {
			dlg = addExpenseDialog;
		}
		dlg->setDateTime(oldData.dateTime);
		dlg->setQty(Household::stringFromQtyInt(oldData.qty));
		dlg->setValue(Household::stringFromValueInt(oldData.value));
		dlg->setCategoryName(oldData.categoryName);
		dlg->setIsMonthStart(oldData.isMonthStart);
		if(dlg->edit() == QDialog::Accepted) {
			Data newData(oldData);
			newData.value = Household::valueIntFromMoneyString(dlg->getValue());
			newData.categoryName = dlg->getCategoryName();
			newData.cid = _household->getCategoryId(newData.categoryName);
			newData.dateTime = dlg->getDateTime();
			newData.qty = Household::qtyIntFromMoneyString(dlg->getQty());
			newData.sum = Household::multiplyToRoundedInt(newData.qty, newData.value);
			newData.aid = _household->getAccountId(dlg->getAccount());
			newData.isMonthStart = dlg->getIsMonthStart();
			int retVal = _household->editData(oldData, newData);
			switch(retVal) {
			case DATA_RELOAD_NEEDED:
				reloadData();
				break;
			case DATA_ADD_EDIT_NOT_SUCCESSFULL:
				break;
			default:
				oldDataPtr->fromData(newData);
				fillTreeInfo(oldDataPtr);
				addDataToTreeAndList(oldDataPtr);
				break;
			}
			refreshUI();
		}
	}
}

void MainWindow::removeData() {
	qDebug() << "MainWindow::removeData()";
	QTreeWidget *tree = _ui->treeWidget_data;
	QTreeWidgetItem *item = tree->currentItem();
	if(item != 0) {
		Data data(*_household->getData(item->text(DATATREE_ID).toLongLong()));
		int retVal = _household->removeData(data);
		switch(retVal) {
		case DATA_RELOAD_NEEDED:
			reloadData();
			break;
		case DATA_ADD_EDIT_NOT_SUCCESSFULL:
			break;
		default:
			_ui->treeWidget_data->takeTopLevelItem(_ui->treeWidget_data->indexOfTopLevelItem(item));
			delete _dataList->takeAt(_household->getDataIndexInList(data));
			break;
		}
		refreshUI();
	}
}

void MainWindow::onCurrentTabChanged(int index) {
	qDebug() << "MainWindow::onCurrentTabChanged()";

	// Tabs: 0 - summary, 1 - data
	switch(index) {
	case 0:
		//refreshSummaryTab();
		break;
	case 1:
		//refreshDataTab();
		break;
	case 2:
		Chart* chart = new Chart(this);
		chart->setMinMaxDate(_household->getLastIncomeDate().addDays(0), QDateTime::currentDateTime());
		chart->setName("Chart");
		chart->addChartData("dupaJasiu", _household->getChartData(_household->getLastIncomeDate().addDays(0), QDateTime::currentDateTime()));
		_ui->verticalLayout_charts->addWidget(chart);
		break;
	}
}

bool MainWindow::filterData(const Data* const data) {
	qDebug() << "MainWindow::filterData()";
	bool filterOut = true;
	if(!_ui->pushButton_toggleIncome->isChecked() && data->type == TYPE_INCOME) {
		return true;
	}
	if(!_ui->pushButton_toggleExpenses->isChecked() && data->type == TYPE_EXPENSE) {
		return true;
	}
	if(_ui->listWidget_activeFilters->count() > 0) {
		for(int i = 0; i < _ui->listWidget_activeFilters->count(); ++i) {
			QListWidgetItem* item = _ui->listWidget_activeFilters->item(i);
			QList<QString> childCategories = _household->getChildCategoryNames(item->text());
			if(childCategories.count() > 0) {
				for(int j = 0; j < childCategories.count(); ++j) {
					if(data->categoryName == childCategories[j]) {
						filterOut = false;
						break;
					}
				}
			}
			if(data->categoryName == item->text()) {
				filterOut = false;
				break;
			}
		}
	} else {
		filterOut = false;
	}
	if(_ui->checkBox_useFilterSum->isChecked()) {
		qlonglong valueFrom = Household::valueIntFromMoneyString(QString::number(_ui->doubleSpinBox_from->value()));
		qlonglong valueTo = Household::valueIntFromMoneyString(QString::number(_ui->doubleSpinBox_to->value()));
		if(valueFrom != 0) {
			if(data->sum < valueFrom) {
				return true;
			}
		}
		if(valueTo != 0) {
			if(data->sum > valueTo) {
				return true;
			}
		}
	}
	return filterOut;
}

void MainWindow::onApplyDataFilteringClicked() {
	qDebug() << "MainWindow::onApplyDataFilteringClicked()";
	if(_dateFilterChanged) {
		if(_ui->radio_filterDatesSpecific->isChecked()) {
			_household->loadDataList(QDateTime(_ui->dateEdit_from->date()), QDateTime(_ui->dateEdit_to->date(), QTime(23, 59, 59)));
		}
		if(_ui->radio_filterDatesLastMonth->isChecked()) {

		}
		if(_ui->radio_filterDatesThisAndLastMonth->isChecked()) {

		}
		if(_ui->radio_filterDatesThisMonth->isChecked()) {
			if(_household->getMonthStartsAt() == MONTH_STARTS_LAST_INCOME) {
				_household->loadDataList(_household->getLastIncomeDate(), QDateTime::currentDateTime());
			} else if(_household->getMonthStartsAt() == MONTH_STARTS_FIRST_DAY) {
				QDate currentDate = QDateTime::currentDateTime().date();
				_household->loadDataList(QDateTime(QDate(currentDate.year(), currentDate.month(), 1)), QDateTime::currentDateTime());
			}
		}
		_dataList = _household->getDataList();
		_dateFilterChanged = false;
		fillTreeInfoAllData();
		fillDataTree();
	}
	refreshUI();
}

void MainWindow::refreshDataTab() {
	qDebug() << "MainWindow::refreshDataTab()";
	if(_sortingChanged) {
		refreshDataTree();
		_sortingChanged = false;
	}
	qlonglong sumOfAllRecords = 0;
	for(int i = 0; i < _dataList->count(); ++i) {
		Data* data = _dataList->at(i);
		if(filterData(data)) {
			data->setHidden(true);
		} else {
			data->setHidden(false);
			if(data->type == TYPE_INCOME) {
				sumOfAllRecords += data->sum;
			} else if(data->type == TYPE_EXPENSE) {
				sumOfAllRecords -= data->sum;
			}
		}
	}
	_ui->label_sumOfAllRecords->setText(Household::stringFromValueInt(sumOfAllRecords));
}

void MainWindow::onAddExpensesCategoryRequested() {
	qDebug() << "MainWindow::onAddExpensesCategoryRequested()";
	AddCategoryDialog dlg(this);
	dlg.fillCategoriesComboBox(_household->getCategoryNames(TYPE_EXPENSE));
	if(dlg.exec() == QDialog::Accepted) {
		Category c;
		c.name = dlg.getCategoryName();
		c.pid = _household->getCategoryId(dlg.getParentName().trimmed());
		c.type = TYPE_EXPENSE;
		_household->addCategory(c);
		addExpenseDialog->fillCategoriesComboBox(_household->getCategoryNames(TYPE_EXPENSE));
		addExpenseDialog->setCategoryName(dlg.getCategoryName());
	}
	QApplication::setActiveWindow(addExpenseDialog);
}

void MainWindow::onAddIncomeCategoryRequested() {
	qDebug() << "MainWindow::onAddIncomeCategoryRequested()";
	AddCategoryDialog dlg(this);
	dlg.fillCategoriesComboBox(_household->getCategoryNames(TYPE_INCOME));
	if(dlg.exec() == QDialog::Accepted) {
		Category c;
		c.name = dlg.getCategoryName();
		c.pid = _household->getCategoryId(dlg.getParentName().trimmed());
		c.type = TYPE_INCOME;
		_household->addCategory(c);
		addIncomeDialog->fillCategoriesComboBox(_household->getCategoryNames(TYPE_INCOME));
		addIncomeDialog->setCategoryName(dlg.getCategoryName());
	}
	QApplication::setActiveWindow(addIncomeDialog);
}

void MainWindow::showContextMenu(const QPoint &p) {
	qDebug() << "MainWindow::showContextMenu()";
	QMenu menu;
	QTreeWidgetItem *i = _ui->treeWidget_data->itemAt(p);
	if(i != 0) {
		if(_household->getUserId(i->text(DATATREE_USERNAME)) == _household->currentUserId()
				|| _household->currentUserAccessLevel() == ACCESS_ADMIN) {
			if(_household->getData(i->text(DATATREE_ID).toLongLong())->type == TYPE_INCOME) {
				menu.addAction(tr("Edit income"), this, SLOT(editData()));
				menu.addSeparator();
				menu.addAction(tr("Remove income"), this, SLOT(removeData()));
			} else if(_household->getData(i->text(DATATREE_ID).toLongLong())->type == TYPE_EXPENSE) {
				menu.addAction(tr("Edit expense"), this, SLOT(editData()));
				menu.addSeparator();
				menu.addAction(tr("Remove expense"), this, SLOT(removeData()));
			}
		}
		menu.exec(_ui->treeWidget_data->viewport()->mapToGlobal(p));
	}
}

void MainWindow::onTreeWidgetDataHeaderClicked(int index) {
	qDebug() << "MainWindow::onTreeWidgetDataHeaderClicked()";
	switch(index) {
	case DATATREE_DATE:
		_household->sortDataList(SORT_DATE);
		break;
	case DATATREE_QTY:
		_household->sortDataList(SORT_QTY);
		break;
	case DATATREE_VALUE:
		_household->sortDataList(SORT_VALUE);
		break;
	case DATATREE_SUM:
		_household->sortDataList(SORT_SUM);
		break;
	case DATATREE_CATEGORY:
		_household->sortDataList(SORT_CATEGORY);
		break;
		/*
	case DATATREE_USERNAME:
		_household->sortDataList(SORT_U);
		break;
		*/
	}
	_sortingChanged = true;
	refreshDataTab();
}

void MainWindow::scrollDataTabToId(qlonglong id) {
	qDebug() << "MainWindow::scrollDataTabToId()";
	QList<QTreeWidgetItem*> items = _ui->treeWidget_data->findItems(QString::number(id), Qt::MatchExactly, DATATREE_ID);
	_ui->treeWidget_data->scrollToItem(items[0], QAbstractItemView::PositionAtCenter);
}

void MainWindow::loadLanguageTexts() {
	qDebug() << "MainWindow::loadLanguageTexts()";
	QString path = QApplication::applicationDirPath() + "/lang/_languages.txt";
	if(QFile::exists(path)) {
		QFile file(path);
		file.open(QFile::ReadOnly | QIODevice::Text);
		QTextStream in(&file);
		while(!in.atEnd()) {
			QString line = in.readLine();
			if(line.left(1) == ";") {
				continue;
			} else {
				QStringList list = line.split("|");
				_languageTexts.insert(list[0], list[1]);
			}
		}
	} else {
		// Only english available.
	}
}

QList<QString> MainWindow::getAvailableLanguagePacks() {
	QList<QString> languagePacks;
	languagePacks.append("English");
	QDir dir;
	dir.setPath(QApplication::applicationDirPath() + "/lang");
	if(dir.exists()) {
		QStringList filters;
		filters << "*.qm";
		dir.setNameFilters(filters);
		QStringList files = dir.entryList();
		for(int i = 0; i < files.count(); ++i) {
			QString file = files[i];
			QString langPack = file.mid(file.length() - 5, 2).toLower();
			if(_languageTexts.find(langPack) != _languageTexts.end()) {
				if(!languagePacks.contains(_languageTexts[langPack])) {
					languagePacks.append(_languageTexts[langPack]);
				}
			} else {
				languagePacks.append(langPack);
			}
		}
	} else {
		// No language packs available.
	}
	return languagePacks;
}

void MainWindow::onOpenConfigurationClicked() {
	qDebug() << "MainWindow::onOpenConfigurationClicked()";
	ConfigDialog cfg(this);
	cfg.setAvailableLanguagePacks(getAvailableLanguagePacks());
	cfg.setLanguage(_language);
	if(cfg.exec() == QDialog::Accepted) {
		_settings->setValue(SETTINGS_LANGUAGE, cfg.getLanguage());
		_language = cfg.getLanguage();
		changeLanguage();
	}
}

void MainWindow::changeLanguage() {
	qDebug() << "MainWindow::changeLanguage()";
	for(QMultiMap<QString, QTranslator*>::const_iterator it = _translations.constBegin(); it != _translations.constEnd(); ++it) {
		if(it.key() == _language) {
			QApplication::installTranslator(it.value());
		}  else {
			QApplication::removeTranslator(it.value());
		}
	}
	_ui->retranslateUi(this);
	refreshUI();
}

void MainWindow::loadTranslations() {
	qDebug() << "MainWindow::loadTranslations()";
	QDir dir;
	dir.setPath(QApplication::applicationDirPath() + "/lang");
	if(dir.exists()) {
		QStringList filters;
		filters << "*.qm";
		dir.setNameFilters(filters);
		QStringList files = dir.entryList();
		for(int i = 0; i < files.count(); ++i) {
			QString file = files[i];
			QTranslator *translator = new QTranslator(this);
			translator->load(dir.path() + "/" + file);
			QString translation;
			QString langPack = file.mid(file.length() - 5, 2).toLower();
			if(_languageTexts.find(langPack) != _languageTexts.end()) {
				translation = _languageTexts[langPack];

			} else {
				translation = langPack;
			}
			_translations.insert(translation, translator);
			if(translation == _language) {
				QApplication::installTranslator(translator);
				_ui->retranslateUi(this);
			}
		}
	}
}

void MainWindow::onExportCategoriesClicked() {
	qDebug() << "MainWindow::exportCategories()";
	QString filter = tr("Category files");
	filter.append(" (*" + CAT_EXT + ")");
	QString filePath = QFileDialog::getSaveFileName(this, tr("Export categories"), QApplication::applicationDirPath(), filter);
	if(!filePath.isEmpty()) {
		const QList<Category*>* categories = _household->getCategories();
		QFile file(filePath);
		file.open(QFile::WriteOnly | QIODevice::Text);
		QTextStream out(&file);
		for(int i = 0; i < categories->count(); ++i) {
			Category *c = categories->at(i);
			out << QString::number(c->id) << "," << QString::number(c->pid) << "," << c->name << "," << c->type << "," << c->uid << "\n";
		}
		out.flush();
		file.close();
	}
}

void MainWindow::onImportCategoriesClicked() {
	qDebug() << "MainWindow::importCategories()";
	if(_household->getIncomeCategoryCount() > 0 || _household->getExpensesCategoryCount() > 0) {
		QMessageBox::information(this, tr("Information"), tr("You can only import categories to a household\nwhich doesn't have any categories yet."));
		return;
	}
	QString filter = tr("Category files");
	filter.append(" (*" + CAT_EXT + ")");
	QString filePath = QFileDialog::getOpenFileName(this, tr("Import categories"), QApplication::applicationDirPath(), filter);
	if(!filePath.isEmpty()) {
		QFile file(filePath);
		file.open(QFile::ReadOnly | QIODevice::Text);
		QTextStream in(&file);
		while(1) {
			if(in.atEnd()) {
				break;
			}
			QString categoryStr = in.readLine();
			QStringList categoryData = categoryStr.split(",");
			Category c;
			c.id = categoryData[0].toLongLong();
			c.pid = categoryData[1].toLongLong();
			c.name = categoryData[2];
			c.type = categoryData[3].toInt();
			c.uid = categoryData[4].toLongLong();
			_household->addCategory(c, false);
		}
		_household->refreshCategories();
	}
}

void MainWindow::onEditUsersClicked() {
	qDebug() << "MainWindow::onEditUsersClicked()";
	if(_household->currentUserAccessLevel() == ACCESS_ADMIN) {
		UsersForm dlg(_household, this);
		dlg.show();
		dlg.exec();
	}
}

User MainWindow::addUser() {
	qDebug() << "MainWindow::addUser()";
	AddEditUserDialog dlg(this);
	dlg.setAccessLevelDisabled();
	dlg.setAccessLevel(tr("Administrator"));
	if(dlg.exec() == QDialog::Accepted) {
		User u;
		u.name = dlg.getName();
		u.password = dlg.getPassword();
		u.description = dlg.getDescription();
		if(dlg.getAccessLevel() == tr("Administrator")) {
			u.level = ACCESS_ADMIN;
		} else {
			u.level = ACCESS_USER;
		}
		return u;
	}
	return User();
}

void MainWindow::onEditAccountsClicked() {
	qDebug() << "MainWindow::onEditAccountsClicked()";
	if(_household->currentUserAccessLevel() == ACCESS_ADMIN) {
		AccountsForm dlg(_household, this);
		dlg.show();
		dlg.exec();
		refreshUI();
	}
}

Account MainWindow::addAccount() {
	qDebug() << "MainWindow::addAccount()";
	AddEditAccountsDialog dlg(this);
	if(dlg.exec() == QDialog::Accepted) {
		Account a;
		a.name = dlg.getName();
		a.description = dlg.getDescription();
		a.money = 0;
		return a;
	}
	return Account();
}

void MainWindow::onTransferMoneyClicked() {
	qDebug() << "MainWindow::onTransferMoneyClicked()";
	MoneyTransferDialog dlg(_household, this);
	dlg.fillAccounts(_household->getAccountNames());
	if(dlg.exec() == QDialog::Accepted) {
		_household->transferMoneyBetweenAccounts(_household->getAccountId(dlg.getAccountFrom()), _household->getAccountId(dlg.getAccountTo()), Household::valueIntFromMoneyString(dlg.getValue()));
		refreshUI();
	}
}

void MainWindow::onToggleIncomeClicked(bool show) {
	qDebug() << "MainWindow::onToggleIncomeClicked()";
	switch(show) {
	case true:
		_ui->pushButton_toggleIncome->setText(tr("Hide income"));
		break;
	case false:
		_ui->pushButton_toggleIncome->setText(tr("Show income"));
		break;
	}
	refreshDataTab();
}

void MainWindow::onToggleExpensesClicked(bool show) {
	qDebug() << "MainWindow::onToggleExpensesClicked();";
	switch(show) {
	case true:
		_ui->pushButton_toggleExpenses->setText(tr("Hide expenses"));
		break;
	case false:
		_ui->pushButton_toggleExpenses->setText(tr("Show expenses"));
		break;
	}
	refreshDataTab();
}

void MainWindow::onDateFilterChanged() {
	qDebug() << "MainWindow::onDateFilterChanged()";
	_dateFilterChanged = true;

}

void MainWindow::onAddCategoryFilterClicked() {
	qDebug() << "MainWindow::onAddCategoryFilterClicked()";
	QList<QListWidgetItem*> list = _ui->listWidget_categories->selectedItems();
	for(int i = 0; i < list.count(); ++i) {
		_ui->listWidget_activeFilters->addItem(_ui->listWidget_categories->takeItem(_ui->listWidget_categories->currentRow()));
	}
}

void MainWindow::onRemoveCategoryFilterClicked() {
	qDebug() << "MainWindow::onRemoveCategoryFilterClicked()";
	QList<QListWidgetItem*> list = _ui->listWidget_activeFilters->selectedItems();
	for(int i = 0; i < list.count(); ++i) {
		_ui->listWidget_categories->addItem(_ui->listWidget_activeFilters->takeItem(_ui->listWidget_activeFilters->currentRow()));
	}
}

void MainWindow::onClearCategoryFiltersClicked() {
	qDebug() << "MainWindow::onClearCategoryFiltersClicked()";
	while(_ui->listWidget_activeFilters->count() > 0) {
		_ui->listWidget_categories->addItem(_ui->listWidget_activeFilters->takeItem(0));
	}
}

int MainWindow::monthNumberFromEnglishShortName(const QString &monthName) {
	if(monthName == "Jan") {
		return 1;
	} else if(monthName == "Feb") {
		return 2;
	} else if(monthName == "Mar") {
		return 3;
	} else if(monthName == "Apr") {
		return 4;
	} else if(monthName == "May") {
		return 5;
	} else if(monthName == "Jun") {
		return 6;
	} else if(monthName == "Jul") {
		return 7;
	} else if(monthName == "Aug") {
		return 8;
	} else if(monthName == "Sep") {
		return 9;
	} else if(monthName == "Oct") {
		return 10;
	} else if(monthName == "Nov") {
		return 11;
	} else if(monthName == "Dec") {
		return 12;
	}
	return 0;
}

void MainWindow::onAboutClicked() {
	qDebug() << "MainWindow::onAboutClicked()";
	QString dateStr = __DATE__;
	int monthNo = monthNumberFromEnglishShortName(dateStr.left(3));
	int yearNo = dateStr.right(4).toInt();
	int dayNo = dateStr.mid(4, dateStr.length() - 9).toInt();
	QDate date(yearNo, monthNo, dayNo);
	QMessageBox::information(this, tr("About"), tr("Household Budget - created by d2uriel.") + "\n\n" \
							 + tr("This piece of software is distributed under") + "\n" \
							 + tr("GNU LGPL version 2.1 and sources will be") + "\n" \
							 + tr("available as soon as version 1.0 is released.") + "\n" \
							 + tr("It comes as is with no warranty whatsoever.") + "\n" \
							 + tr("It may be freely redistributed.") + "\n" \
							 + tr("2012 - All rights reserved.") + "\n\n" \
							 + tr("Version: ") + VERSION + "\n" \
							 + tr("Build timestamp: ") + date.toString("yyyy-MM-dd") + " " + __TIME__);
}

void MainWindow::columnVisibilityContextMenu(const QPoint& p) {
	qDebug() << "MainWindow::columnVisibilityContextMenu()";
	//const int DT_ALL = DT_DATE | DT_QTY | DT_VAL | DT_SUM | DT_CAT | DT_USER | DT_DETAIL;
	QMenu* menu = new QMenu(this);
	menu->setTitle(tr("Column visibility:"));
	QAction* actDate = new QAction(menu); actDate->setText(tr("Date column")); actDate->setCheckable(true);
	QAction* actQty = new QAction(menu); actQty->setText(tr("Qty column")); actQty->setCheckable(true);
	QAction* actVal = new QAction(menu); actVal->setText(tr("Value column")); actVal->setCheckable(true);
	QAction* actSum = new QAction(menu); actSum->setText(tr("Sum column")); actSum->setCheckable(true);
	QAction* actCat = new QAction(menu); actCat->setText(tr("Category column")); actCat->setCheckable(true);
	QAction* actUser = new QAction(menu); actUser->setText(tr("Added by column")); actUser->setCheckable(true);
	QAction* actDetail = new QAction(menu); actDetail->setText(tr("Details column")); actDetail->setCheckable(true);
#ifdef DEBUG
	QAction* actId = new QAction(menu); actDetail->setText(tr("Id column")); actId->setCheckable(true);
#endif
	int colVis = _household->getColumnVisibility();
	int colsVisible = 0;
	if(colVis & DT_DATE) {
		actDate->setChecked(true);
		++colsVisible;
	}
	if(colVis & DT_QTY) {
		actQty->setChecked(true);
		++colsVisible;
	}
	if(colVis & DT_VAL) {
		actVal->setChecked(true);
		++colsVisible;
	}
	if(colVis & DT_SUM) {
		actSum->setChecked(true);
		++colsVisible;
	}
	if(colVis & DT_CAT) {
		actCat->setChecked(true);
		++colsVisible;
	}
	if(colVis & DT_USER) {
		actUser->setChecked(true);
		++colsVisible;
	}
	if(colVis & DT_DETAIL) {
		actDetail->setChecked(true);
		++colsVisible;
	}
#ifdef DEBUG
	if(colVis & DT_ID) {
		actId->setChecked(true);
	}
#endif
	if(colsVisible == 1) {
		if(colVis & DT_DATE) {
			actDate->setDisabled(true);
		} else if(colVis & DT_QTY) {
			actQty->setDisabled(true);
		} else if(colVis & DT_VAL) {
			actVal->setDisabled(true);
		} else if(colVis & DT_SUM) {
			actSum->setDisabled(true);
		} else if(colVis & DT_CAT) {
			actCat->setDisabled(true);
		} else if(colVis & DT_USER) {
			actUser->setDisabled(true);
		} else if(colVis & DT_DETAIL) {
			actDetail->setDisabled(true);
		}
	}
	menu->addAction(actDate); menu->addAction(actQty); menu->addAction(actVal); menu->addAction(actSum);
	menu->addAction(actCat); menu->addAction(actUser); menu->addAction(actDetail);
#ifdef DEBUG
	menu->addAction(actId);
	connect(actId, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
#endif
	connect(actDate, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	connect(actQty, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	connect(actVal, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	connect(actSum, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	connect(actCat, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	connect(actUser, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	connect(actDetail, SIGNAL(toggled(bool)), this, SLOT(onColumnVisibilityChanged(bool)));
	menu->exec(_ui->treeWidget_data->viewport()->mapToGlobal(p));
	menu->clear();
	delete menu;
}

void MainWindow::onColumnVisibilityChanged(bool b) {
	qDebug() << "MainWindow::onColumnVisibilityChanged()";
	QString columnName = ((QAction*)sender())->text();
	if(columnName == tr("Date column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_DATE);
	} else if(columnName == tr("Qty column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_QTY);
	} else if(columnName == tr("Value column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_VAL);
	} else if(columnName == tr("Sum column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_SUM);
	} else if(columnName == tr("Category column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_CAT);
	} else if(columnName == tr("Added by column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_USER);
	} else if(columnName == tr("Details column")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_DETAIL);
	}
#ifdef DEBUG
	else if(columnName == tr("Column Id")) {
		_household->setColumnVisibility(_household->getColumnVisibility() ^ DT_ID);
	}
#endif
	changeDataTreeColumnVisibility();
}

void MainWindow::changeDataTreeColumnVisibility() {
	qDebug() << "MainWindow::changeDataTreeColumnVisibility()";
	int colVis = _household->getColumnVisibility();
	if(colVis & DT_DATE) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_DATE, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_DATE, true);
	}
	if(colVis & DT_QTY) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_QTY, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_QTY, true);
	}
	if(colVis & DT_VAL) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_VALUE, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_VALUE, true);
	}
	if(colVis & DT_SUM) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_SUM, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_SUM, true);
	}
	if(colVis & DT_CAT) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_CATEGORY, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_CATEGORY, true);
	}
	if(colVis & DT_USER) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_USERNAME, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_USERNAME, true);
	}
	if(colVis & DT_DETAIL) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_DETAILS, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_DETAILS, true);
	}
#ifdef DEBUG
	if(colVis & DT_ID) {
		_ui->treeWidget_data->setColumnHidden(DATATREE_ID, false);
	} else {
		_ui->treeWidget_data->setColumnHidden(DATATREE_ID, true);
	}
#endif
}
