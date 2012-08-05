#include "moneyTransferDialog.h"
#include "ui_moneyTransferDialog.h"

#include <QDebug>
#include <QMessageBox>

MoneyTransferDialog::MoneyTransferDialog(Household *h, QWidget *parent) : QDialog(parent), ui(new Ui::MoneyTransferDialog) {
	ui->setupUi(this);
	_household = h;
	connect(ui->pushButton_swap, SIGNAL(clicked()), this, SLOT(swapAccounts()));
	connect(ui->comboBox_accountFrom, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshAccountFromMoney(int)));
	connect(ui->comboBox_accountTo, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshAccountToMoney(int)));
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(verifyDialog()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

MoneyTransferDialog::~MoneyTransferDialog() {
	delete ui;
}

void MoneyTransferDialog::setShowHelpText(bool show) {
	if(!show) {
		ui->label_helpText->setHidden(true);
	} else {
		ui->label_helpText->setVisible(true);
	}
}

void MoneyTransferDialog::verifyDialog() {
	qDebug() << "MoneyTransferDialog::verifyDialog()";
	if(ui->comboBox_accountFrom->currentText() == ui->comboBox_accountTo->currentText()) {
		QMessageBox::information(this, tr("Information"), tr("You cannot transfer money to the same account."));
		return;
	}
	if(ui->doubleSpinBox_value->value() == 0.0) {
		QMessageBox::information(this, tr("Information"), tr("Transfer value must be greater than zero."));
		return;
	}
	accept();
}

void MoneyTransferDialog::setAccountFrom(const QString& from) {
	qDebug() << "MoneyTransferDialog::setAccountFrom()";
	ui->comboBox_accountFrom->setCurrentIndex(ui->comboBox_accountFrom->findText(from, Qt::MatchExactly));
}

QString MoneyTransferDialog::getAccountFrom() {
	qDebug() << "MoneyTransferDialog::getAccountFrom()";
	return ui->comboBox_accountFrom->currentText();
}

void MoneyTransferDialog::setAccountTo(const QString& to) {
	qDebug() << "MoneyTransferDialog::setAccountTo()";
	ui->comboBox_accountTo->setCurrentIndex(ui->comboBox_accountTo->findText(to, Qt::MatchExactly));
}

QString MoneyTransferDialog::getAccountTo() {
	qDebug() << "MoneyTransferDialog::getAccountTo()";
	return ui->comboBox_accountTo->currentText();
}

void MoneyTransferDialog::fillAccounts(const QList<QString> an) {
	qDebug() << "MoneyTransferDialog::fillAccounts()";
	ui->comboBox_accountFrom->addItems(an);
	ui->comboBox_accountFrom->setCurrentIndex(0);
	ui->comboBox_accountTo->addItems(an);
	ui->comboBox_accountTo->removeItem(ui->comboBox_accountTo->findText(ui->comboBox_accountFrom->currentText(), Qt::MatchExactly));
	ui->comboBox_accountTo->setCurrentIndex(0);
	ui->comboBox_accountFrom->removeItem(ui->comboBox_accountFrom->findText(ui->comboBox_accountTo->currentText(), Qt::MatchExactly));
}

void MoneyTransferDialog::swapAccounts() {
	qDebug() << "MoneyTransferDialog::swapAccounts()";
	QString currentFrom = ui->comboBox_accountFrom->currentText();
	QString currentTo = ui->comboBox_accountTo->currentText();
	const QList<QString> an = _household->getAccountNames();
	disconnect(ui->comboBox_accountFrom, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshAccountFromMoney(int)));
	disconnect(ui->comboBox_accountTo, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshAccountToMoney(int)));
	ui->comboBox_accountFrom->clear();
	ui->comboBox_accountTo->clear();
	ui->comboBox_accountFrom->addItems(an);
	ui->comboBox_accountTo->addItems(an);
	ui->comboBox_accountFrom->setCurrentIndex(ui->comboBox_accountFrom->findText(currentTo, Qt::MatchExactly));
	ui->comboBox_accountTo->setCurrentIndex(ui->comboBox_accountTo->findText(currentFrom, Qt::MatchExactly));
	ui->comboBox_accountTo->removeItem(ui->comboBox_accountTo->findText(ui->comboBox_accountFrom->currentText(), Qt::MatchExactly));
	ui->comboBox_accountFrom->removeItem(ui->comboBox_accountFrom->findText(ui->comboBox_accountTo->currentText(), Qt::MatchExactly));
	connect(ui->comboBox_accountFrom, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshAccountFromMoney(int)));
	connect(ui->comboBox_accountTo, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshAccountToMoney(int)));
	refreshAccountFromMoney(0);
	refreshAccountToMoney(0);
}

void MoneyTransferDialog::refreshAccountFromMoney(int index) {
	qDebug() << "MoneyTransferDialog::refreshAccountFromMoney()";
	ui->label_availableFundsFrom->setText(Household::stringFromValueInt(_household->getAccountMoney(_household->getAccountId(ui->comboBox_accountFrom->currentText()))));
	ui->doubleSpinBox_value->setMaximum(((double)_household->getAccountMoney(_household->getAccountId(ui->comboBox_accountFrom->currentText())) / 100.));
}

void MoneyTransferDialog::refreshAccountToMoney(int index) {
	qDebug() << "MoneyTransferDialog::refreshAccountToMoney()";
	ui->label_availableFundsTo->setText(Household::stringFromValueInt(_household->getAccountMoney(_household->getAccountId(ui->comboBox_accountTo->currentText()))));
}

QString MoneyTransferDialog::getValue() {
	qDebug() << "MoneyTransferDialog::getValue()";
	return QString::number(ui->doubleSpinBox_value->value());
}
