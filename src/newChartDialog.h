#ifndef NEWCHARTDIALOG_H
#define NEWCHARTDIALOG_H

#include <QDialog>

namespace Ui {
class NewChartDialog;
}

class NewChartDialog : public QDialog {
	Q_OBJECT
	
public:
	explicit NewChartDialog(QWidget *parent = 0);
	~NewChartDialog();
	
private:
	Ui::NewChartDialog *ui;
};

#endif // NEWCHARTDIALOG_H
