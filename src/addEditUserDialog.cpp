#include "addEditUserDialog.h"
#include "ui_addEditUserDialog.h"
#include "constants.h"
#include <QDebug>
#include <QMessageBox>

AddEditUserDialog::AddEditUserDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddEditUserDialog) {
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(verifyDialog()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

AddEditUserDialog::~AddEditUserDialog() {
	delete ui;
}

void AddEditUserDialog::setShowHelpText(bool show) {
	if(!show) {
		ui->label_helpText->setHidden(true);
	} else {
		ui->label_helpText->setVisible(true);
	}
}

void AddEditUserDialog::verifyDialog() {
	qDebug() << "AddEditUserDialog::verifyDialog()";
	if(ui->lineEdit_name->text().isEmpty()) {
		QMessageBox::information(this, tr("Information"), tr("User name must not be empty."));
		return;
	}
	if(ui->lineEdit_password->text() != ui->lineEdit_repeatPassword->text()) {
		QMessageBox::information(this, tr("Information"), tr("Passwords do not match."));
		return;
	}
	accept();
}

QString	AddEditUserDialog::getName() {
	qDebug() << "AddEditUserDialog::getName()";
	return ui->lineEdit_name->text();
}

void AddEditUserDialog::setName(const QString& name) {
	qDebug() << "AddEditUserDialog::setName()";
	ui->lineEdit_name->setText(name);
}

QString AddEditUserDialog::getDescription() {
	qDebug() << "AddEditUserDialog::getDescription()";
	return ui->lineEdit_description->text();
}

void AddEditUserDialog::setDescription(const QString& desc) {
	qDebug() << "AddEditUserDialog::setDescription()";
	ui->lineEdit_description->setText(desc);
}

QString AddEditUserDialog::getAccessLevel() {
	qDebug() << "AddEditUserDialog::getAccessLevel()";
	return ui->comboBox_userLevel->currentText();
}

void AddEditUserDialog::setAccessLevel(const QString& level) {
	qDebug() << "AddEditUserDialog::setAccessLevel()";
	ui->comboBox_userLevel->setCurrentIndex(ui->comboBox_userLevel->findText(level, Qt::MatchExactly));
}

QString AddEditUserDialog::getPassword() {
	qDebug() << "AddEditUserDialog::getPassword()";
	return ui->lineEdit_password->text();
}

void AddEditUserDialog::setPassword(const QString& pwd) {
	qDebug() << "AddEditUserDialog::setPassword()";
	ui->lineEdit_password->setText(pwd);
	ui->lineEdit_repeatPassword->setText(pwd);
}

void AddEditUserDialog::setAccessLevelDisabled() {
	qDebug() << "AddEditUserDialog::setAccessLevelDisabled()";
	ui->comboBox_userLevel->setDisabled(true);

}

void AddEditUserDialog::setPasswordsDisabled() {
	qDebug() << "AddEditUserDialog::setPasswordsDisabled()";
	ui->lineEdit_password->setVisible(false);
	ui->lineEdit_password->setDisabled(true);
	ui->lineEdit_repeatPassword->setVisible(false);
	ui->lineEdit_repeatPassword->setDisabled(true);
	ui->label_password->setVisible(false);
	ui->label_passwordRepeat->setVisible(false);
}
