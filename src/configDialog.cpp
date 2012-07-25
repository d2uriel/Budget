#include "configDialog.h"
#include "ui_configDialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ConfigDialog) {
	ui->setupUi(this);
}

ConfigDialog::~ConfigDialog() {
	delete ui;
}

void ConfigDialog::setAvailableLanguagePacks(const QList<QString>& languagePacks) {
	ui->comboBox_language->addItems(languagePacks);
}

QString ConfigDialog::getLanguage() {
	return ui->comboBox_language->currentText();
}

void ConfigDialog::setLanguage(const QString& language) {
	ui->comboBox_language->setCurrentIndex(ui->comboBox_language->findText(language, Qt::MatchExactly));
}
