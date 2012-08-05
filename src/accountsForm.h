#ifndef ACCOUNTSFORM_H
#define ACCOUNTSFORM_H

#include <QDialog>
#include "household.h"

namespace Ui {
class AccountsForm;
}

class AccountsForm : public QDialog
{
	Q_OBJECT
	
public:
	explicit AccountsForm(Household *h, QWidget *parent = 0);
	~AccountsForm();

	void setShowHelpText(bool show = true);

	void refreshAccountList();
	
private:
	Ui::AccountsForm *ui;
	void clear();
	Household *_household;

private slots:
	void onAddAccountClicked();
	void showContextMenu(QPoint p);
	void editAccount();
	void removeAccount();
	void setDefaultAccount();
};

#endif // ACCOUNTSFORM_H
