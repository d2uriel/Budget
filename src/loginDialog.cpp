#include "loginDialog.h"
#include "ui_loginDialog.h"
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDialog) {
	ui->setupUi(this);
}

LoginDialog::~LoginDialog() {
	delete ui;
}

QString LoginDialog::getName() {
	return ui->lineEdit_name->text();
}

QString LoginDialog::getPassword() {
	return ui->lineEdit_password->text();
}
