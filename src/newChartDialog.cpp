#include "newChartDialog.h"
#include "ui_newChartDialog.h"

NewChartDialog::NewChartDialog(QWidget *parent) : QDialog(parent), ui(new Ui::NewChartDialog) {
	ui->setupUi(this);
}

NewChartDialog::~NewChartDialog() {
	delete ui;
}
