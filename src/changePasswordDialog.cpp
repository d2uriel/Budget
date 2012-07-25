#include "changePasswordDialog.h"
#include "ui_changePasswordDialog.h"

#include <QDebug>
#include <QMessageBox>

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ChangePasswordDialog) {
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(verifyDialog()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

ChangePasswordDialog::~ChangePasswordDialog() {
	delete ui;
}

void ChangePasswordDialog::verifyDialog() {
	qDebug() << "ChangePasswordDialog::verifyDialog()";
	if(ui->lineEdit_pwdNew->text() != ui->lineEdit_pwdNewRepeat->text()) {
		QMessageBox::information(this, tr("Information"), tr("Passwords do not match."));
		return;
	}
	accept();
}

QString ChangePasswordDialog::getCurrentPassword() {
	qDebug() << "ChangePasswordDialog::getCurrentPassword()";
	return ui->lineEdit_pwdOld->text();
}

QString ChangePasswordDialog::getNewPassword() {
	qDebug() << "ChangePasswordDialog::getNewPassword()";
	return ui->lineEdit_pwdNew->text();
}
