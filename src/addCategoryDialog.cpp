#include "addCategoryDialog.h"
#include "ui_addCategoryDialog.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

AddCategoryDialog::AddCategoryDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddCategoryDialog) {
	qDebug() << "AddCategoryDialog::AddCategoryDialog()";
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(verifyDialog()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

AddCategoryDialog::~AddCategoryDialog() {
	qDebug() << "AddCategoryDialog::~AddCategoryDialog()";
	delete ui;
}

void AddCategoryDialog::verifyDialog() {
	qDebug() << "AddCategoryDialog::verifyDialog()";
	if(ui->lineEdit_categoryName->text().isEmpty()) {
		QMessageBox::information(this, tr("Information"), tr("Category name must not be empty."));
	} else {
		accept();
	}
}

void AddCategoryDialog::setCategoryName(const QString& name) {
	qDebug() << "AddCategoryDialog::setCategoryName()";
	ui->lineEdit_categoryName->setText(name);
}

QString AddCategoryDialog::getCategoryName() {
	qDebug() << "AddCategoryDialog::getCategoryName()";
	return ui->lineEdit_categoryName->text();
}

QString AddCategoryDialog::getParentName() {
	qDebug() << "AddCategoryDialog::getParentName()";
	return ui->comboBox_parentCategory->currentText();
}

void AddCategoryDialog::fillCategoriesComboBox(const QList<QString> cn) {
	qDebug() << "AddCategoryDialog::fillCategoriesComboBox()";
	ui->comboBox_parentCategory->clear();
	ui->comboBox_parentCategory->addItem("");
	ui->comboBox_parentCategory->addItems(cn);
}

void AddCategoryDialog::setCurrentParent(const QString& p) {
	qDebug() << "AddCategoryDialog::setCurrentParent()";
	ui->comboBox_parentCategory->setCurrentIndex(ui->comboBox_parentCategory->findText(p, Qt::MatchExactly));
}

void AddCategoryDialog::setDialogTitle(const QString &title) {
	qDebug() << "AddCategoryDialog::setDialogTitle()";
	setWindowTitle(title);
}
