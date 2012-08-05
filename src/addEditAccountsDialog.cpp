#include "addEditAccountsDialog.h"
#include "ui_addEditAccountsDialog.h"
#include <QDebug>
#include <QMessageBox>

AddEditAccountsDialog::AddEditAccountsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddEditAccountsDialog) {
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(verifyDialog()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void AddEditAccountsDialog::verifyDialog() {
	qDebug() << "AddEditUserDialog::verifyDialog()";
	if(ui->lineEdit_name->text().isEmpty()) {
		QMessageBox::information(this, tr("Information"), tr("Account name must not be empty."));
		return;
	}
	accept();
}

AddEditAccountsDialog::~AddEditAccountsDialog() {
	delete ui;
}

void AddEditAccountsDialog::setShowHelpText(bool show) {
	if(!show) {
		ui->label_helpText->setHidden(true);
	} else {
		ui->label_helpText->setVisible(true);
	}
}

QString	AddEditAccountsDialog::getName() {
	qDebug() << "AddEditAccountsDialog::getName()";
	return ui->lineEdit_name->text();
}

void AddEditAccountsDialog::setName(const QString& name) {
	qDebug() << "AddEditAccountsDialog::setName()";
	ui->lineEdit_name->setText(name);
}

QString AddEditAccountsDialog::getDescription() {
	qDebug() << "AddEditAccountsDialog::getDescription()";
	return ui->plainTextEdit->toPlainText();
}

void AddEditAccountsDialog::setDescription(const QString& desc) {
	qDebug() << "AddEditAccountsDialog::setDescription()";
	ui->plainTextEdit->setPlainText(desc);
}
