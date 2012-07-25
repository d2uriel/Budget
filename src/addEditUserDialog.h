#ifndef ADDEDITUSERDIALOG_H
#define ADDEDITUSERDIALOG_H

#include <QDialog>

namespace Ui {
class AddEditUserDialog;
}

class AddEditUserDialog : public QDialog {
	Q_OBJECT
	
public:
	explicit AddEditUserDialog(QWidget *parent = 0);
	~AddEditUserDialog();

	QString		getName();
	void		setName(const QString& name);

	QString		getDescription();
	void		setDescription(const QString& desc);

	QString		getAccessLevel();
	void		setAccessLevel(const QString& level);

	QString		getPassword();
	void		setPassword(const QString& pwd);

	void		setAccessLevelDisabled();
	void		setPasswordsDisabled();

private:
	Ui::AddEditUserDialog *ui;

private slots:
	void		verifyDialog();
};

#endif // ADDEDITUSERDIALOG_H
