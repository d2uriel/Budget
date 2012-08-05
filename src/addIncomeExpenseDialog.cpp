#include "addIncomeExpenseDialog.h"
#include "ui_addIncomeExpense.h"
#include "household.h"
#include <QMessageBox>
#include <QDebug>

AddIncomeExpenseDialog::AddIncomeExpenseDialog(const int type, QWidget *parent) : QDialog(parent), ui(new Ui::addIncomeExpense) {
	qDebug() << "AddIncomeExpenseDialog::AddIncomeExpenseDialog()";
	ui->setupUi(this);
	setWindowModality(Qt::ApplicationModal);
	_type = type;
	if(_type == TYPE_INCOME) {
		this->setWindowTitle(tr("Add income"));
		ui->label_qtyText->setHidden(true);
		ui->doubleSpinBox_qty->setHidden(true);
		ui->label_sumText->setHidden(true);
		ui->doubleSpinBox_sum->setHidden(true);
	} else if(_type == TYPE_EXPENSE) {
		this->setWindowTitle(tr("Add expense"));
		ui->label_qtyText->setVisible(true);
		ui->doubleSpinBox_qty->setVisible(true);
		ui->label_sumText->setVisible(true);
		ui->doubleSpinBox_sum->setVisible(true);
		ui->doubleSpinBox_sum->setMaximumWidth(ui->doubleSpinBox_value->width() - 15);
		ui->doubleSpinBox_sum->resize(ui->doubleSpinBox_value->width() - 15, ui->doubleSpinBox_sum->height());
		ui->checkBox_isMonthStart->setChecked(false);
		ui->checkBox_isMonthStart->setVisible(false);
	}
	ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(verifyDialog()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(ui->doubleSpinBox_qty, SIGNAL(valueChanged(QString)), this, SLOT(onValuesChanged()));
	connect(ui->doubleSpinBox_value, SIGNAL(valueChanged(QString)), this, SLOT(onValuesChanged()));
	onValuesChanged();
}

AddIncomeExpenseDialog::~AddIncomeExpenseDialog() {
	qDebug() << "AddIncomeExpenseDialog::~AddIncomeExpenseDialog()";
	delete ui;
}

void AddIncomeExpenseDialog::setShowHelpText(bool show) {
	if(!show) {
		ui->label_helpText->setHidden(true);
	} else {
		ui->label_helpText->setVisible(true);
	}
}

void AddIncomeExpenseDialog::verifyDialog() {
	qDebug() << "AddIncomeExpenseDialog::verifyDialog()";
	if(ui->comboBox_category->currentText() == tr("### ADD NEW CATEGORY ###")) {
		emit(addCategoryRequested());
	} else {
		accept();
	}
}

void AddIncomeExpenseDialog::onValuesChanged() {
	qDebug() << "AddIncomeExpenseDialog::onValuesChanged()";
	qlonglong sumVal = Household::multiplyToRoundedInt(Household::qtyIntFromMoneyString(getQty()), Household::valueIntFromMoneyString(getValue()));
	ui->doubleSpinBox_sum->setValue(Household::stringFromValueInt(sumVal).toDouble());
}

void AddIncomeExpenseDialog::fillCategoriesComboBox(const QList<QString> cn) {
	qDebug() << "AddIncomeExpenseDialog::fillCategoriesComboBox()";
	ui->comboBox_category->clear();
	ui->comboBox_category->addItem(tr("### ADD NEW CATEGORY ###"));
	ui->comboBox_category->addItems(cn);
}

void AddIncomeExpenseDialog::setValue(const QString& v) {
	qDebug() << "AddIncomeExpenseDialog::setValue()";
	ui->doubleSpinBox_value->setValue(v.toDouble());
}

QString AddIncomeExpenseDialog::getValue() {
	qDebug() << "AddIncomeExpenseDialog::getValue()";
	return QString::number(ui->doubleSpinBox_value->value());
}


void AddIncomeExpenseDialog::setCategoryName(const QString& name) {
	qDebug() << "AddIncomeExpenseDialog::setCategoryName()";
	ui->comboBox_category->setCurrentIndex(ui->comboBox_category->findText(name, Qt::MatchExactly));
}

QString AddIncomeExpenseDialog::getCategoryName() {
	qDebug() << "AddIncomeExpenseDialog::getCategoryName()";
	return ui->comboBox_category->currentText().trimmed();
}

void AddIncomeExpenseDialog::setDateTime(const QDateTime& dt) {
	qDebug() << "AddIncomeExpenseDialog::setDateTime()";
	ui->dateTimeEdit->setDateTime(dt);
}

QDateTime AddIncomeExpenseDialog::getDateTime() {
	qDebug() << "AddIncomeExpenseDialog::getDateTime()";
	return ui->dateTimeEdit->dateTime();
}

void AddIncomeExpenseDialog::setQty(const QString& q) {
	qDebug() << "AddIncomeExpenseDialog::setQty()";
	ui->doubleSpinBox_qty->setValue(q.toDouble());
}

QString AddIncomeExpenseDialog::getQty() {
	qDebug() << "AddIncomeExpenseDialog::getQty()";
	return QString::number(ui->doubleSpinBox_qty->value());
}

int AddIncomeExpenseDialog::add() {
	qDebug() << "AddIncomeExpenseDialog::add()";
	this->show();
	ui->doubleSpinBox_qty->setValue(1.0f);
	if(ui->comboBox_account->count() > 1) {
		ui->label_accountText->setVisible(true);
		ui->comboBox_account->setVisible(true);
	} else {
		ui->label_accountText->setVisible(false);
		ui->comboBox_account->setVisible(false);
	}
	//ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
	ui->plainTextEdit_details->setPlainText("");
	ui->doubleSpinBox_qty->setValue(1.0f);
	ui->doubleSpinBox_value->setFocus();
	ui->doubleSpinBox_value->selectAll();
	onValuesChanged();
	return QDialog::exec();
}

int AddIncomeExpenseDialog::edit() {
	qDebug() << "AddIncomeExpenseDialog::edit()";
	this->show();
	if(ui->comboBox_account->count() > 1) {
		ui->label_accountText->setVisible(true);
		ui->comboBox_account->setVisible(true);
	} else {
		ui->label_accountText->setVisible(false);
		ui->comboBox_account->setVisible(false);
	}
	ui->doubleSpinBox_value->setFocus();
	ui->doubleSpinBox_value->selectAll();
	onValuesChanged();
	return QDialog::exec();
}

void AddIncomeExpenseDialog::fillAccountsComboBox(const QList<QString> acc) {
	qDebug() << "AddIncomeExpenseDialog::fillAccountsComboBox()";
	ui->comboBox_account->clear();
	ui->comboBox_account->addItems(acc);
}

void AddIncomeExpenseDialog::setAccount(const QString &acc) {
	qDebug() << "AddIncomeExpenseDialog::setAccount()";
	ui->comboBox_account->setCurrentIndex(ui->comboBox_account->findText(acc, Qt::MatchExactly));
}

QString AddIncomeExpenseDialog::getAccount() {
	qDebug() << "AddIncomeExpenseDialog::getAccount()";
	return ui->comboBox_account->currentText();
}

void AddIncomeExpenseDialog::setDetails(const QString &details) {
	qDebug() << "AddIncomeExpenseDialog::setDetails()";
	ui->plainTextEdit_details->setPlainText(details);
}

QString AddIncomeExpenseDialog::getDetails() {
	qDebug() << "AddIncomeExpenseDialog::getDetails()";
	return ui->plainTextEdit_details->toPlainText();
}

void AddIncomeExpenseDialog::hideAccounts() {
	qDebug() << "AddIncomeExpenseDialog::hideAccounts()";
	ui->comboBox_account->setVisible(false);
	ui->label_accountText->setVisible(false);
}

void AddIncomeExpenseDialog::showAccounts() {
	qDebug() << "AddIncomeExpenseDialog::showAccounts()";
	ui->comboBox_account->setVisible(true);
	ui->label_accountText->setVisible(true);
}

bool AddIncomeExpenseDialog::getIsMonthStart() {
	qDebug() << "AddIncomeExpenseDialog::getIsMonthStart()";
	return ui->checkBox_isMonthStart->isChecked();
}

void AddIncomeExpenseDialog::setIsMonthStart(const bool isIt) {
	qDebug() << "AddIncomeExpenseDialog::setIsMonthStart()";
	ui->checkBox_isMonthStart->setChecked(isIt);
}
