#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
	Q_OBJECT
	
public:
	explicit LoginDialog(QWidget *parent = 0);
	~LoginDialog();

	QString getName();
	QString getPassword();
	
private:
	Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
