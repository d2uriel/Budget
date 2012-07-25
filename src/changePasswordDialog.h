#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class ChangePasswordDialog;
}

class ChangePasswordDialog : public QDialog {
	Q_OBJECT
	
public:
	explicit ChangePasswordDialog(QWidget *parent = 0);
	~ChangePasswordDialog();

	QString getCurrentPassword();
	QString getNewPassword();
	
private:
	Ui::ChangePasswordDialog *ui;

private slots:
	void verifyDialog();
};

#endif // CHANGEPASSWORDDIALOG_H
