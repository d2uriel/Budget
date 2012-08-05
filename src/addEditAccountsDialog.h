#ifndef ADDEDITACCOUNTSDIALOG_H
#define ADDEDITACCOUNTSDIALOG_H

#include <QDialog>

namespace Ui {
class AddEditAccountsDialog;
}

class AddEditAccountsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit AddEditAccountsDialog(QWidget *parent = 0);
	~AddEditAccountsDialog();

	void		setShowHelpText(bool show = true);

	QString		getName();
	void		setName(const QString& name);

	QString		getDescription();
	void		setDescription(const QString& desc);
	
private:
	Ui::AddEditAccountsDialog *ui;

private slots:
	void					verifyDialog();
};

#endif // ADDEDITACCOUNTSDIALOG_H
